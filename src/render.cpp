#include "render.h" 

void Render::init(GLFWwindow *_window, Light *_light, std::vector<Object> *_objects) {
    window = _window;
    initVulkan();
    light = _light;
    objects = _objects;
    createResources();
}


void Render::initVulkan() {

    const int deviceId = 0;

    std::vector<const char*> extensions;
    {
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        extensions     = std::vector<const char*>(glfwExtensions, glfwExtensions + glfwExtensionCount);
    }

    instance = vk_utils::CreateInstance(enableValidationLayers, enabledLayers, extensions);
    if (enableValidationLayers)
        vk_utils::InitDebugReportCallback(instance, &debugReportCallbackFn, &debugReportCallback);

    if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS)
        throw std::runtime_error("glfwCreateWindowSurface: failed to create window surface!");

    physicalDevice = vk_utils::FindPhysicalDevice(instance, true, deviceId);
    auto queueFID  = vk_utils::GetQueueFamilyIndex(physicalDevice, VK_QUEUE_GRAPHICS_BIT);

    VkBool32 presentSupport = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, queueFID, surface, &presentSupport);
    if (!presentSupport)
        throw std::runtime_error("vkGetPhysicalDeviceSurfaceSupportKHR: no present support for the target device and graphics queue");

    device = vk_utils::CreateLogicalDevice(queueFID, physicalDevice, enabledLayers, deviceExtensions);
    vkGetDeviceQueue(device, queueFID, 0, &graphicsQueue);
    vkGetDeviceQueue(device, queueFID, 0, &presentQueue);

    // ==> commadnPool
    {
        VkCommandPoolCreateInfo poolInfo = {};
        poolInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.queueFamilyIndex = vk_utils::GetQueueFamilyIndex(physicalDevice, VK_QUEUE_GRAPHICS_BIT);
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;


        if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS)
            throw std::runtime_error("[CreateCommandPoolAndBuffers]: failed to create command pool!");
    }

    vk_utils::CreateCwapChain(physicalDevice, device, surface, WIDTH, HEIGHT,
            &screen);

    vk_utils::CreateScreenImageViews(device, &screen);
}


void Render::createResources() {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    createPosMatrixDescriptorPool(device, &posPool);
    createPosMatrixDescriptorSetLayout(device, &posSetLayout);

    createVertShaderBuffer(device, physicalDevice, 
            2 * sizeof(glm::mat4), &vertBufferPos, 2 * sizeof(glm::mat4), &vertBufferLight, 
            &vertBufferMemory, &vertOffset);
    createVertShaderDescriptorPool(device, &vertPool);
    createVertShaderDescriptorSetLayout(device, &vertSetLayout);
    createVertShaderDescriptorSet(device, vertBufferPos, 2 * sizeof(glm::mat4),
            vertBufferLight, 2 * sizeof(glm::mat4), &vertSetLayout,
            vertPool, &vertSet);

    createFragShaderBuffer(device, physicalDevice, 2 * sizeof(float),
            &fragBuffer, &fragBufferMemory);
    createFragShaderDescriptorPool(device, &fragPool);
    createFragShaderDescriptorSetLayout(device, &fragSetLayout);
    createFragShaderDescriptorSet(device, fragBuffer, 2 * sizeof(float), &fragSetLayout,
            fragPool, &fragSet);

    createTexShaderSampler(device, physicalDevice, &texSampler);

    createTexShaderDescriptorPool(device, &texPool);
    createTexShaderDescriptorSetLayout(device, &texSetLayout);

    createDepthResources(device, physicalDevice, screen.swapChainExtent, &depthImage, &depthImageMemory, 
            &depthImageView);

    createShadowDescriptorSetLayout(device, &shadowSetLayout);
    createShadowDescriptorPool(device, &shadowPool);
    createShadowBuffer(device, physicalDevice, 2 * sizeof(glm::mat4),
            &shadowBuffer, &shadowBufferMemory);
    createShadowDescriptorSet(device, shadowBuffer, 2 * sizeof(glm::mat4), &shadowSetLayout,
            shadowPool, &shadowSet);

    createShadowMapRenderPass(device, VK_FORMAT_D32_SFLOAT, &shadowMapRenderPass);
    
    createShadowMapPipeline(device, shadowMapRenderPass, &shadowMapPipelineLayout, &shadowMapPipeline,
            posSetLayout, shadowSetLayout);

    createShadowMapDescriptorPool(device, &shadowMapPool);
    createShadowMapDescriptorSetLayout(device, &shadowMapSetLayout);

    createRenderPass(device, screen.swapChainImageFormat, VK_FORMAT_D32_SFLOAT, &renderPass);
    createGraphicsPipeline(device, screen.swapChainExtent, renderPass, 
            &pipelineLayout, &graphicsPipeline, posSetLayout, vertSetLayout, texSetLayout, shadowMapSetLayout, fragSetLayout);

    CreateScreenFrameBuffers(device, renderPass, &screen, depthImageView);

    createSyncObjects(device, &m_sync);

    createCommandBuffers(device, commandPool, screen.swapChainFramebuffers, &commandBuffers);
}


void Render::updateRender() {
    waitExecQueue();

    writeCommandBuffers(device, 
            screen.swapChainFramebuffers, screen.swapChainExtent, 
            renderPass, graphicsPipeline, pipelineLayout, 
            &commandBuffers, objects,
            vertSet, fragSet, shadowSet, light,
            shadowMapRenderPass, shadowMapPipeline, shadowMapPipelineLayout);
}


void Render::cleanup() { 

    vkDestroyDescriptorPool(device, shadowPool, NULL);
    vkDestroyDescriptorSetLayout(device, shadowSetLayout, NULL);
    vkFreeMemory(device, shadowBufferMemory, NULL);
    vkDestroyBuffer(device, shadowBuffer, NULL); 

    vkFreeMemory(device, vertBufferMemory, NULL);
    vkDestroyBuffer(device, vertBufferPos, NULL); 
    vkDestroyBuffer(device, vertBufferLight, NULL); 
    vkDestroyDescriptorPool(device, vertPool, NULL);
    vkDestroyDescriptorSetLayout(device, vertSetLayout, NULL);


    vkFreeMemory(device, fragBufferMemory, NULL);
    vkDestroyBuffer(device, fragBuffer, NULL); 
    vkDestroyDescriptorPool(device, fragPool, NULL);
    vkDestroyDescriptorSetLayout(device, fragSetLayout, NULL);

    vkDestroyDescriptorPool(device, posPool, NULL);
    vkDestroyDescriptorSetLayout(device, posSetLayout, NULL);

    vkDestroyDescriptorPool(device, texPool, NULL);
    vkDestroyDescriptorSetLayout(device, texSetLayout, NULL);


    vkDestroyDescriptorPool(device, shadowMapPool, NULL);
    vkDestroyDescriptorSetLayout(device, shadowMapSetLayout, NULL);

    vkDestroySampler(device, texSampler, NULL);

    if (enableValidationLayers) {
        // destroy callback.
        auto func = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT");
        if (func == nullptr)
            throw std::runtime_error("Could not load vkDestroyDebugReportCallbackEXT");
        func(instance, debugReportCallback, NULL);
    }

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(device, m_sync.renderFinishedSemaphores[i], nullptr);
        vkDestroySemaphore(device, m_sync.imageAvailableSemaphores[i], nullptr);
        vkDestroyFence    (device, m_sync.inFlightFences[i], nullptr);
    }

    vkDestroyCommandPool(device, commandPool, nullptr);

    for (auto framebuffer : screen.swapChainFramebuffers) {
        vkDestroyFramebuffer(device, framebuffer, nullptr);
    }

    vkDestroyPipeline      (device, graphicsPipeline, nullptr);
    vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
    vkDestroyRenderPass    (device, renderPass, nullptr);

    vkDestroyPipeline      (device, shadowMapPipeline, nullptr);
    vkDestroyPipelineLayout(device, shadowMapPipelineLayout, nullptr);
    vkDestroyRenderPass    (device, shadowMapRenderPass, nullptr);

    for (auto imageView : screen.swapChainImageViews) {
        vkDestroyImageView(device, imageView, nullptr);
    }

    vkDestroyImageView(device, depthImageView, nullptr);
    vkDestroyImage(device, depthImage, nullptr);
    vkFreeMemory(device, depthImageMemory, nullptr);

    vkDestroySwapchainKHR(device, screen.swapChain, nullptr);
    vkDestroyDevice(device, nullptr);

    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyInstance(instance, nullptr);

}


void Render::waitExecQueue() {
    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkWaitForFences(device, 1, &m_sync.inFlightFences[i], VK_TRUE, UINT64_MAX);
    }
}


void Render::setVertBufferMemory(glm::mat4 *pos, glm::mat4 *rot, glm::mat4 *conv, glm::mat4 *proj) {
    void *mappedMemory = nullptr;

    vkMapMemory(device, vertBufferMemory, 0, 2 * sizeof(glm::mat4), 0, &mappedMemory);

    glm::mat4 *mats = (glm::mat4 *)mappedMemory;
    
    if (pos != nullptr) {
        mats[0] = *pos;
    }

    if (proj != nullptr) {
        mats[1] = *rot;
    }

    vkUnmapMemory(device, vertBufferMemory);

    vkMapMemory(device, vertBufferMemory, vertOffset, 2 * sizeof(glm::mat4), 0, &mappedMemory);

    mats = (glm::mat4 *)mappedMemory;

    if (conv != nullptr) {
        mats[0] = *conv;
    }

    if (proj != nullptr) {
        mats[1] = *proj;
    }

    vkUnmapMemory(device, vertBufferMemory);
}
void Render::setShadowBufferMemory(glm::mat4 *pos, glm::mat4 *rot) {
    void *mappedMemory = nullptr;
    
    vkMapMemory(device, shadowBufferMemory, 0, 2 * sizeof(glm::mat4), 0, &mappedMemory);
        
    glm::mat4 *mats = (glm::mat4 *)mappedMemory;
    
    if (pos != nullptr){
        mats[0] = *pos;
    }

    if (rot != nullptr){
        mats[1] = *rot;
    }

    vkUnmapMemory(device, shadowBufferMemory);

}
void Render::setFragBufferMemory(glm::vec3 *lightPos) {
    void *mappedMemory = nullptr;

    vkMapMemory(device, fragBufferMemory, 0, 3 * sizeof(float), 0, &mappedMemory);
        
    glm::vec3 *light = (glm::vec3 *)mappedMemory;

    light[0] = *lightPos;

    vkUnmapMemory(device, fragBufferMemory);
}

VkDevice Render::getDevice() const {
    return device;
}


ObjectNecessary Render::getObjectNecessary() const {
    ObjectNecessary objectNecessary;

    objectNecessary.device                 = device;
    objectNecessary.physDevice             = physicalDevice;
    objectNecessary.queue                  = graphicsQueue;
    objectNecessary.commandPool            = commandPool; 
    objectNecessary.sampler                = texSampler;
    objectNecessary.descriptorPool         = texPool; 
    objectNecessary.descriptorSetLayout    = texSetLayout; 
    objectNecessary.posDescriptorPool      = posPool; 
    objectNecessary.posDescriptorSetLayout = posSetLayout;

    return objectNecessary;
}


LightNecessary Render::getLightNecessary() const {
    LightNecessary lightNecessary;

    lightNecessary.device              = device;
    lightNecessary.physDevice          = physicalDevice;
    lightNecessary.shadowMapRenderPass = shadowMapRenderPass; 
    lightNecessary.shadowMapPool       = shadowMapPool;
    lightNecessary.shadowMapSetLayout  = shadowMapSetLayout;

    return lightNecessary;
}


void Render::createShadowMapDescriptorPool(VkDevice a_device, VkDescriptorPool* a_pDSPool) {

    VkDescriptorPoolSize descriptorPoolSize = {};
    descriptorPoolSize.type            = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorPoolSize.descriptorCount = 10;

    VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
    descriptorPoolCreateInfo.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolCreateInfo.maxSets       = 10;
    descriptorPoolCreateInfo.poolSizeCount = 1;
    descriptorPoolCreateInfo.pPoolSizes    = &descriptorPoolSize;

    VK_CHECK_RESULT(vkCreateDescriptorPool(a_device, &descriptorPoolCreateInfo, NULL, a_pDSPool));
}


void Render::createShadowMapDescriptorSetLayout(VkDevice a_device, VkDescriptorSetLayout* a_pDSLayout) {

    VkDescriptorSetLayoutBinding descriptorSetLayoutBinding = {};
    descriptorSetLayoutBinding.binding         = 0; 
    descriptorSetLayoutBinding.descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorSetLayoutBinding.descriptorCount = 1;
    descriptorSetLayoutBinding.stageFlags      = VK_SHADER_STAGE_FRAGMENT_BIT;

    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
    descriptorSetLayoutCreateInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorSetLayoutCreateInfo.bindingCount = 1;
    descriptorSetLayoutCreateInfo.pBindings    = &descriptorSetLayoutBinding;

    VK_CHECK_RESULT(vkCreateDescriptorSetLayout(a_device, &descriptorSetLayoutCreateInfo, NULL, a_pDSLayout));
}  


void Render::createPosMatrixDescriptorPool(VkDevice a_device, VkDescriptorPool* a_pDSPool) {

    VkDescriptorPoolSize descriptorPoolSize = {};
    descriptorPoolSize.type            = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorPoolSize.descriptorCount = 100;

    VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
    descriptorPoolCreateInfo.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolCreateInfo.maxSets       = 100;
    descriptorPoolCreateInfo.poolSizeCount = 1;
    descriptorPoolCreateInfo.pPoolSizes    = &descriptorPoolSize;

    VK_CHECK_RESULT(vkCreateDescriptorPool(a_device, &descriptorPoolCreateInfo, NULL, a_pDSPool));
}


void Render::createPosMatrixDescriptorSetLayout(VkDevice a_device, VkDescriptorSetLayout* a_pDSLayout) {

    VkDescriptorSetLayoutBinding descriptorSetLayoutBinding = {};
    descriptorSetLayoutBinding.binding         = 0; 
    descriptorSetLayoutBinding.descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorSetLayoutBinding.descriptorCount = 1;
    descriptorSetLayoutBinding.stageFlags      = VK_SHADER_STAGE_VERTEX_BIT;

    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
    descriptorSetLayoutCreateInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorSetLayoutCreateInfo.bindingCount = 1;
    descriptorSetLayoutCreateInfo.pBindings    = &descriptorSetLayoutBinding;

    VK_CHECK_RESULT(vkCreateDescriptorSetLayout(a_device, &descriptorSetLayoutCreateInfo, NULL, a_pDSLayout));
}   



void Render::createShadowBuffer(VkDevice a_device, VkPhysicalDevice a_physDevice, 
        const size_t a_bufferSize, VkBuffer* a_pBuffer, VkDeviceMemory* a_pBufferMemory) {

    VkBufferCreateInfo bufferCreateInfo = {};
    bufferCreateInfo.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.size        = a_bufferSize; 
    bufferCreateInfo.usage       = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT; 
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
    
    VK_CHECK_RESULT(vkBindBufferMemory(a_device, (*a_pBuffer) , (*a_pBufferMemory), 0));
}


void Render::createShadowDescriptorPool(VkDevice a_device, VkDescriptorPool* a_pDSPool) {

    VkDescriptorPoolSize descriptorPoolSize = {};
    descriptorPoolSize.type            = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorPoolSize.descriptorCount = 3;

    VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
    descriptorPoolCreateInfo.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolCreateInfo.maxSets       = 3;
    descriptorPoolCreateInfo.poolSizeCount = 1;
    descriptorPoolCreateInfo.pPoolSizes    = &descriptorPoolSize;

    VK_CHECK_RESULT(vkCreateDescriptorPool(a_device, &descriptorPoolCreateInfo, NULL, a_pDSPool));
}


void Render::createShadowDescriptorSetLayout(VkDevice a_device, VkDescriptorSetLayout* a_pDSLayout) {

    VkDescriptorSetLayoutBinding descriptorSetLayoutBinding = {};
    descriptorSetLayoutBinding.binding         = 0; 
    descriptorSetLayoutBinding.descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorSetLayoutBinding.descriptorCount = 1;
    descriptorSetLayoutBinding.stageFlags      = VK_SHADER_STAGE_VERTEX_BIT;

    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
    descriptorSetLayoutCreateInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorSetLayoutCreateInfo.bindingCount = 1;
    descriptorSetLayoutCreateInfo.pBindings    = &descriptorSetLayoutBinding;

    VK_CHECK_RESULT(vkCreateDescriptorSetLayout(a_device, &descriptorSetLayoutCreateInfo, NULL, a_pDSLayout));
}   


void Render::createShadowDescriptorSet(VkDevice a_device, VkBuffer a_buffer, size_t a_bufferSize, 
        const VkDescriptorSetLayout* a_pDSLayout, VkDescriptorPool a_DSPool, VkDescriptorSet* a_pDS) {

    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};
    descriptorSetAllocateInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descriptorSetAllocateInfo.descriptorPool     = a_DSPool;
    descriptorSetAllocateInfo.descriptorSetCount = 1;
    descriptorSetAllocateInfo.pSetLayouts        = a_pDSLayout;

    VK_CHECK_RESULT(vkAllocateDescriptorSets(a_device, &descriptorSetAllocateInfo, a_pDS));

    VkDescriptorBufferInfo descriptorBufferInfo = {};
    descriptorBufferInfo.buffer = a_buffer;
    descriptorBufferInfo.offset = 0;
    descriptorBufferInfo.range  = a_bufferSize;

    VkWriteDescriptorSet writeDescriptorSet = {};
    writeDescriptorSet.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDescriptorSet.dstSet          = (*a_pDS);
    writeDescriptorSet.dstBinding      = 0;
    writeDescriptorSet.descriptorCount = 1;
    writeDescriptorSet.descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    writeDescriptorSet.pBufferInfo     = &descriptorBufferInfo;


    vkUpdateDescriptorSets(a_device, 1, &writeDescriptorSet, 0, NULL);
}


void Render::createVertShaderBuffer(VkDevice a_device, VkPhysicalDevice a_physDevice, 
        const size_t a_bufferPosSize, VkBuffer* a_pBufferPos, 
        const size_t a_bufferLightSize, VkBuffer *a_pBufferLight, 
        VkDeviceMemory* a_pBufferMemory, unsigned *off) {

    VkBufferCreateInfo bufferCreateInfo = {};
    bufferCreateInfo.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.size        = a_bufferPosSize; 
    bufferCreateInfo.usage       = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT; 
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE; 

    VK_CHECK_RESULT(vkCreateBuffer(a_device, &bufferCreateInfo, NULL, a_pBufferPos)); 

    bufferCreateInfo.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.size        = a_bufferLightSize; 
    bufferCreateInfo.usage       = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT; 
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE; 

    VK_CHECK_RESULT(vkCreateBuffer(a_device, &bufferCreateInfo, NULL, a_pBufferLight)); 

    VkMemoryRequirements memoryRequirementsPos;
    vkGetBufferMemoryRequirements(a_device, (*a_pBufferPos), &memoryRequirementsPos);

    VkMemoryRequirements memoryRequirementsLight;
    vkGetBufferMemoryRequirements(a_device, (*a_pBufferLight), &memoryRequirementsLight);

    VkMemoryAllocateInfo allocateInfo = {};
    allocateInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocateInfo.allocationSize  = memoryRequirementsPos.size + memoryRequirementsLight.size;
    allocateInfo.memoryTypeIndex = vk_utils::FindMemoryType(memoryRequirementsPos.memoryTypeBits, 
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, a_physDevice);

    VK_CHECK_RESULT(vkAllocateMemory(a_device, &allocateInfo, NULL, a_pBufferMemory));
    
    VK_CHECK_RESULT(vkBindBufferMemory(a_device, (*a_pBufferPos)  , (*a_pBufferMemory), 0));
    VK_CHECK_RESULT(vkBindBufferMemory(a_device, (*a_pBufferLight), (*a_pBufferMemory), memoryRequirementsPos.size));

    *off = memoryRequirementsPos.size;
}


void Render::createVertShaderDescriptorPool(VkDevice a_device, VkDescriptorPool* a_pDSPool) {

    VkDescriptorPoolSize descriptorPoolSize = {};
    descriptorPoolSize.type            = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorPoolSize.descriptorCount = 3;

    VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
    descriptorPoolCreateInfo.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolCreateInfo.maxSets       = 3;
    descriptorPoolCreateInfo.poolSizeCount = 1;
    descriptorPoolCreateInfo.pPoolSizes    = &descriptorPoolSize;

    VK_CHECK_RESULT(vkCreateDescriptorPool(a_device, &descriptorPoolCreateInfo, NULL, a_pDSPool));
}


void Render::createVertShaderDescriptorSetLayout(VkDevice a_device, VkDescriptorSetLayout* a_pDSLayout) {

    VkDescriptorSetLayoutBinding descriptorSetLayoutBinding1 = {};
    descriptorSetLayoutBinding1.binding         = 0; 
    descriptorSetLayoutBinding1.descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorSetLayoutBinding1.descriptorCount = 1;
    descriptorSetLayoutBinding1.stageFlags      = VK_SHADER_STAGE_VERTEX_BIT;

    VkDescriptorSetLayoutBinding descriptorSetLayoutBinding2 = {};
    descriptorSetLayoutBinding2.binding         = 1; 
    descriptorSetLayoutBinding2.descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorSetLayoutBinding2.descriptorCount = 1;
    descriptorSetLayoutBinding2.stageFlags      = VK_SHADER_STAGE_FRAGMENT_BIT;

    VkDescriptorSetLayoutBinding binds[] = {descriptorSetLayoutBinding1, descriptorSetLayoutBinding2};

    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
    descriptorSetLayoutCreateInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorSetLayoutCreateInfo.bindingCount = 2;
    descriptorSetLayoutCreateInfo.pBindings    = binds;

    VK_CHECK_RESULT(vkCreateDescriptorSetLayout(a_device, &descriptorSetLayoutCreateInfo, NULL, a_pDSLayout));
}   


void Render::createVertShaderDescriptorSet(VkDevice a_device, VkBuffer a_bufferPos, size_t a_bufferPosSize, 
        VkBuffer a_bufferLight, size_t a_bufferLightSize,
        const VkDescriptorSetLayout* a_pDSLayout, VkDescriptorPool a_DSPool, VkDescriptorSet* a_pDS) {

    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};
    descriptorSetAllocateInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descriptorSetAllocateInfo.descriptorPool     = a_DSPool;
    descriptorSetAllocateInfo.descriptorSetCount = 1;
    descriptorSetAllocateInfo.pSetLayouts        = a_pDSLayout;

    VK_CHECK_RESULT(vkAllocateDescriptorSets(a_device, &descriptorSetAllocateInfo, a_pDS));

    VkDescriptorBufferInfo descriptorBufferInfo1 = {};
    descriptorBufferInfo1.buffer = a_bufferPos;
    descriptorBufferInfo1.offset = 0;
    descriptorBufferInfo1.range  = a_bufferPosSize;

    VkWriteDescriptorSet writeDescriptorSet1 = {};
    writeDescriptorSet1.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDescriptorSet1.dstSet          = (*a_pDS);
    writeDescriptorSet1.dstBinding      = 0;
    writeDescriptorSet1.descriptorCount = 1;
    writeDescriptorSet1.descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    writeDescriptorSet1.pBufferInfo     = &descriptorBufferInfo1;


    VkDescriptorBufferInfo descriptorBufferInfo2= {};
    descriptorBufferInfo2.buffer = a_bufferLight;
    descriptorBufferInfo2.offset = 0;
    descriptorBufferInfo2.range  = a_bufferLightSize;

    VkWriteDescriptorSet writeDescriptorSet2 = {};
    writeDescriptorSet2.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDescriptorSet2.dstSet          = (*a_pDS);
    writeDescriptorSet2.dstBinding      = 1;
    writeDescriptorSet2.descriptorCount = 1;
    writeDescriptorSet2.descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    writeDescriptorSet2.pBufferInfo     = &descriptorBufferInfo2;

    VkWriteDescriptorSet writeInfo[] = {writeDescriptorSet1, writeDescriptorSet2};

    vkUpdateDescriptorSets(a_device, 2, writeInfo, 0, NULL);
}


void Render::createTexShaderSampler(VkDevice a_device, VkPhysicalDevice a_physDevice, 
        VkSampler* a_pSampler) {

    VkPhysicalDeviceProperties properties{};

    vkGetPhysicalDeviceProperties(a_physDevice, &properties);

    VkSamplerCreateInfo samplerCreateInfo = {};

    samplerCreateInfo.sType =                   VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerCreateInfo.magFilter =               VK_FILTER_NEAREST;
    samplerCreateInfo.minFilter =               VK_FILTER_NEAREST;
    samplerCreateInfo.mipmapMode =              VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerCreateInfo.addressModeU =            VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerCreateInfo.addressModeV =            VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerCreateInfo.addressModeW =            VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerCreateInfo.anisotropyEnable =        VK_FALSE;
    samplerCreateInfo.maxAnisotropy =           properties.limits.maxSamplerAnisotropy;
    samplerCreateInfo.compareEnable =           VK_FALSE;
    samplerCreateInfo.compareOp =               VK_COMPARE_OP_ALWAYS;
    samplerCreateInfo.borderColor =             VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;
    

    VK_CHECK_RESULT(vkCreateSampler(a_device, &samplerCreateInfo, NULL, a_pSampler)); 
}


void Render::createTexShaderDescriptorPool(VkDevice a_device, VkDescriptorPool* a_pDSPool) {

    VkDescriptorPoolSize descriptorPoolSize = {};
    descriptorPoolSize.type            = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorPoolSize.descriptorCount = 100;

    VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
    descriptorPoolCreateInfo.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolCreateInfo.maxSets       = 100;
    descriptorPoolCreateInfo.poolSizeCount = 1;
    descriptorPoolCreateInfo.pPoolSizes    = &descriptorPoolSize;

    VK_CHECK_RESULT(vkCreateDescriptorPool(a_device, &descriptorPoolCreateInfo, NULL, a_pDSPool));
}


void Render::createTexShaderDescriptorSetLayout(VkDevice a_device, VkDescriptorSetLayout* a_pDSLayout) {

    VkDescriptorSetLayoutBinding descriptorSetLayoutBinding = {};
    descriptorSetLayoutBinding.binding         = 0; 
    descriptorSetLayoutBinding.descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorSetLayoutBinding.descriptorCount = 1;
    descriptorSetLayoutBinding.stageFlags      = VK_SHADER_STAGE_FRAGMENT_BIT;

    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
    descriptorSetLayoutCreateInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorSetLayoutCreateInfo.bindingCount = 1;
    descriptorSetLayoutCreateInfo.pBindings    = &descriptorSetLayoutBinding;

    VK_CHECK_RESULT(vkCreateDescriptorSetLayout(a_device, &descriptorSetLayoutCreateInfo, NULL, a_pDSLayout));
}  


void Render::createFragShaderBuffer(VkDevice a_device, VkPhysicalDevice a_physDevice, const size_t a_bufferSize,
        VkBuffer* a_pBuffer, VkDeviceMemory* a_pBufferMemory) {

    VkBufferCreateInfo bufferCreateInfo = {};
    bufferCreateInfo.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.size        = a_bufferSize; 
    bufferCreateInfo.usage       = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT; 
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


void Render::createFragShaderDescriptorPool(VkDevice a_device, VkDescriptorPool* a_pDSPool) {

    VkDescriptorPoolSize descriptorPoolSize = {};
    descriptorPoolSize.type            = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorPoolSize.descriptorCount = 1;

    VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
    descriptorPoolCreateInfo.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolCreateInfo.maxSets       = 1;
    descriptorPoolCreateInfo.poolSizeCount = 1;
    descriptorPoolCreateInfo.pPoolSizes    = &descriptorPoolSize;

    VK_CHECK_RESULT(vkCreateDescriptorPool(a_device, &descriptorPoolCreateInfo, NULL, a_pDSPool));
}


void Render::createFragShaderDescriptorSetLayout(VkDevice a_device, VkDescriptorSetLayout* a_pDSLayout) {

    VkDescriptorSetLayoutBinding descriptorSetLayoutBinding = {};
    descriptorSetLayoutBinding.binding         = 0; 
    descriptorSetLayoutBinding.descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorSetLayoutBinding.descriptorCount = 1;
    descriptorSetLayoutBinding.stageFlags      = VK_SHADER_STAGE_FRAGMENT_BIT;

    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
    descriptorSetLayoutCreateInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorSetLayoutCreateInfo.bindingCount = 1;
    descriptorSetLayoutCreateInfo.pBindings    = &descriptorSetLayoutBinding;

    VK_CHECK_RESULT(vkCreateDescriptorSetLayout(a_device, &descriptorSetLayoutCreateInfo, NULL, a_pDSLayout));
}   


void Render::createFragShaderDescriptorSet(VkDevice a_device, VkBuffer a_buffer, size_t a_bufferSize, 
        const VkDescriptorSetLayout* a_pDSLayout, VkDescriptorPool a_DSPool, VkDescriptorSet* a_pDS) {

    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};
    descriptorSetAllocateInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descriptorSetAllocateInfo.descriptorPool     = a_DSPool;
    descriptorSetAllocateInfo.descriptorSetCount = 1;
    descriptorSetAllocateInfo.pSetLayouts        = a_pDSLayout;

    VK_CHECK_RESULT(vkAllocateDescriptorSets(a_device, &descriptorSetAllocateInfo, a_pDS));

    VkDescriptorBufferInfo descriptorBufferInfo = {};
    descriptorBufferInfo.buffer = a_buffer;
    descriptorBufferInfo.offset = 0;
    descriptorBufferInfo.range  = a_bufferSize;

    VkWriteDescriptorSet writeDescriptorSet = {};
    writeDescriptorSet.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDescriptorSet.dstSet          = (*a_pDS);
    writeDescriptorSet.dstBinding      = 0;
    writeDescriptorSet.descriptorCount = 1;
    writeDescriptorSet.descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    writeDescriptorSet.pBufferInfo     = &descriptorBufferInfo;

    vkUpdateDescriptorSets(a_device, 1, &writeDescriptorSet, 0, NULL);
}


void Render::createRenderPass(VkDevice a_device, VkFormat a_swapChainImageFormat, VkFormat a_depthImageFormat,
        VkRenderPass* a_pRenderPass) {

    VkAttachmentDescription colorAttachment = {};
    colorAttachment.format         = a_swapChainImageFormat;
    colorAttachment.samples        = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentDescription depthAttachment = {};
    depthAttachment.format         = a_depthImageFormat;
    depthAttachment.samples        = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp        = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout    = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference colorAttachmentRef = {};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentRef{};
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout     = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount    = 1;
    subpass.pColorAttachments       = &colorAttachmentRef;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;

    VkSubpassDependency dependency = {};
    dependency.srcSubpass    = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass    = 0;
    dependency.srcStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | 
            VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    VkAttachmentDescription attachments[] = {colorAttachment, depthAttachment};

    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 2;
    renderPassInfo.pAttachments    = attachments;
    renderPassInfo.subpassCount    = 1;
    renderPassInfo.pSubpasses      = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies   = &dependency;

    if (vkCreateRenderPass(a_device, &renderPassInfo, nullptr, a_pRenderPass) != VK_SUCCESS)
        throw std::runtime_error("[CreateRenderPass]: failed to create render pass!");
}


void Render::createShadowMapRenderPass(VkDevice a_device, VkFormat a_depthImageFormat,
        VkRenderPass* a_pRenderPass) {

    VkAttachmentDescription depthAttachment = {};
    depthAttachment.format         = a_depthImageFormat;
    depthAttachment.samples        = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
    depthAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout    = VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentRef{};
    depthAttachmentRef.attachment = 0;
    depthAttachmentRef.layout     = VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount    = 0;
    subpass.pColorAttachments       = nullptr;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;

    VkSubpassDependency dependency = {};
    dependency.srcSubpass    = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass    = 0;
    dependency.srcStageMask  = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask  = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    VkAttachmentDescription attachments[] = {depthAttachment};

    VkSubpassDependency dependencies[] = {dependency};

    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments    = attachments;
    renderPassInfo.subpassCount    = 1;
    renderPassInfo.pSubpasses      = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies   = dependencies;

    if (vkCreateRenderPass(a_device, &renderPassInfo, nullptr, a_pRenderPass) != VK_SUCCESS)
        throw std::runtime_error("[CreateRenderPass]: failed to create render pass!");
}


void Render::createGraphicsPipeline(VkDevice a_device, VkExtent2D a_screenExtent, VkRenderPass a_renderPass,
        VkPipelineLayout* a_pLayout, VkPipeline* a_pPipiline,
        const VkDescriptorSetLayout &a_pdsLayout, 
        const VkDescriptorSetLayout &a_vdsLayout, 
        const VkDescriptorSetLayout &a_tdsLayout,
        const VkDescriptorSetLayout &a_smdsLayout,
        const VkDescriptorSetLayout &a_fdsLayout) {

    auto vertShaderCode = vk_utils::ReadFile("shaders/vert.spv");
    auto fragShaderCode = vk_utils::ReadFile("shaders/frag.spv");

    VkShaderModule vertShaderModule = vk_utils::CreateShaderModule(a_device, vertShaderCode);
    VkShaderModule fragShaderModule = vk_utils::CreateShaderModule(a_device, fragShaderCode);

    VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
    vertShaderStageInfo.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage  = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName  = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
    fragShaderStageInfo.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage  = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName  = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };


    VkVertexInputBindingDescription vInputBinding = { };
    vInputBinding.binding   = 0;
    vInputBinding.stride    = sizeof(PolygonPoint);
    vInputBinding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    VkVertexInputAttributeDescription vAttributeVertex = {};
    vAttributeVertex.binding  = 0;
    vAttributeVertex.location = 0;
    vAttributeVertex.format   = VK_FORMAT_R32G32B32_SFLOAT;
    vAttributeVertex.offset   = 0;

    VkVertexInputAttributeDescription vAttributeVertexTexture = {};
    vAttributeVertexTexture.binding  = 0;
    vAttributeVertexTexture.location = 1;
    vAttributeVertexTexture.format   = VK_FORMAT_R32G32_SFLOAT;
    vAttributeVertexTexture.offset   = sizeof(float) * 3;

    VkVertexInputAttributeDescription vAttributeVertexNormal = {};
    vAttributeVertexNormal.binding  = 0;
    vAttributeVertexNormal.location = 2;
    vAttributeVertexNormal.format   = VK_FORMAT_R32G32B32_SFLOAT;
    vAttributeVertexNormal.offset   = sizeof(float) * 5;

    VkVertexInputAttributeDescription vAttribute[] = {
        vAttributeVertex, vAttributeVertexTexture, vAttributeVertexNormal
    };

    VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount   = 1;
    vertexInputInfo.vertexAttributeDescriptionCount = 3;
    vertexInputInfo.pVertexBindingDescriptions      = &vInputBinding;
    vertexInputInfo.pVertexAttributeDescriptions    = vAttribute;
    
    VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
    inputAssembly.sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkViewport viewport = {};
    viewport.x        = 0.0f;
    viewport.y        = 0.0f;
    viewport.width    = (float)a_screenExtent.width;
    viewport.height   = (float)a_screenExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor = {};
    scissor.offset = { 0, 0 };
    scissor.extent = a_screenExtent;

    VkPipelineViewportStateCreateInfo viewportState = {};
    viewportState.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports    = &viewport;
    viewportState.scissorCount  = 1;
    viewportState.pScissors     = &scissor;

    VkPipelineRasterizationStateCreateInfo rasterizer = {};
    rasterizer.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable        = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode             = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth               = 1.0f;
    rasterizer.cullMode                = VK_CULL_MODE_FRONT_BIT; // VK_CULL_MODE_NONE;
    rasterizer.frontFace               = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable         = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo multisampling = {};
    multisampling.sType                = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable  = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineDepthStencilStateCreateInfo depthStencil = {};
    depthStencil.sType                 = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable       = VK_TRUE;
    depthStencil.depthWriteEnable      = VK_TRUE;
    depthStencil.depthCompareOp        = VK_COMPARE_OP_LESS_OR_EQUAL;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.stencilTestEnable     = VK_FALSE;
    depthStencil.back.failOp           = VK_STENCIL_OP_KEEP;
    depthStencil.back.passOp           = VK_STENCIL_OP_KEEP;
    depthStencil.back.compareOp        = VK_COMPARE_OP_ALWAYS;
    depthStencil.front                 = depthStencil.back;
    
    VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable    = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo colorBlending = {};
    colorBlending.sType             = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable     = VK_FALSE;
    colorBlending.logicOp           = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount   = 1;
    colorBlending.pAttachments      = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;

    VkDescriptorSetLayout dsLayout[] = {a_pdsLayout, a_vdsLayout, a_tdsLayout, a_smdsLayout, a_fdsLayout};

    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
    pipelineLayoutInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount         = 5;
    pipelineLayoutInfo.pSetLayouts            = dsLayout;
    pipelineLayoutInfo.pushConstantRangeCount = 0;

    if (vkCreatePipelineLayout(a_device, &pipelineLayoutInfo, nullptr, a_pLayout) != VK_SUCCESS)
        throw std::runtime_error("[CreateGraphicsPipeline]: failed to create pipeline layout!");

    VkGraphicsPipelineCreateInfo pipelineInfo = {};
    pipelineInfo.sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount          = 2;
    pipelineInfo.pStages             = shaderStages;
    pipelineInfo.pVertexInputState   = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState      = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState   = &multisampling;
    pipelineInfo.pDepthStencilState  = &depthStencil;
    pipelineInfo.pColorBlendState    = &colorBlending;
    pipelineInfo.layout              = (*a_pLayout);
    pipelineInfo.renderPass          = a_renderPass;
    pipelineInfo.subpass             = 0;
    pipelineInfo.basePipelineHandle  = VK_NULL_HANDLE;

    if (vkCreateGraphicsPipelines(a_device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, a_pPipiline) != VK_SUCCESS)
        throw std::runtime_error("[CreateGraphicsPipeline]: failed to create graphics pipeline!");

    vkDestroyShaderModule(a_device, fragShaderModule, nullptr);
    vkDestroyShaderModule(a_device, vertShaderModule, nullptr);
}


void Render::createShadowMapPipeline(VkDevice a_device, VkRenderPass a_renderPass,
        VkPipelineLayout* a_pLayout, VkPipeline* a_pPipiline,
        const VkDescriptorSetLayout &a_pdsLayout, 
        const VkDescriptorSetLayout &a_vdsLayout) {


    auto vertShaderCode = vk_utils::ReadFile("shaders/shadow_vert.spv");

    VkShaderModule vertShaderModule = vk_utils::CreateShaderModule(a_device, vertShaderCode);

    VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
    vertShaderStageInfo.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage  = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName  = "main";


    VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo };


    VkVertexInputBindingDescription vInputBinding = { };
    vInputBinding.binding   = 0;
    vInputBinding.stride    = sizeof(PolygonPoint);
    vInputBinding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    VkVertexInputAttributeDescription vAttributeVertex = {};
    vAttributeVertex.binding  = 0;
    vAttributeVertex.location = 0;
    vAttributeVertex.format   = VK_FORMAT_R32G32B32_SFLOAT;
    vAttributeVertex.offset   = 0;

    VkVertexInputAttributeDescription vAttributeVertexTexture = {};
    vAttributeVertexTexture.binding  = 0;
    vAttributeVertexTexture.location = 1;
    vAttributeVertexTexture.format   = VK_FORMAT_R32G32_SFLOAT;
    vAttributeVertexTexture.offset   = sizeof(float) * 3;

    VkVertexInputAttributeDescription vAttributeVertexNormal = {};
    vAttributeVertexNormal.binding  = 0;
    vAttributeVertexNormal.location = 2;
    vAttributeVertexNormal.format   = VK_FORMAT_R32G32B32_SFLOAT;
    vAttributeVertexNormal.offset   = sizeof(float) * 5;

    VkVertexInputAttributeDescription vAttribute[] = {
        vAttributeVertex, vAttributeVertexTexture, vAttributeVertexNormal
    };

    VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount   = 1;
    vertexInputInfo.vertexAttributeDescriptionCount = 3;
    vertexInputInfo.pVertexBindingDescriptions      = &vInputBinding;
    vertexInputInfo.pVertexAttributeDescriptions    = vAttribute;
    
    VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
    inputAssembly.sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkViewport viewport = {};
    viewport.x        = 0.0f;
    viewport.y        = 0.0f;
    viewport.width    = (float)SHADOWMAP_WIDTH;
    viewport.height   = (float)SHADOWMAP_HEIGHT;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor = {};
    scissor.offset = { 0, 0 };
    scissor.extent.width = SHADOWMAP_WIDTH;
    scissor.extent.height = SHADOWMAP_HEIGHT;

    VkPipelineViewportStateCreateInfo viewportState = {};
    viewportState.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports    = &viewport;
    viewportState.scissorCount  = 1;
    viewportState.pScissors     = &scissor;

    VkPipelineRasterizationStateCreateInfo rasterizer = {};
    rasterizer.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable        = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode             = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth               = 1.0f;
    rasterizer.cullMode                = VK_CULL_MODE_FRONT_BIT; // VK_CULL_MODE_NONE;
    rasterizer.frontFace               = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable         = VK_TRUE;

    VkPipelineMultisampleStateCreateInfo multisampling = {};
    multisampling.sType                = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable  = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineDepthStencilStateCreateInfo depthStencil = {};
    depthStencil.sType                 = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable       = VK_TRUE;
    depthStencil.depthWriteEnable      = VK_TRUE;
    depthStencil.depthCompareOp        = VK_COMPARE_OP_LESS_OR_EQUAL;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.stencilTestEnable     = VK_FALSE;
    depthStencil.back.failOp           = VK_STENCIL_OP_KEEP;
    depthStencil.back.passOp           = VK_STENCIL_OP_KEEP;
    depthStencil.back.compareOp        = VK_COMPARE_OP_ALWAYS;
    depthStencil.front                 = depthStencil.back;

    VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable    = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo colorBlending = {};
    colorBlending.sType             = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable     = VK_FALSE;
    colorBlending.logicOp           = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount   = 1;
    colorBlending.pAttachments      = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;

    VkDescriptorSetLayout dsLayout[] = {a_pdsLayout, a_vdsLayout};


    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
    pipelineLayoutInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount         = 2;
    pipelineLayoutInfo.pSetLayouts            = dsLayout;
    pipelineLayoutInfo.pushConstantRangeCount = 0;

    if (vkCreatePipelineLayout(a_device, &pipelineLayoutInfo, nullptr, a_pLayout) != VK_SUCCESS)
        throw std::runtime_error("[CreateShadowPipeline]: failed to create pipeline layout!");

    VkGraphicsPipelineCreateInfo pipelineInfo = {};
    pipelineInfo.sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount          = 1;
    pipelineInfo.pStages             = shaderStages;
    pipelineInfo.pVertexInputState   = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState      = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState   = &multisampling;
    pipelineInfo.pDepthStencilState  = &depthStencil;
    pipelineInfo.pColorBlendState    = &colorBlending;
    pipelineInfo.layout              = (*a_pLayout);
    pipelineInfo.renderPass          = a_renderPass;
    pipelineInfo.subpass             = 0;
    pipelineInfo.basePipelineHandle  = VK_NULL_HANDLE;

    if (vkCreateGraphicsPipelines(a_device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, a_pPipiline) != VK_SUCCESS)
        throw std::runtime_error("[CreateShadowPipeline]: failed to create graphics pipeline!");

    vkDestroyShaderModule(a_device, vertShaderModule, nullptr);
}



void Render::createCommandBuffers(VkDevice a_device, VkCommandPool a_cmdPool, 
        std::vector<VkFramebuffer> a_swapChainFramebuffers, std::vector<VkCommandBuffer>* a_cmdBuffers) {


    std::vector<VkCommandBuffer>& commandBuffers = (*a_cmdBuffers);

    commandBuffers.resize(a_swapChainFramebuffers.size());

    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool        = a_cmdPool;
    allocInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

    if (vkAllocateCommandBuffers(a_device, &allocInfo, commandBuffers.data()) != VK_SUCCESS)
        throw std::runtime_error("[CreateCommandPoolAndBuffers]: failed to allocate command buffers!");
}


void Render::writeCommandBuffers(VkDevice a_device, 
        std::vector<VkFramebuffer> a_swapChainFramebuffers, VkExtent2D a_frameBufferExtent,
        VkRenderPass a_renderPass, VkPipeline a_graphicsPipeline, VkPipelineLayout a_layout, 
        std::vector<VkCommandBuffer>* a_cmdBuffers, std::vector<Object> *a_obj,
        const VkDescriptorSet &a_vds, 
        const VkDescriptorSet &a_fds,
        const VkDescriptorSet &a_sds,
        Light *a_light,        
        VkRenderPass a_shadowMapRenderPass, VkPipeline a_shadowMapPipeline, VkPipelineLayout a_shadowMapLayout) {


    std::vector<VkCommandBuffer>& commandBuffers = (*a_cmdBuffers);

    for (size_t i = 0; i < commandBuffers.size(); i++) {


        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS) 
            throw std::runtime_error("[CreateCommandPoolAndBuffers]: failed to begin recording command buffer!");

        /// Shadow

        VkRenderPassBeginInfo renderPassInfo = {};
        renderPassInfo.sType                    = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass               = a_shadowMapRenderPass;
        renderPassInfo.framebuffer              = a_light->getShadowMapFramebuffer();
        renderPassInfo.renderArea.offset        = {0, 0};
        renderPassInfo.renderArea.extent.width  = SHADOWMAP_WIDTH;
        renderPassInfo.renderArea.extent.height = SHADOWMAP_HEIGHT;

        VkClearValue clearColor = {};
        clearColor.color        = {0.5f, 0.5f, 0.5f, 1.0f};
        VkClearValue clearDepth = {};
        clearDepth.depthStencil = {1.0f, 0};

        VkClearValue clearValues[] = {clearColor, clearDepth};
        
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues    = &clearDepth;

        vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, a_shadowMapPipeline);

        for (size_t j = 0; j < a_obj->size(); j++){

            {
                VkBuffer vertexBuffers[] = { (*a_obj)[j].getBuffer() };
                VkDeviceSize offsets[]   = { 0 };
                vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, vertexBuffers, offsets);
            }

            VkDescriptorSet ds[] = {(*a_obj)[j].getPosDescriptorSet(), a_sds};

            vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, a_shadowMapLayout, 0, 2, ds, 0, NULL);

            //for (size_t pol = 0; pol < (*a_obj)[j].getPolygonsSize(); pol++) {
            vkCmdDraw(commandBuffers[i], 3 * (*a_obj)[j].getPolygonsSize(), 1, 0, 0);
            //}
        }


        vkCmdEndRenderPass(commandBuffers[i]);

        ///Graphic


        //VkRenderPassBeginInfo renderPassInfo = {};
        renderPassInfo                   = {};
        renderPassInfo.sType             = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass        = a_renderPass;
        renderPassInfo.framebuffer       = a_swapChainFramebuffers[i];
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = a_frameBufferExtent;

        
        renderPassInfo.clearValueCount = 2;
        renderPassInfo.pClearValues    = clearValues;

        vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, a_graphicsPipeline);

        for (size_t j = 0; j < a_obj->size(); j++) {

            {
                VkBuffer vertexBuffers[] = { (*a_obj)[j].getBuffer() };
                VkDeviceSize offsets[]   = { 0 };
                vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, vertexBuffers, offsets);
            }

            VkDescriptorSet ds[] = {(*a_obj)[j].getPosDescriptorSet(), a_vds, 
                    (*a_obj)[j].getPolygonDescriptorSet(), a_light->getShadowMapSet(), a_fds};

            vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, a_layout, 0, 5, ds, 0, NULL);
            
            vkCmdDraw(commandBuffers[i], 3 * (*a_obj)[j].getPolygonsSize(), 1, 0, 0);
        }


        vkCmdEndRenderPass(commandBuffers[i]);
    

        if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to record command buffer!");
        }
    }
}



void Render::createSyncObjects(VkDevice a_device, SyncObj* a_pSyncObjs) {

    a_pSyncObjs->imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    a_pSyncObjs->renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    a_pSyncObjs->inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo = {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if     (vkCreateSemaphore(a_device, &semaphoreInfo, nullptr, &a_pSyncObjs->imageAvailableSemaphores[i]) != VK_SUCCESS ||
                vkCreateSemaphore(a_device, &semaphoreInfo, nullptr, &a_pSyncObjs->renderFinishedSemaphores[i]) != VK_SUCCESS ||
                vkCreateFence    (a_device, &fenceInfo,     nullptr, &a_pSyncObjs->inFlightFences[i]) != VK_SUCCESS) {
            throw std::runtime_error("[CreateSyncObjects]: failed to create synchronization objects for a frame!");
        }
    }
}


void Render::createDepthResources(VkDevice a_device, VkPhysicalDevice a_physDevice, VkExtent2D a_screenExtent,
        VkImage* a_pImage, VkDeviceMemory* a_pImageMemory, VkImageView * a_pImageView) {

    VkImageCreateInfo imageCreateInfo = {};
    imageCreateInfo.sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageCreateInfo.imageType     = VK_IMAGE_TYPE_2D;
    imageCreateInfo.format        = VK_FORMAT_D32_SFLOAT;
    imageCreateInfo.extent.width  = a_screenExtent.width;
    imageCreateInfo.extent.height = a_screenExtent.height;
    imageCreateInfo.extent.depth  = 1;
    imageCreateInfo.mipLevels     = 1;
    imageCreateInfo.arrayLayers   = 1;
    imageCreateInfo.samples       = VK_SAMPLE_COUNT_1_BIT;
    imageCreateInfo.tiling        = VK_IMAGE_TILING_OPTIMAL;
    imageCreateInfo.usage         = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
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

    VkImageViewCreateInfo viewInfo = {};
    viewInfo.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image                           = (*a_pImage);
    viewInfo.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format                          = VK_FORMAT_D32_SFLOAT;
    viewInfo.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_DEPTH_BIT;
    viewInfo.subresourceRange.baseMipLevel   = 0;
    viewInfo.subresourceRange.levelCount     = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount     = 1;

    VK_CHECK_RESULT(vkCreateImageView(a_device, &viewInfo, nullptr, a_pImageView));
}


void Render::drawFrame() {

    vkWaitForFences(device, 1, &m_sync.inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);
    vkResetFences  (device, 1, &m_sync.inFlightFences[currentFrame]);

    uint32_t imageIndex;
    vkAcquireNextImageKHR(device, screen.swapChain, UINT64_MAX, m_sync.imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

    VkSemaphore      waitSemaphores[] = { m_sync.imageAvailableSemaphores[currentFrame] };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

    VkSubmitInfo submitInfo = {};
    submitInfo.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores    = waitSemaphores;
    submitInfo.pWaitDstStageMask  = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers    = &commandBuffers[imageIndex];

    VkSemaphore signalSemaphores[]  = { m_sync.renderFinishedSemaphores[currentFrame] };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores    = signalSemaphores;

    if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, m_sync.inFlightFences[currentFrame]) != VK_SUCCESS)
        throw std::runtime_error("[DrawFrame]: failed to submit draw command buffer!");

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores    = signalSemaphores;

    VkSwapchainKHR swapChains[] = { screen.swapChain };
    presentInfo.swapchainCount  = 1;
    presentInfo.pSwapchains     = swapChains;
    presentInfo.pImageIndices   = &imageIndex;

    vkQueuePresentKHR(presentQueue, &presentInfo);
    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}
