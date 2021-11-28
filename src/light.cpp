#include "light.h" 

void Light::gen() {

    createShadowMapResources(device, physDevice, shadowMapRenderPass, 
            &shadowMapImage, &shadowMapImageMemory, &shadowMapImageView, 
            &shadowMapSampler, &shadowMapFramebuffer);

    createShadowMapDescriptorSet(device, shadowMapSampler, shadowMapImageView, &shadowMapSetLayout, 
            shadowMapPool, &shadowMapSet);
}


void Light::cleanup() { 

    vkFreeMemory           (device, shadowMapImageMemory, NULL);
    vkDestroyImage         (device, shadowMapImage, nullptr);
    vkDestroyImageView     (device, shadowMapImageView, nullptr);
    vkDestroySampler       (device, shadowMapSampler, NULL);
    vkDestroyFramebuffer   (device, shadowMapFramebuffer, nullptr);

}



void Light::setRender(LightNecessary lightNecessary) {

    device = lightNecessary.device;
    physDevice = lightNecessary.physDevice;

    shadowMapRenderPass = lightNecessary.shadowMapRenderPass;

    shadowMapPool = lightNecessary.shadowMapPool;
    shadowMapSetLayout = lightNecessary.shadowMapSetLayout;
}


VkDescriptorSet Light::getShadowMapSet() const {
    return shadowMapSet;
}

VkFramebuffer Light::getShadowMapFramebuffer() const {
    return shadowMapFramebuffer;
}


void Light::createShadowMapResources(VkDevice a_device, VkPhysicalDevice a_physDevice, 
        VkRenderPass a_renderPass, VkImage* a_pShadowImage, VkDeviceMemory* a_pImageMemory,
        VkImageView * a_pShadowImageView, VkSampler *a_pSampler, VkFramebuffer *a_framebuffer) {

    VkImageCreateInfo shadowMapImageCreateInfo = {};
    shadowMapImageCreateInfo.sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    shadowMapImageCreateInfo.imageType     = VK_IMAGE_TYPE_2D;
    shadowMapImageCreateInfo.format        = VK_FORMAT_D32_SFLOAT;
    shadowMapImageCreateInfo.extent.width  = SHADOWMAP_WIDTH;
    shadowMapImageCreateInfo.extent.height = SHADOWMAP_HEIGHT;
    shadowMapImageCreateInfo.extent.depth  = 1;
    shadowMapImageCreateInfo.mipLevels     = 1;
    shadowMapImageCreateInfo.arrayLayers   = 1;
    shadowMapImageCreateInfo.samples       = VK_SAMPLE_COUNT_1_BIT;
    shadowMapImageCreateInfo.tiling        = VK_IMAGE_TILING_OPTIMAL;
    shadowMapImageCreateInfo.usage         = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    shadowMapImageCreateInfo.sharingMode   = VK_SHARING_MODE_EXCLUSIVE;
    shadowMapImageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    VK_CHECK_RESULT(vkCreateImage(a_device, &shadowMapImageCreateInfo, NULL, a_pShadowImage));

    VkMemoryRequirements memoryRequirements;
    vkGetImageMemoryRequirements(a_device, (*a_pShadowImage), &memoryRequirements);

    VkMemoryAllocateInfo allocateInfo = {};
    allocateInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocateInfo.allocationSize  = memoryRequirements.size;
    allocateInfo.memoryTypeIndex = vk_utils::FindMemoryType(memoryRequirements.memoryTypeBits, 
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, a_physDevice);

    VK_CHECK_RESULT(vkAllocateMemory(a_device, &allocateInfo, NULL, a_pImageMemory));
    
    VK_CHECK_RESULT(vkBindImageMemory(a_device, (*a_pShadowImage), (*a_pImageMemory), 0));

    VkImageViewCreateInfo shadowViewInfo = {};
    shadowViewInfo.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    shadowViewInfo.image                           = (*a_pShadowImage);
    shadowViewInfo.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
    shadowViewInfo.format                          = VK_FORMAT_D32_SFLOAT;
    shadowViewInfo.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_DEPTH_BIT;
    shadowViewInfo.subresourceRange.baseMipLevel   = 0;
    shadowViewInfo.subresourceRange.levelCount     = 1;
    shadowViewInfo.subresourceRange.baseArrayLayer = 0;
    shadowViewInfo.subresourceRange.layerCount     = 1;
    VK_CHECK_RESULT(vkCreateImageView(a_device, &shadowViewInfo, nullptr, a_pShadowImageView));


    VkImageView attachments[] = { *a_pShadowImageView };

        
    VkFramebufferCreateInfo framebufferCreateInfo = {};
    framebufferCreateInfo.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferCreateInfo.renderPass      = a_renderPass;
    framebufferCreateInfo.attachmentCount = 1;
    framebufferCreateInfo.pAttachments    = attachments;
    framebufferCreateInfo.width           = SHADOWMAP_WIDTH;
    framebufferCreateInfo.height          = SHADOWMAP_HEIGHT;
    framebufferCreateInfo.layers          = 1;
    VK_CHECK_RESULT(vkCreateFramebuffer(a_device, &framebufferCreateInfo, nullptr, a_framebuffer));
    

    VkSamplerCreateInfo samplerCreateInfo = {};

    samplerCreateInfo.sType            = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerCreateInfo.magFilter        = VK_FILTER_NEAREST;
    samplerCreateInfo.minFilter        = VK_FILTER_NEAREST;
    samplerCreateInfo.mipmapMode       = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerCreateInfo.addressModeU     = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerCreateInfo.addressModeV     = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerCreateInfo.addressModeW     = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerCreateInfo.mipLodBias       = 0.0f;
    samplerCreateInfo.anisotropyEnable = VK_FALSE;
    samplerCreateInfo.maxAnisotropy    = 1.0f;
    samplerCreateInfo.minLod           = 0.0f;
    samplerCreateInfo.maxLod           = 1.0f;
    samplerCreateInfo.borderColor =             VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
    samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;
    
    VK_CHECK_RESULT(vkCreateSampler(a_device, &samplerCreateInfo, NULL, a_pSampler)); 
}

void Light::createShadowMapDescriptorSet(VkDevice a_device, VkSampler a_sampler, VkImageView a_imageView, 
        const VkDescriptorSetLayout* a_pDSLayout, VkDescriptorPool a_DSPool, VkDescriptorSet* a_pDS){

    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};
    descriptorSetAllocateInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descriptorSetAllocateInfo.descriptorPool     = a_DSPool;
    descriptorSetAllocateInfo.descriptorSetCount = 1;
    descriptorSetAllocateInfo.pSetLayouts        = a_pDSLayout;

    VK_CHECK_RESULT(vkAllocateDescriptorSets(a_device, &descriptorSetAllocateInfo, a_pDS));

    VkDescriptorImageInfo descriptorImageInfo = {};
    descriptorImageInfo.sampler     = a_sampler;
    descriptorImageInfo.imageView   = a_imageView;
    descriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;

    VkWriteDescriptorSet writeDescriptorSet = {};
    writeDescriptorSet.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDescriptorSet.dstSet          = (*a_pDS);
    writeDescriptorSet.dstBinding      = 0;
    writeDescriptorSet.descriptorCount = 1;
    writeDescriptorSet.descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    writeDescriptorSet.pImageInfo      = &descriptorImageInfo;


    vkUpdateDescriptorSets(a_device, 1, &writeDescriptorSet, 0, NULL);
}
