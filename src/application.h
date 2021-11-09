#ifndef APPLICATION_H
#define APPLICATION_H

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#ifdef WIN32
#pragma comment(lib,"glfw3.lib")
#endif

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


const std::vector<const char*> deviceExtensions = {
  VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif


extern bool press_keys[1024];

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);


const int WIDTH  = 1600;
const int HEIGHT = 900;
const float M_PIF = acosf(-1.0f);

const long long TICK = 25000;

const int MAX_FRAMES_IN_FLIGHT = 2;


#include "object.h"
#include "image.h"
#include "light.h"


class Application{
    struct SyncObj{
        std::vector<VkSemaphore> imageAvailableSemaphores;
        std::vector<VkSemaphore> renderFinishedSemaphores;
        std::vector<VkFence>     inFlightFences;
    } m_sync;
    

public:
    void run();
    void initWindow();
    void initVulkan();
    void createResources();
    void setScene();
    void setPlayer();
    void mainLoop();
    void cleanup();
    void updateScene();
    void drawFrame();


    static void createShadowMapDescriptorPool(VkDevice a_device, VkDescriptorPool* a_pDSPool);

    static void createShadowMapDescriptorSetLayout(VkDevice a_device, VkDescriptorSetLayout* a_pDSLayout);

    static void createShadowMapDescriptorSet(VkDevice a_device, VkSampler a_sampler, VkImageView a_imageView, 
            const VkDescriptorSetLayout* a_pDSLayout, VkDescriptorPool a_DSPool, VkDescriptorSet* a_pDS);

    static void createPosMatrixDescriptorPool(VkDevice a_device, VkDescriptorPool* a_pDSPool);

    static void createPosMatrixDescriptorSetLayout(VkDevice a_device, VkDescriptorSetLayout* a_pDSLayout);

    static void createShadowBuffer(VkDevice a_device, VkPhysicalDevice a_physDevice, 
        const size_t a_bufferSize, VkBuffer* a_pBuffer, VkDeviceMemory* a_pBufferMemory);

    static void createShadowDescriptorPool(VkDevice a_device, VkDescriptorPool* a_pDSPool);

    static void createShadowDescriptorSetLayout(VkDevice a_device, VkDescriptorSetLayout* a_pDSLayout);

    static void createShadowDescriptorSet(VkDevice a_device, VkBuffer a_buffer, size_t a_bufferSize, 
        const VkDescriptorSetLayout* a_pDSLayout, VkDescriptorPool a_DSPool, VkDescriptorSet* a_pDS);

    static void createVertShaderBuffer(VkDevice a_device, VkPhysicalDevice a_physDevice, 
            const size_t a_bufferPosSize, VkBuffer* a_pBufferPos, 
            const size_t a_bufferLightSize, VkBuffer *a_pBufferLight, 
            VkDeviceMemory* a_pBufferMemory, unsigned *off);

    static void createVertShaderDescriptorPool(VkDevice a_device, VkDescriptorPool* a_pDSPool);

    static void createVertShaderDescriptorSetLayout(VkDevice a_device, VkDescriptorSetLayout* a_pDSLayout);

    static void createVertShaderDescriptorSet(VkDevice a_device, 
            VkBuffer a_bufferPos, size_t a_bufferPosSize, 
            VkBuffer a_bufferLight, size_t a_bufferLightSize,
            const VkDescriptorSetLayout* a_pDSLayout, VkDescriptorPool a_DSPool, VkDescriptorSet* a_pDS);

    static void createFragShaderBuffer(VkDevice a_device, VkPhysicalDevice a_physDevice, const size_t a_bufferSize,
            VkBuffer* a_pBuffer, VkDeviceMemory* a_pBufferMemory);

    static void createFragShaderDescriptorPool(VkDevice a_device, VkDescriptorPool* a_pDSPool);

    static void createFragShaderDescriptorSetLayout(VkDevice a_device, VkDescriptorSetLayout* a_pDSLayout);

    static void createFragShaderDescriptorSet(VkDevice a_device, VkBuffer a_buffer, size_t a_bufferSize, 
            const VkDescriptorSetLayout* a_pDSLayout, VkDescriptorPool a_DSPool, VkDescriptorSet* a_pDS) ;

    static void createTexShaderSampler(VkDevice a_device, VkPhysicalDevice a_physDevice,
            VkSampler* a_pSampler);

    static void createTexShaderDescriptorPool(VkDevice a_device, VkDescriptorPool* a_pDSPool);

    static void createTexShaderDescriptorSetLayout(VkDevice a_device, VkDescriptorSetLayout* a_pDSLayout);

    static void createDebugRenderPass(VkDevice a_device, VkFormat a_swapChainImageFormat, VkFormat a_depthImageFormat,
            VkRenderPass* a_pRenderPass);

    static void createRenderPass(VkDevice a_device, VkFormat a_swapChainImageFormat, VkFormat a_depthImageFormat,
            VkRenderPass* a_pRenderPass);

    static void createShadowMapRenderPass(VkDevice a_device, VkFormat a_depthImageFormat,
            VkRenderPass* a_pRenderPass);

    static void createDebugPipeline(VkDevice a_device, VkExtent2D a_screenExtent, VkRenderPass a_renderPass,
            VkPipelineLayout* a_pLayout, VkPipeline* a_pPipiline,
            const VkDescriptorSetLayout &a_smdsLayout);

    static void createGraphicsPipeline(VkDevice a_device, VkExtent2D a_screenExtent, VkRenderPass a_renderPass,
            VkPipelineLayout* a_pLayout, VkPipeline* a_pPipiline,
            const VkDescriptorSetLayout &a_pdsLayout, 
            const VkDescriptorSetLayout &a_vdsLayout, 
            const VkDescriptorSetLayout &a_tdsLayout, 
            const VkDescriptorSetLayout &a_smdsLayout,
            const VkDescriptorSetLayout &a_fdsLayout);

    static void createShadowMapPipeline(VkDevice a_device, VkRenderPass a_renderPass,
            VkPipelineLayout* a_pLayout, VkPipeline* a_pPipiline,
            const VkDescriptorSetLayout &a_pdsLayout, 
            const VkDescriptorSetLayout &a_vdsLayout);

    static void createAndWriteCommandBuffers(VkDevice a_device, VkCommandPool a_cmdPool, 
        std::vector<VkFramebuffer> a_swapChainFramebuffers, VkExtent2D a_frameBufferExtent,
        VkRenderPass a_renderPass, VkPipeline a_graphicsPipeline, VkPipelineLayout a_layout, 
        std::vector<VkCommandBuffer>* a_cmdBuffers, std::vector<Object> a_obj, unsigned a_cam,
        const VkDescriptorSet &a_vds, 
        const VkDescriptorSet &a_fds,
        const VkDescriptorSet &a_sds,
        Light a_light,        
        VkRenderPass a_shadowMapRenderPass, VkPipeline a_shadowMapPipeline, VkPipelineLayout a_shadowMapLayout, 
        VkRenderPass a_debugRenderPass, VkPipeline a_debugPipeline, VkPipelineLayout a_debugLayout);

    static void createSyncObjects(VkDevice a_device, SyncObj* a_pSyncObjs);


    static void createDepthResources(VkDevice a_device, VkPhysicalDevice a_physDevice, VkExtent2D a_screenExtent,
            VkImage* a_pImage, VkDeviceMemory* a_pImageMemory, VkImageView * a_pImageView);

    static void createShadowMapResources(VkDevice a_device, VkPhysicalDevice a_physDevice, 
            VkFormat a_swapChainImageFormat, VkRenderPass a_renderPass, 
            VkImage* a_pImage, VkImage* a_pShadowImage, VkDeviceMemory* a_pImageMemory,
            VkImageView * a_pImageView, VkImageView * a_pShadowImageView, VkSampler *a_pSampler, 
            VkFramebuffer *a_pFramebuffer);


    static VKAPI_ATTR VkBool32 VKAPI_CALL debugReportCallbackFn(
            VkDebugReportFlagsEXT                       flags,
            VkDebugReportObjectTypeEXT                  objectType,
            uint64_t                                    object,
            size_t                                      location,
            int32_t                                     messageCode,
            const char*                                 pLayerPrefix,
            const char*                                 pMessage,
            void*                                       pUserData) {

        printf("[Debug Report]: %s: %s\n", pLayerPrefix, pMessage);
        return VK_FALSE;
    }
    VkDebugReportCallbackEXT debugReportCallback;

private:
    GLFWwindow * window;

    VkInstance instance;
    std::vector<const char*> enabledLayers;

    VkDebugUtilsMessengerEXT debugMessenger;
    VkSurfaceKHR surface;

    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device;

    VkQueue graphicsQueue;
    VkQueue presentQueue;

    vk_utils::ScreenBufferResources screen;

    VkImage        depthImage;
    VkDeviceMemory depthImageMemory;
    VkImageView    depthImageView;

    VkRenderPass     renderPass;
    VkPipelineLayout pipelineLayout;
    VkPipeline       graphicsPipeline;

    VkRenderPass     shadowMapRenderPass;
    VkPipelineLayout shadowMapPipelineLayout;
    VkPipeline       shadowMapPipeline;

    VkRenderPass     debugRenderPass;
    VkPipelineLayout debugPipelineLayout;
    VkPipeline       debugPipeline;

    VkCommandPool                commandPool;
    std::vector<VkCommandBuffer> commandBuffers;
    std::vector<VkCommandBuffer> shadowMapCommandBuffers;

    VkBuffer              vertBufferPos;
    VkBuffer              vertBufferLight;
    VkDeviceMemory        vertBufferMemory;
    VkDescriptorPool      vertPool;
    VkDescriptorSetLayout vertSetLayout;
    VkDescriptorSet       vertSet;

    VkBuffer              fragBuffer;
    VkDeviceMemory        fragBufferMemory;
    VkDescriptorPool      fragPool;
    VkDescriptorSetLayout fragSetLayout;
    VkDescriptorSet       fragSet;

    VkDescriptorPool      posPool;
    VkDescriptorSetLayout posSetLayout;

    VkDescriptorPool      texPool;
    VkDescriptorSetLayout texSetLayout;
    VkSampler             texSampler;

    VkDescriptorPool      shadowMapPool;
    VkDescriptorSetLayout shadowMapSetLayout;

    VkBuffer              shadowBuffer;
    VkDeviceMemory        shadowBufferMemory;
    VkDescriptorPool      shadowPool;
    VkDescriptorSetLayout shadowSetLayout;
    VkDescriptorSet       shadowSet;
    

    size_t currentFrame = 0;

    long long time = 0;
    std::chrono::high_resolution_clock::time_point startCycle;
    std::chrono::high_resolution_clock::time_point endCycle;
    
    unsigned vertOffset;

    static unsigned cam;

    float playerPosX;
    float playerPosY;
    float playerPosZ;

    float playerAddF;
    float playerAddS;
    float playerAddB;

    float playerAngleX;
    float playerAngleY;
    
    float playerAddX;
    float playerAddY;

    float lowerBoundX;
    float upperBoundX;
    float lowerBoundY;
    float upperBoundY;
    float lowerBoundZ;
    float upperBoundZ;

    double prevMousePosX;
    double prevMousePosY;

    std::vector<Object> objects;

    Light light;

    float objPos = 0.0f;

    unsigned fps = 0;
    unsigned sec = 0;
}; 


#endif