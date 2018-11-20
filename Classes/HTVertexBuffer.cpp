//
// Created by ocean on 2018/11/19.
// Copyright (c) 2018 handytool. All rights reserved.
//

#include "HTVertexBuffer.hpp"
#include "HTVKCheckUtil.hpp"

HTVertexBuffer::HTVertexBuffer(HTRenderDevicePtr renderDevicePtr, HTVertex *vertices, uint32_t vertexCount, bool shouldCopyData):
        _renderDevicePtr(renderDevicePtr),
        _vertices(vertices),
        _shouldCopyData(shouldCopyData),
        _vertexCount(vertexCount) {
    if (_shouldCopyData) {
        _vertices = (HTVertex *)malloc(vertexCount * sizeof(HTVertex));
        memcpy(_vertices, vertices, vertexCount * sizeof(HTVertex));
    }
    createBuffer();
    copyData();
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
}

void HTVertexBuffer::createBuffer() {
    VkBufferCreateInfo bufferCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .size = VkDeviceSize(_vertexCount * sizeof(HTVertex)),
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
    };
    htCheckVKOp(vkCreateBuffer(_renderDevicePtr->vkLogicDevice, &bufferCreateInfo, nullptr, &vkVertexBuffer), "VK Vertex Buffer create fail.");

    VkMemoryRequirements memoryRequirements = {};
    vkGetBufferMemoryRequirements(_renderDevicePtr->vkLogicDevice, vkVertexBuffer, &memoryRequirements);

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
    VkResult result = vkAllocateMemory(_renderDevicePtr->vkLogicDevice, &memoryAllocateInfo, nullptr, &vkVertexBufferDeviceMemory);
    htCheckVKOp(result, "VK Vertex Buffer Memory allocate fail.");
    result = vkBindBufferMemory(_renderDevicePtr->vkLogicDevice, vkVertexBuffer, vkVertexBufferDeviceMemory, VkDeviceSize(0));
    htCheckVKOp(result, "VK Vertex Buffer Memory bind fail.");
}

void HTVertexBuffer::copyData() {
    void *data;
    uint32_t dataSize = _vertexCount * sizeof(HTVertex);
    vkMapMemory(_renderDevicePtr->vkLogicDevice, vkVertexBufferDeviceMemory, VkDeviceSize(0), VkDeviceSize(dataSize), 0, &data);
    memcpy(data, reinterpret_cast<void *>(_vertices), dataSize);
    vkUnmapMemory(_renderDevicePtr->vkLogicDevice, vkVertexBufferDeviceMemory);
}