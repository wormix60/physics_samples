#include "object.h"


Point3 operator-(Point3 a, Point3 b) {
    Point3 ret = {a.x - b.x, a.y - b.y, a.z - b.z};

    return ret;
}


Point3 crossProd(Point3 a, Point3 b) {
    Point3 cross = {-a.y * b.z + a.z * b.y, a.x * b.z - a.z * b.x, -a.x * b.y + a.y * b.x};

    return cross;
}



void Object::genCube(Point3 begVertex, float sizeX, float sizeY, float sizeZ,
        float angleX, float angleY, float angleZ){

    glm::vec3 beg = {begVertex.x, begVertex.y, begVertex.z};

    rot = glm::rotate(glm::mat4(1.0f), angleX, glm::vec3(1.0f, 0.0f, 0.0f));
    rot = glm::rotate(rot            , angleY, glm::vec3(0.0f, 1.0f, 0.0f));
    rot = glm::rotate(rot            , angleZ, glm::vec3(0.0f, 0.0f, 1.0f));


    pos = glm::translate(glm::mat4(1.0), beg);


    polygons = {
        {
            {{0.0f , 0.0f , 0.0f }, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}},
            {{0.0f , sizeY, 0.0f }, {0.0f, 1.0f}, {1.0f, 1.0f, 1.0f}},
            {{sizeX, 0.0f , 0.0f }, {1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}},
        },
        {
            {{sizeX, 0.0f , 0.0f }, {1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}},
            {{0.0f , sizeY, 0.0f }, {0.0f, 1.0f}, {1.0f, 1.0f, 1.0f}},
            {{sizeX, sizeY, 0.0f }, {1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}},
        },
        {
            {{sizeX, 0.0f , 0.0f }, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}},
            {{sizeX, sizeY, 0.0f }, {0.0f, 1.0f}, {1.0f, 1.0f, 1.0f}},
            {{sizeX, 0.0f , sizeZ}, {1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}},
        },
        {
            {{sizeX, 0.0f , sizeZ}, {1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}},
            {{sizeX, sizeY, 0.0f }, {0.0f, 1.0f}, {1.0f, 1.0f, 1.0f}},
            {{sizeX, sizeY, sizeZ}, {1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}},
        },
        {
            {{sizeX, 0.0f , sizeZ}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}},
            {{sizeX, sizeY, sizeZ}, {0.0f, 1.0f}, {1.0f, 1.0f, 1.0f}},
            {{0.0f , 0.0f , sizeZ}, {1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}},
        },
        {
            {{0.0f , 0.0f , sizeZ}, {1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}},
            {{sizeX, sizeY, sizeZ}, {0.0f, 1.0f}, {1.0f, 1.0f, 1.0f}},
            {{0.0f , sizeY, sizeZ}, {1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}},
        },
        {
            {{0.0f , 0.0f , sizeZ}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}},
            {{0.0f , sizeY, sizeZ}, {0.0f, 1.0f}, {1.0f, 1.0f, 1.0f}},
            {{0.0f , 0.0f , 0.0f }, {1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}},
        },
        {
            {{0.0f , 0.0f , 0.0f }, {1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}},
            {{0.0f , sizeY, sizeZ}, {0.0f, 1.0f}, {1.0f, 1.0f, 1.0f}},
            {{0.0f , sizeY, 0.0f }, {1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}},
        },
        {
            {{0.0f , sizeY, 0.0f }, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}},
            {{0.0f , sizeY, sizeZ}, {0.0f, 1.0f}, {1.0f, 1.0f, 1.0f}},
            {{sizeX, sizeY, 0.0f }, {1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}},
        },
        {
            {{sizeX, sizeY, 0.0f }, {1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}},
            {{0.0f , sizeY, sizeZ}, {0.0f, 1.0f}, {1.0f, 1.0f, 1.0f}},
            {{sizeX, sizeY, sizeZ}, {1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}},
        },
        {
            {{0.0f , 0.0f , 0.0f }, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}},
            {{sizeX, 0.0f , 0.0f }, {0.0f, 1.0f}, {1.0f, 1.0f, 1.0f}},
            {{0.0f , 0.0f , sizeZ}, {1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}},
        },
        {
            {{0.0f , 0.0f , sizeZ}, {1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}},
            {{sizeX, 0.0f , 0.0f }, {0.0f, 1.0f}, {1.0f, 1.0f, 1.0f}},
            {{sizeX, 0.0f , sizeZ}, {1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}},
        },
    };
}




void Object::genCube2(Point3 begVertex, float sizeX, float sizeY, float sizeZ,
        float angleX, float angleY, float angleZ){

    glm::vec3 beg = {begVertex.x, begVertex.y, begVertex.z};

    rot = glm::rotate(glm::mat4(1.0f), angleX, glm::vec3(1.0f, 0.0f, 0.0f));
    rot = glm::rotate(rot            , angleY, glm::vec3(0.0f, 1.0f, 0.0f));
    rot = glm::rotate(rot            , angleZ, glm::vec3(0.0f, 0.0f, 1.0f));


    pos = glm::translate(glm::mat4(1.0), beg);


    float texX  = 0.5f * sizeX / (sizeX + sizeZ);
    float texZX = 0.5f * sizeZ / (sizeX + sizeZ);
    float texY  =        sizeY / (sizeY + sizeZ);
    float texZY =        sizeZ / (sizeY + sizeZ);


    polygons = {
        {
            {{0.0f , 0.0f , 0.0f }, {0.0f + texZX, 0.0f}, {1.0f, 1.0f, 1.0f}},
            {{0.0f , sizeY, 0.0f }, {0.0f + texZX, texY}, {1.0f, 1.0f, 1.0f}},
            {{sizeX, 0.0f , 0.0f }, {0.5f        , 0.0f}, {1.0f, 1.0f, 1.0f}},
        },
        {
            {{sizeX, 0.0f , 0.0f }, {0.5f        , 0.0f}, {1.0f, 1.0f, 1.0f}},
            {{0.0f , sizeY, 0.0f }, {0.0f + texZX, texY}, {1.0f, 1.0f, 1.0f}},
            {{sizeX, sizeY, 0.0f }, {0.5f        , texY}, {1.0f, 1.0f, 1.0f}},
        },
        {
            {{sizeX, 0.0f , 0.0f }, {0.5f        , 0.0f}, {1.0f, 1.0f, 1.0f}},
            {{sizeX, sizeY, 0.0f }, {0.5f        , texY}, {1.0f, 1.0f, 1.0f}},
            {{sizeX, 0.0f , sizeZ}, {1.0f - texX , 0.0f}, {1.0f, 1.0f, 1.0f}},
        },
        {
            {{sizeX, 0.0f , sizeZ}, {1.0f - texX , 0.0f}, {1.0f, 1.0f, 1.0f}},
            {{sizeX, sizeY, 0.0f }, {0.5f        , texY}, {1.0f, 1.0f, 1.0f}},
            {{sizeX, sizeY, sizeZ}, {1.0f - texX , texY}, {1.0f, 1.0f, 1.0f}},
        },
        {
            {{sizeX, 0.0f , sizeZ}, {1.0f - texX , 0.0f}, {1.0f, 1.0f, 1.0f}},
            {{sizeX, sizeY, sizeZ}, {1.0f - texX , texY}, {1.0f, 1.0f, 1.0f}},
            {{0.0f , 0.0f , sizeZ}, {1.0f        , 0.0f}, {1.0f, 1.0f, 1.0f}},
        },
        {
            {{0.0f , 0.0f , sizeZ}, {1.0f        , 0.0f}, {1.0f, 1.0f, 1.0f}},
            {{sizeX, sizeY, sizeZ}, {1.0f - texX , texY}, {1.0f, 1.0f, 1.0f}},
            {{0.0f , sizeY, sizeZ}, {1.0f        , texY}, {1.0f, 1.0f, 1.0f}},
        },
        {
            {{0.0f , 0.0f , sizeZ}, {0.0f        , 0.0f}, {1.0f, 1.0f, 1.0f}},
            {{0.0f , sizeY, sizeZ}, {0.0f        , texY}, {1.0f, 1.0f, 1.0f}},
            {{0.0f , 0.0f , 0.0f }, {0.0f + texZX, 0.0f}, {1.0f, 1.0f, 1.0f}},
        },
        {
            {{0.0f , 0.0f , 0.0f }, {0.0f + texZX, 0.0f}, {1.0f, 1.0f, 1.0f}},
            {{0.0f , sizeY, sizeZ}, {0.0f        , texY}, {1.0f, 1.0f, 1.0f}},
            {{0.0f , sizeY, 0.0f }, {0.0f + texZX, texY}, {1.0f, 1.0f, 1.0f}},
        },
        {
            {{0.0f , sizeY, 0.0f }, {0.0f + texZX, texY}, {1.0f, 1.0f, 1.0f}},
            {{0.0f , sizeY, sizeZ}, {0.0f + texZX, 1.0f}, {1.0f, 1.0f, 1.0f}},
            {{sizeX, sizeY, 0.0f }, {0.5f        , texY}, {1.0f, 1.0f, 1.0f}},
        },
        {
            {{sizeX, sizeY, 0.0f }, {0.5f        , texY}, {1.0f, 1.0f, 1.0f}},
            {{0.0f , sizeY, sizeZ}, {0.0f + texZX, 1.0f}, {1.0f, 1.0f, 1.0f}},
            {{sizeX, sizeY, sizeZ}, {0.5f        , 1.0f}, {1.0f, 1.0f, 1.0f}},
        },
        {
            {{0.0f , 0.0f , 0.0f }, {0.5f        , texY}, {1.0f, 1.0f, 1.0f}},
            {{sizeX, 0.0f , 0.0f }, {1.0f - texX , texY}, {1.0f, 1.0f, 1.0f}},
            {{0.0f , 0.0f , sizeZ}, {0.5f        , 1.0f}, {1.0f, 1.0f, 1.0f}},
        },
        {
            {{0.0f , 0.0f , sizeZ}, {0.5f        , 1.0f}, {1.0f, 1.0f, 1.0f}},
            {{sizeX, 0.0f , 0.0f }, {1.0f - texX , texY}, {1.0f, 1.0f, 1.0f}},
            {{sizeX, 0.0f , sizeZ}, {1.0f - texX , 1.0f}, {1.0f, 1.0f, 1.0f}},
        },
    };
}


void Object::genField(Point3 begVertex, float sizeX, float sizeZ,
        float angleX, float angleY, float angleZ){

    glm::vec3 beg = {begVertex.x, begVertex.y, begVertex.z};

    rot = glm::rotate(glm::mat4(1.0f), angleX, glm::vec3(1.0f, 0.0f, 0.0f));
    rot = glm::rotate(rot            , angleY, glm::vec3(0.0f, 1.0f, 0.0f));
    rot = glm::rotate(rot            , angleZ, glm::vec3(0.0f, 0.0f, 1.0f));


    pos = glm::translate(glm::mat4(1.0), beg);

    polygons = {
        {
            {{0.0f , 0.0f , 0.0f }, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}},
            {{0.0f , 0.0f , sizeZ}, {0.0f, 1.0f}, {1.0f, 1.0f, 1.0f}},
            {{sizeX, 0.0f , 0.0f }, {1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}},
        },
        {
            {{sizeX, 0.0f , 0.0f }, {1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}},
            {{0.0f , 0.0f , sizeZ}, {0.0f, 1.0f}, {1.0f, 1.0f, 1.0f}},
            {{sizeX, 0.0f , sizeZ}, {1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}},
        },
    };
}


void Object::loadModel(Point3 begVertex, const char *modelPath, float scale,
        float angleX, float angleY, float angleZ) {

    glm::vec3 beg = {begVertex.x, begVertex.y, begVertex.z};

    rot = glm::rotate(glm::mat4(1.0f), angleX, glm::vec3(1.0f, 0.0f, 0.0f));
    rot = glm::rotate(rot            , angleY, glm::vec3(0.0f, 1.0f, 0.0f));
    rot = glm::rotate(rot            , angleZ, glm::vec3(0.0f, 0.0f, 1.0f));


    pos = glm::translate(glm::mat4(1.0), beg);

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, modelPath)) {
        throw std::runtime_error(warn + err);
    }
    polygons = {};
    for (const auto& shape : shapes) {
        Polygon polygon = {};
        int idx = 0;
        for (const auto& index : shape.mesh.indices) {
            if (idx == 0) {
                polygon.ver1 = {
                        {scale * (attrib.vertices[3 * index.vertex_index + 1]),
                         scale * (attrib.vertices[3 * index.vertex_index + 2]),
                         scale * (attrib.vertices[3 * index.vertex_index + 0]),}, 
                        {attrib.texcoords[2 * index.texcoord_index + 0],
                         attrib.texcoords[2 * index.texcoord_index + 1]},
                        {attrib.normals[3 * index.normal_index + 1],
                         attrib.normals[3 * index.normal_index + 2],
                         attrib.normals[3 * index.normal_index + 0]}
                };
            } else if (idx == 1) {
                polygon.ver2 = {
                        {scale * (attrib.vertices[3 * index.vertex_index + 1]),
                         scale * (attrib.vertices[3 * index.vertex_index + 2]),
                         scale * (attrib.vertices[3 * index.vertex_index + 0]),}, 
                        {attrib.texcoords[2 * index.texcoord_index + 0],
                         attrib.texcoords[2 * index.texcoord_index + 1]},
                        {attrib.normals[3 * index.normal_index + 1],
                         attrib.normals[3 * index.normal_index + 2],
                         attrib.normals[3 * index.normal_index + 0]}
                };
            } else if (idx == 2) {
                polygon.ver3 = {
                        {scale * (attrib.vertices[3 * index.vertex_index + 1]),
                         scale * (attrib.vertices[3 * index.vertex_index + 2]),
                         scale * (attrib.vertices[3 * index.vertex_index + 0]),}, 
                        {attrib.texcoords[2 * index.texcoord_index + 0],
                         attrib.texcoords[2 * index.texcoord_index + 1]},
                        {attrib.normals[3 * index.normal_index + 1],
                         attrib.normals[3 * index.normal_index + 2],
                         attrib.normals[3 * index.normal_index + 0]}
                };
                polygons.push_back(polygon);
            }
            idx = (idx + 1) % 3;
        }
    }
}


void Object::setDevice(VkDevice _device, VkPhysicalDevice _physDevice, VkQueue _queue, VkCommandPool _commandPool, 
        VkSampler _sampler, VkDescriptorPool _descriptorPool, VkDescriptorSetLayout _descriptorSetLayout, 
        VkDescriptorPool _posDescriptorPool, VkDescriptorSetLayout _posDescriptorSetLayout) {

    tex.setDevice(_device, _physDevice, _queue, _commandPool, _sampler, _descriptorPool, _descriptorSetLayout);

    device = _device;
    physDevice = _physDevice;
    queue = _queue;

    commandPool = _commandPool;

    posDescriptorPool = _posDescriptorPool;
    posDescriptorSetLayout = _posDescriptorSetLayout;
}


void Object::setTexture(const Image &_image) {
    tex.setTexture(_image);
}


void Object::setPos(Point3 begVertex, float angleX, float angleY, float angleZ) {
    glm::vec3 beg = {begVertex.x, begVertex.y, begVertex.z};

    rot = glm::rotate(glm::mat4(1.0f), angleX, glm::vec3(1.0f, 0.0f, 0.0f));
    rot = glm::rotate(rot            , angleY, glm::vec3(0.0f, 1.0f, 0.0f));
    rot = glm::rotate(rot            , angleZ, glm::vec3(0.0f, 0.0f, 1.0f));


    pos = glm::translate(glm::mat4(1.0), beg);

}


void Object::setNormals() {
    for (size_t i = 0; i < polygons.size(); i++) {
        Point3 vec2 = polygons[i].ver2.ver - polygons[i].ver1.ver;
        Point3 vec1 = polygons[i].ver3.ver - polygons[i].ver1.ver;

        Point3 n = crossProd(vec1, vec2);

        polygons[i].ver1.norm = n;
        polygons[i].ver2.norm = n;
        polygons[i].ver3.norm = n;
    }
}


void Object::gen() {
    createBuffers(device, physDevice, polygons.size() * sizeof(Polygon), &polygonBuffer, 
            2 * sizeof(glm::mat4), &posBuffer, &bufferMemory);

    update();

    createPosDescriptorSet(device, posBuffer, sizeof(glm::mat4), &posDescriptorSetLayout,
            posDescriptorPool, &posSet);
}


void Object::updatePos() {
    VkCommandBuffer commandBuffer = beginCommands(device, commandPool);
    vkCmdUpdateBuffer(commandBuffer, posBuffer, 0, sizeof(glm::mat4), &pos);
    vkCmdUpdateBuffer(commandBuffer, posBuffer, sizeof(glm::mat4), sizeof(glm::mat4), &rot);
    endCommands(device, queue, commandPool, commandBuffer);
}


void Object::update() {

    VkBuffer tmpBuffer;
    VkDeviceMemory tmpBufferMemory;

    VkBufferCreateInfo bufferCreateInfo = {};
    bufferCreateInfo.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.pNext       = nullptr;
    bufferCreateInfo.size        = polygons.size() * sizeof(Polygon);                       
    bufferCreateInfo.usage       = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;            

    VK_CHECK_RESULT(vkCreateBuffer(device, &bufferCreateInfo, NULL, &tmpBuffer)); 

                
    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(device, tmpBuffer, &memoryRequirements);

    VkMemoryAllocateInfo allocateInfo = {};
    allocateInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocateInfo.pNext           = nullptr;
    allocateInfo.allocationSize  = memoryRequirements.size; 
    allocateInfo.memoryTypeIndex = vk_utils::FindMemoryType(memoryRequirements.memoryTypeBits,  
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, physDevice); 

    VK_CHECK_RESULT(vkAllocateMemory(device, &allocateInfo, NULL, &tmpBufferMemory));   

    VK_CHECK_RESULT(vkBindBufferMemory(device, tmpBuffer, tmpBufferMemory, 0));


    void *mappedMemory;
    vkMapMemory(device, tmpBufferMemory, 0, polygons.size() * sizeof(Polygon), 0, &mappedMemory);
    memcpy(mappedMemory, polygons.data(), polygons.size() * sizeof(Polygon));
    vkUnmapMemory(device, tmpBufferMemory);

    VkCommandBuffer commandBuffer = beginCommands(device, commandPool);

    vkCmdUpdateBuffer(commandBuffer, posBuffer, 0, sizeof(glm::mat4), &pos);
    vkCmdUpdateBuffer(commandBuffer, posBuffer, sizeof(glm::mat4), sizeof(glm::mat4), &rot);

    VkBufferCopy region = {};
    region.size = polygons.size() * sizeof(Polygon);

    vkCmdCopyBuffer(commandBuffer, tmpBuffer, polygonBuffer, 1, &region);

    endCommands(device, queue, commandPool, commandBuffer);

    vkDestroyBuffer(device, tmpBuffer, nullptr);
    vkFreeMemory(device, tmpBufferMemory, nullptr);
}


void Object::cleanup() {
    tex.cleanup();

    vkFreeMemory(device, bufferMemory, NULL);
    vkDestroyBuffer(device, polygonBuffer, NULL); 
    vkDestroyBuffer(device, posBuffer, NULL); 
}


VkBuffer Object::getBuffer() const {
    return polygonBuffer;
}


VkDescriptorSet Object::getPolygonDescriptorSet() const {
    return tex.getDescriptorSet();
}


VkDescriptorSet Object::getPosDescriptorSet() const {
    return posSet;
}

size_t Object::getPolygonsSize() const {
    return polygons.size();
}

void Object::createBuffers(VkDevice a_device, VkPhysicalDevice a_physDevice, const size_t a_polygonBufferSize,
        VkBuffer *a_pPolygonBuffer, const size_t a_posBufferSize, VkBuffer *a_pPosBuffer, 
        VkDeviceMemory *a_pBufferMemory) {

    VkBufferCreateInfo bufferCreateInfo = {};
    bufferCreateInfo.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.pNext       = nullptr;
    bufferCreateInfo.size        = a_posBufferSize;                         
    bufferCreateInfo.usage       = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;            

    VK_CHECK_RESULT(vkCreateBuffer(a_device, &bufferCreateInfo, NULL, a_pPosBuffer)); // create bufferStaging.

                
    VkMemoryRequirements memoryPosRequirements;
    vkGetBufferMemoryRequirements(a_device, (*a_pPosBuffer), &memoryPosRequirements);

    bufferCreateInfo.size        = a_polygonBufferSize;                         
    bufferCreateInfo.usage       = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;            

    VK_CHECK_RESULT(vkCreateBuffer(a_device, &bufferCreateInfo, NULL, a_pPolygonBuffer)); // create bufferStaging.

                
    VkMemoryRequirements memoryPolygonRequirements;
    vkGetBufferMemoryRequirements(a_device, (*a_pPolygonBuffer), &memoryPolygonRequirements);

    VkMemoryAllocateInfo allocateInfo = {};
    allocateInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocateInfo.pNext           = nullptr;
    allocateInfo.allocationSize  = memoryPosRequirements.size + memoryPolygonRequirements.size; // specify required memory.
    allocateInfo.memoryTypeIndex = vk_utils::FindMemoryType(memoryPosRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, a_physDevice); // #NOTE VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT

    VK_CHECK_RESULT(vkAllocateMemory(a_device, &allocateInfo, NULL, a_pBufferMemory));   // allocate memory on device.

    VK_CHECK_RESULT(vkBindBufferMemory(a_device, (*a_pPosBuffer), (*a_pBufferMemory), 0));  // Now associate that allocated memory with the bufferStaging. With that, the bufferStaging is backed by actual memory.

    VK_CHECK_RESULT(vkBindBufferMemory(a_device, (*a_pPolygonBuffer), (*a_pBufferMemory), memoryPosRequirements.size));
}


VkCommandBuffer Object::beginCommands(VkDevice a_device, VkCommandPool a_commandPool) {

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

void Object::endCommands(VkDevice a_device, VkQueue a_queue, VkCommandPool a_commandPool, VkCommandBuffer a_commandBuffer) {
    
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



void Object::createPosDescriptorSet(VkDevice a_device, VkBuffer a_buffer, size_t a_bufferSize, 
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
