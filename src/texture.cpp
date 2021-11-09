#include "texture.h"


void Texture::setDevice(VkDevice _device, VkPhysicalDevice _physDevice, VkQueue _queue, VkCommandPool _commandPool, 
        VkSampler _sampler, VkDescriptorPool _descriptorPool, VkDescriptorSetLayout _descriptorSetLayout) {

    device = _device;
    physDevice = _physDevice;
    queue = _queue;
    commandPool = _commandPool;
    sampler = _sampler;
    descriptorPool = _descriptorPool;
    descriptorSetLayout = _descriptorSetLayout;
}


void Texture::setTexture(const Image &image_) {

    unsigned width = image_.getWidth();
    unsigned height = image_.getHeight();
    size_t bpp = 4 * sizeof(float);

    size_t imageSize = width * height * bpp;


    VkBuffer buffer;
    VkDeviceMemory bufferMemory;

    createImageBuffer(device, physDevice, imageSize, &buffer, &bufferMemory);

    void *mappedMemory;
    vkMapMemory(device, bufferMemory, 0, imageSize, 0, &mappedMemory);
    memcpy(mappedMemory, image_.getPixelArray(), imageSize);
    vkUnmapMemory(device, bufferMemory);

    createImage(device, physDevice, width, height, &image, &imageMemory);

    transitionImageLayout(device, queue, commandPool, image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    
    copyBufferToImage(device, queue, commandPool, buffer, image, width, height);
    
    transitionImageLayout(device, queue, commandPool, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    vkDestroyBuffer(device, buffer, nullptr);
    vkFreeMemory(device, bufferMemory, nullptr);


    createImageView(device, image, &imageView);

    createDescriptorSet(device, sampler, imageView, &descriptorSetLayout, descriptorPool, &descriptorSet);
}


void Texture::cleanup() {
    vkDestroyImageView(device, imageView, nullptr);

    vkDestroyImage(device, image, nullptr);
    vkFreeMemory(device, imageMemory, nullptr);
}


VkDescriptorSet Texture::getDescriptorSet() const {
    return descriptorSet;
}


void Texture::createImageBuffer(VkDevice a_device, VkPhysicalDevice a_physDevice, const size_t a_bufferSize,
        VkBuffer* a_pBuffer, VkDeviceMemory* a_pBufferMemory) {

    VkBufferCreateInfo bufferCreateInfo = {};
    bufferCreateInfo.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.size        = a_bufferSize; 
    bufferCreateInfo.usage       = VK_BUFFER_USAGE_TRANSFER_SRC_BIT; 
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE; 

    VK_CHECK_RESULT(vkCreateBuffer(a_device, &bufferCreateInfo, NULL, a_pBuffer)); 

    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(a_device, (*a_pBuffer), &memoryRequirements);

    VkMemoryAllocateInfo allocateInfo = {};
    allocateInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocateInfo.allocationSize  = memoryRequirements.size;
    allocateInfo.memoryTypeIndex = vk_utils::FindMemoryType(memoryRequirements.memoryTypeBits, 
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, a_physDevice);

    VK_CHECK_RESULT(vkAllocateMemory(a_device, &allocateInfo, NULL, a_pBufferMemory));
    
    VK_CHECK_RESULT(vkBindBufferMemory(a_device, (*a_pBuffer), (*a_pBufferMemory), 0));
}


void Texture::createImage(VkDevice a_device, VkPhysicalDevice a_physDevice, unsigned width, unsigned height,
        VkImage* a_pImage, VkDeviceMemory* a_pImageMemory) {

    VkImageCreateInfo imageCreateInfo = {};
    imageCreateInfo.sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageCreateInfo.imageType     = VK_IMAGE_TYPE_2D;
    imageCreateInfo.format        = VK_FORMAT_R32G32B32A32_SFLOAT;
    imageCreateInfo.extent.width  = width;
    imageCreateInfo.extent.height = height;
    imageCreateInfo.extent.depth  = 1;
    imageCreateInfo.mipLevels     = 1;
    imageCreateInfo.arrayLayers   = 1;
    imageCreateInfo.samples       = VK_SAMPLE_COUNT_1_BIT;
    imageCreateInfo.tiling        = VK_IMAGE_TILING_OPTIMAL;
    imageCreateInfo.usage         = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    imageCreateInfo.sharingMode   = VK_SHARING_MODE_EXCLUSIVE;
    imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    VK_CHECK_RESULT(vkCreateImage(a_device, &imageCreateInfo, NULL, a_pImage));

    VkMemoryRequirements memoryRequirements;
    vkGetImageMemoryRequirements(a_device, (*a_pImage), &memoryRequirements);

    VkMemoryAllocateInfo allocateInfo = {};
    allocateInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocateInfo.allocationSize  = memoryRequirements.size;
    allocateInfo.memoryTypeIndex = vk_utils::FindMemoryType(memoryRequirements.memoryTypeBits, 
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, a_physDevice);

    VK_CHECK_RESULT(vkAllocateMemory(a_device, &allocateInfo, NULL, a_pImageMemory));
    
    VK_CHECK_RESULT(vkBindImageMemory(a_device, (*a_pImage), (*a_pImageMemory), 0));

}


void Texture::transitionImageLayout(VkDevice a_device, VkQueue a_queue, VkCommandPool a_commandPool, 
        VkImage a_image, VkImageLayout a_oldLayout, VkImageLayout a_newLayout) {
    
    VkCommandBuffer commandBuffer = beginCommands(a_device, a_commandPool);

    VkImageMemoryBarrier barrier{};
    barrier.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout                       = a_oldLayout;
    barrier.newLayout                       = a_newLayout;
    barrier.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
    barrier.image                           = a_image;
    barrier.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel   = 0;
    barrier.subresourceRange.levelCount     = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount     = 1;

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if (a_oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && a_newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } else if (a_oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && a_newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } else {
        throw std::invalid_argument("unsupported layout transition!");
    }

    vkCmdPipelineBarrier(
        commandBuffer,
        sourceStage, destinationStage,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier
    );

    endCommands(a_device, a_queue, a_commandPool, commandBuffer);
}


void Texture::copyBufferToImage(VkDevice a_device, VkQueue a_queue, VkCommandPool a_commandPool, 
        VkBuffer buffer, VkImage image, unsigned width, unsigned height) {

    VkCommandBuffer commandBuffer = beginCommands(a_device, a_commandPool);

    VkBufferImageCopy region{};
    region.bufferOffset                    = 0;
    region.bufferRowLength                 = 0;
    region.bufferImageHeight               = 0;
    region.imageSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel       = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount     = 1;
    region.imageOffset                     = {0, 0, 0};
    region.imageExtent                     = {width, height, 1};

    vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    endCommands(a_device, a_queue, a_commandPool, commandBuffer);
}


VkCommandBuffer Texture::beginCommands(VkDevice a_device, VkCommandPool a_commandPool) {

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = a_commandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    VK_CHECK_RESULT(vkAllocateCommandBuffers(a_device, &allocInfo, &commandBuffer));

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    VK_CHECK_RESULT(vkBeginCommandBuffer(commandBuffer, &beginInfo));

    return commandBuffer;
}

void Texture::endCommands(VkDevice a_device, VkQueue a_queue, VkCommandPool a_commandPool, VkCommandBuffer a_commandBuffer) {
    
    vkEndCommandBuffer(a_commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers    = &a_commandBuffer;

    VkFence fence;
    VkFenceCreateInfo fenceCreateInfo = {};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.flags = 0;
    VK_CHECK_RESULT(vkCreateFence(a_device, &fenceCreateInfo, NULL, &fence));

    VK_CHECK_RESULT(vkQueueSubmit(a_queue, 1, &submitInfo, fence));

    VK_CHECK_RESULT(vkWaitForFences(a_device, 1, &fence, VK_TRUE, 1000000000000));

    vkDestroyFence(a_device, fence, NULL);

    vkFreeCommandBuffers(a_device, a_commandPool, 1, &a_commandBuffer);
}


void Texture::createImageView(VkDevice a_device, VkImage a_image, VkImageView * a_pImageView) {

    VkImageViewCreateInfo viewInfo = {};

    viewInfo.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image                           = a_image;
    viewInfo.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format                          = VK_FORMAT_R32G32B32A32_SFLOAT;
    viewInfo.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel   = 0;
    viewInfo.subresourceRange.levelCount     = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount     = 1;

    VK_CHECK_RESULT(vkCreateImageView(a_device, &viewInfo, nullptr, a_pImageView));
}

void Texture::createDescriptorSet(VkDevice a_device, VkSampler a_sampler, VkImageView a_imageView, 
        const VkDescriptorSetLayout* a_pDSLayout, VkDescriptorPool a_DSPool, VkDescriptorSet* a_pDS){

    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};
    descriptorSetAllocateInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descriptorSetAllocateInfo.descriptorPool     = a_DSPool;
    descriptorSetAllocateInfo.descriptorSetCount = 1;
    descriptorSetAllocateInfo.pSetLayouts        = a_pDSLayout;

    VK_CHECK_RESULT(vkAllocateDescriptorSets(a_device, &descriptorSetAllocateInfo, a_pDS));

    VkDescriptorImageInfo descriptorImageInfo = {};
    descriptorImageInfo.sampler = a_sampler;
    descriptorImageInfo.imageView = a_imageView;
    descriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    VkWriteDescriptorSet writeDescriptorSet = {};
    writeDescriptorSet.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDescriptorSet.dstSet          = (*a_pDS);
    writeDescriptorSet.dstBinding      = 0;
    writeDescriptorSet.descriptorCount = 1;
    writeDescriptorSet.descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    writeDescriptorSet.pImageInfo      = &descriptorImageInfo;

    vkUpdateDescriptorSets(a_device, 1, &writeDescriptorSet, 0, NULL);
}