#ifndef LIGHT_H
#define LIGHT_H

#include <vulkan/vulkan.h>
#define GLM_FORCE_RADIANS

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <algorithm>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <cassert>
#include <cmath>
#include <chrono>

#include "vk_utils.h"

const int SHADOWMAP_WIDTH = 4096;
const int SHADOWMAP_HEIGHT = 4096;

struct LightNecessary{
    VkDevice device;
    VkPhysicalDevice physDevice;
    VkRenderPass shadowMapRenderPass; 
    VkDescriptorPool shadowMapPool;
    VkDescriptorSetLayout shadowMapSetLayout;
};

class Light{
public:
    void gen();
    void cleanup();

    void setRender(LightNecessary lightNecessary);

    VkDescriptorSet getShadowMapSet() const;
    VkFramebuffer getShadowMapFramebuffer() const;


    static void createShadowMapResources(VkDevice a_device, VkPhysicalDevice a_physDevice, 
            VkRenderPass a_renderPass, VkImage* a_pShadowImage, VkDeviceMemory* a_pImageMemory,
            VkImageView * a_pShadowImageView, VkSampler *a_pSampler, 
            VkFramebuffer *a_framebuffer);

    static void createShadowMapDescriptorSet(VkDevice a_device, VkSampler a_sampler, VkImageView a_imageView, 
            const VkDescriptorSetLayout* a_pDSLayout, VkDescriptorPool a_DSPool, VkDescriptorSet* a_pDS);

private:
    VkDevice device;
    VkPhysicalDevice physDevice;

    VkDescriptorPool      shadowMapPool;
    VkDescriptorSetLayout shadowMapSetLayout;

    VkRenderPass shadowMapRenderPass;

    ///inside

    VkImage          shadowMapImage;
    VkDeviceMemory   shadowMapImageMemory;
    VkImageView      shadowMapImageView;
    VkSampler        shadowMapSampler;
    VkDescriptorSet  shadowMapSet;
    VkFramebuffer    shadowMapFramebuffer;
    
}; 


#endif