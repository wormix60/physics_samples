#ifndef TEXTURE_H
#define TEXTURE_H

#include <cstring>

#include <vulkan/vulkan.h> 

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <algorithm>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <cassert>

#include "vk_utils.h"

#include "image.h"

class Texture{
public:
    void setDevice(VkDevice _device, VkPhysicalDevice _physDevice, VkQueue _queue, VkCommandPool _commandPool, 
            VkSampler _sampler, VkDescriptorPool _descriptorPool, VkDescriptorSetLayout _descriptorSetLayout);

    void setTexture(const Image &image_);

    void cleanup();

    VkDescriptorSet getDescriptorSet() const;

    static void createImageBuffer(VkDevice a_device, VkPhysicalDevice a_physDevice, const size_t a_bufferSize,
            VkBuffer* a_pBuffer, VkDeviceMemory* a_pBufferMemory);

    static void createImage(VkDevice a_device, VkPhysicalDevice a_physDevice, unsigned width, unsigned height,
            VkImage* a_pImage, VkDeviceMemory* a_pImageMemory);

    static void transitionImageLayout(VkDevice a_device, VkQueue a_queue, VkCommandPool a_commandPool, 
            VkImage a_image, VkImageLayout a_oldLayout, VkImageLayout a_newLayout);

    static void copyBufferToImage(VkDevice a_device, VkQueue a_queue, VkCommandPool a_commandPool, 
            VkBuffer buffer, VkImage image, unsigned width, unsigned height);

    static VkCommandBuffer beginCommands(VkDevice a_device, VkCommandPool a_commandPool);

    static void endCommands(VkDevice a_device, VkQueue a_queue, VkCommandPool a_commandPool, VkCommandBuffer a_commandBuffer);

    static void createImageView(VkDevice a_device, VkImage a_image, VkImageView * a_pImageView);

    static void createDescriptorSet(VkDevice a_device, VkSampler a_sampler, VkImageView a_imageView, 
        const VkDescriptorSetLayout* a_pDSLayout, VkDescriptorPool a_DSPool, VkDescriptorSet* a_pDS);

private:
    VkDevice device;
    VkPhysicalDevice physDevice;
    VkQueue queue;

    VkCommandPool commandPool;
    VkSampler sampler;

    VkDescriptorPool descriptorPool;
    VkDescriptorSetLayout descriptorSetLayout;
    

    /// Inside

    VkImage image;
    VkDeviceMemory imageMemory;

    VkImageView imageView;

    VkDescriptorSet descriptorSet;
};

#endif