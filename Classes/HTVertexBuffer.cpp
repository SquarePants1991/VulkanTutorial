//
// Created by ocean on 2018/11/19.
// Copyright (c) 2018 handytool. All rights reserved.
//

#include "HTVertexBuffer.hpp"
#include "HTVKCheckUtil.hpp"

HTVertexBuffer::HTVertexBuffer(HTRenderDevicePtr renderDevicePtr, HTVertex *vertices, uint32_t vertexCount, uint16_t *indexes, uint32_t indexCount, bool shouldCopyData):
        _renderDevicePtr(renderDevicePtr),
        _vertices(vertices),
        _vertexCount(vertexCount),
        _indexes(indexes),
        _indexCount(indexCount),
        _shouldCopyData(shouldCopyData) {
    if (_shouldCopyData) {
        if (vertices != nullptr) {
            _vertices = (HTVertex *)malloc(vertexCount * sizeof(HTVertex));
            memcpy(_vertices, vertices, vertexCount * sizeof(HTVertex));
        }
        if (indexes != nullptr) {
            _indexes = (uint16_t *) malloc(indexCount * sizeof(uint16_t));
            memcpy(_indexes, indexes, indexCount * sizeof(uint16_t));
        }
    }
    createBuffers();
}

HTVertexBuffer::~HTVertexBuffer() {
    if (_shouldCopyData && _vertices) {
        free(_vertices);
    }
    if (vkVertexBuffer) {
        vkDestroyBuffer(_renderDevicePtr->vkLogicDevice, vkVertexBuffer, nullptr);
    }
    if (vkVertexBufferDeviceMemory) {
        vkFreeMemory(_renderDevicePtr->vkLogicDevice, vkVertexBufferDeviceMemory, nullptr);
    }
    if (vkIndexBuffer) {
        vkDestroyBuffer(_renderDevicePtr->vkLogicDevice, vkIndexBuffer, nullptr);
    }
    if (vkIndexBufferDeviceMemory) {
        vkFreeMemory(_renderDevicePtr->vkLogicDevice, vkIndexBufferDeviceMemory, nullptr);
    }
}

void HTVertexBuffer::createBuffers() {
    uint32_t vertexBufferSize = sizeof(HTVertex) * _vertexCount;
    createBuffer(vertexBufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, vkVertexBuffer, vkVertexBufferDeviceMemory);
    copyData(vertexBufferSize, reinterpret_cast<void *>(_vertices), vkVertexBufferDeviceMemory);
    uint32_t indexBufferSize = sizeof(uint16_t) * _indexCount;
    createBuffer(indexBufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, vkIndexBuffer, vkIndexBufferDeviceMemory);
    copyData(indexBufferSize, reinterpret_cast<void *>(_indexes), vkIndexBufferDeviceMemory);
}

void HTVertexBuffer::copyData(uint32_t sizeInBytes, void *data, VkDeviceMemory &dstMemory) {
    void *mappingZone;
    vkMapMemory(_renderDevicePtr->vkLogicDevice, dstMemory, VkDeviceSize(0), VkDeviceSize(sizeInBytes), 0, &mappingZone);
    memcpy(mappingZone, data, sizeInBytes);
    vkUnmapMemory(_renderDevicePtr->vkLogicDevice, dstMemory);
}

void HTVertexBuffer::createBuffer(uint32_t sizeInBytes, VkBufferUsageFlagBits usageFlagBits, VkBuffer &buffer, VkDeviceMemory &bufferMemory) {
    VkBufferCreateInfo bufferCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .size = VkDeviceSize(sizeInBytes),
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .usage = usageFlagBits
    };
    htCheckVKOp(vkCreateBuffer(_renderDevicePtr->vkLogicDevice, &bufferCreateInfo, nullptr, &buffer), "VK Buffer create fail.");

    VkMemoryRequirements memoryRequirements = {};
    vkGetBufferMemoryRequirements(_renderDevicePtr->vkLogicDevice, buffer, &memoryRequirements);

    int32_t memoryTypeIndex = -1;
    VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties = {};
    vkGetPhysicalDeviceMemoryProperties(_renderDevicePtr->vkPhysicsDevice, &physicalDeviceMemoryProperties);
    int32_t shouldSupportBits = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    for (int i = 0; i < physicalDeviceMemoryProperties.memoryTypeCount; ++i) {
        VkMemoryType memoryType = physicalDeviceMemoryProperties.memoryTypes[i];
        if (((memoryRequirements.memoryTypeBits & memoryType.propertyFlags) == memoryRequirements.memoryTypeBits) &&
                (memoryType.propertyFlags & shouldSupportBits) == shouldSupportBits) {
            memoryTypeIndex = i;
            break;
        }
    }

    VkMemoryAllocateInfo memoryAllocateInfo = {
            .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
            .allocationSize = memoryRequirements.size,
            .memoryTypeIndex = static_cast<uint32_t>(memoryTypeIndex)
    };
    VkResult result = vkAllocateMemory(_renderDevicePtr->vkLogicDevice, &memoryAllocateInfo, nullptr, &bufferMemory);
    htCheckVKOp(result, "VK Buffer Memory allocate fail.");
    result = vkBindBufferMemory(_renderDevicePtr->vkLogicDevice, buffer, bufferMemory, VkDeviceSize(0));
    htCheckVKOp(result, "VK Buffer Memory bind fail.");
}