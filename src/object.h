#ifndef OBJECT_H
#define OBJECT_H
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <tiny_obj_loader.h>

#include "texture.h"

struct Color{
    float r;
    float g;
    float b;
    float a;
};


struct PolygonPoint{
    glm::vec3 ver;
    glm::vec2 tex;
    glm::vec3 norm;
};


struct Polygon{
    PolygonPoint ver1;
    PolygonPoint ver2;
    PolygonPoint ver3;
};


struct Angle{
    float x;
    float y;
    float z;
};


struct ObjectNecessary{
    VkDevice device;
    VkPhysicalDevice physDevice;
    VkQueue queue;
    VkCommandPool commandPool; 
    VkSampler sampler;
    VkDescriptorPool descriptorPool; 
    VkDescriptorSetLayout descriptorSetLayout; 
    VkDescriptorPool posDescriptorPool; 
    VkDescriptorSetLayout posDescriptorSetLayout;
};


class Object{
public: 
    void setRender(ObjectNecessary objectNecessary);

    void setTexture(const Image &_image);

    void genCube(glm::vec3 beg, float sizeX, float sizeY, float sizeZ,
            float angleX, float angleY, float angleZ);
    void genCube2(glm::vec3 beg, float sizeX, float sizeY, float sizeZ,
            float angleX, float angleY, float angleZ);
    void genField(glm::vec3 beg, float sizeX, float sizeZ,
            float angleX, float angleY, float angleZ);

    void loadModel(glm::vec3 beg, const char *modelPath, float scale,
            float angleX, float angleY, float angleZ);

    void setPos(glm::vec3 beg, float angleX, float angleY, float angleZ);
    void setNormals();
    void gen();
    void updatePos();
    void update();
    void cleanup();

    VkBuffer getBuffer() const;
    VkDescriptorSet getPolygonDescriptorSet() const;
    VkDescriptorSet getPosDescriptorSet() const;
    size_t getPolygonsSize() const;

    static void createBuffers(VkDevice a_device, VkPhysicalDevice a_physDevice, const size_t a_polygonBufferSize,
            VkBuffer *a_pPolygonBuffer, const size_t a_posBufferSize, VkBuffer *a_pPosBuffer, 
            VkDeviceMemory *a_pBufferMemory);

    static VkCommandBuffer beginCommands(VkDevice a_device, VkCommandPool a_commandPool);

    static void endCommands(VkDevice a_device, VkQueue a_queue, VkCommandPool a_commandPool, VkCommandBuffer a_commandBuffer);

    static void createPosDescriptorSet(VkDevice a_device, VkBuffer a_buffer, size_t a_bufferSize, 
            const VkDescriptorSetLayout* a_pDSLayout, VkDescriptorPool a_DSPool, VkDescriptorSet* a_pDS);


private:
    VkDevice device;
    VkPhysicalDevice physDevice;
    VkQueue queue;

    VkCommandPool commandPool;


    VkDescriptorPool posDescriptorPool;
    VkDescriptorSetLayout posDescriptorSetLayout;
    //// Inside

    std::vector<Polygon> polygons = {};

    Texture tex;


    VkDeviceMemory  bufferMemory;

    VkBuffer        polygonBuffer;
    VkBuffer        posBuffer;

    VkDescriptorSet posSet;


    glm::mat4 pos;
    glm::mat4 rot;
};

#endif