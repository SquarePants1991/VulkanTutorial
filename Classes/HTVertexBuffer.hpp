//
// Created by ocean on 2018/11/19.
// Copyright (c) 2018 handytool. All rights reserved.
//

#ifndef VULKANTUTORIALMACOS_HTVERTEXBUFFER_HPP
#define VULKANTUTORIALMACOS_HTVERTEXBUFFER_HPP

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <vector>
#include "HTRenderDevice.hpp"

struct HTVertex {
    glm::vec3 position;
    glm::vec3 color;
    
    static std::vector<VkVertexInputBindingDescription> getBindingDescriptions() {
        std::vector<VkVertexInputBindingDescription> bindingDescriptions;
        VkVertexInputBindingDescription bindingDescription  = {
                .binding = 0,
                .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
                .stride = sizeof(HTVertex)
        };
        bindingDescriptions.push_back(bindingDescription);
        return bindingDescriptions;
    }

    static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions() {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
        VkVertexInputAttributeDescription positionAttr = {
                .binding = 0,
                .offset = static_cast<uint32_t>(offsetof(HTVertex, position)),
                .location = 0,
                .format = VK_FORMAT_R32G32B32_SFLOAT
        };
        VkVertexInputAttributeDescription colorAttr = {
                .binding = 0,
                .offset = static_cast<uint32_t>(offsetof(HTVertex, color)),
                .location = 1,
                .format = VK_FORMAT_R32G32B32_SFLOAT
        };
        attributeDescriptions.push_back(positionAttr);
        attributeDescriptions.push_back(colorAttr);
        return attributeDescriptions;
    }
};

class HTVertexBuffer {
private:
    HTRenderDevicePtr _renderDevicePtr;
    HTVertex *_vertices;
    uint32_t _vertexCount;
    uint16_t *_indexes;
    uint32_t _indexCount;
    bool _shouldCopyData;
    void createBuffers();
    void createBuffer(uint32_t sizeInBytes, VkBufferUsageFlagBits usageFlagBits, VkBuffer &buffer, VkDeviceMemory &bufferMemory);
    void copyData(uint32_t sizeInBytes, void *data, VkDeviceMemory &dstMemory);
public:
    VkBuffer vkVertexBuffer;
    VkBuffer vkIndexBuffer;
    VkDeviceMemory vkVertexBufferDeviceMemory;
    VkDeviceMemory vkIndexBufferDeviceMemory;
    HTVertexBuffer(HTRenderDevicePtr renderDevicePtr, HTVertex *vertices, uint32_t vertexCount, uint16_t *indexes = nullptr, uint32_t indexCount = 0, bool shouldCopyData = true);
    ~HTVertexBuffer();

    uint32_t vertexCount() {
        return _vertexCount;
    }

    uint32_t indexCount() {
        return _indexCount;
    }

    bool supportIndexMode() {
        return _indexCount > 0;
    }
};

HTMakeClass(HTVertexBuffer)
#endif //VULKANTUTORIALMACOS_HTVERTEXBUFFER_HPP
