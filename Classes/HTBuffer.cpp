//
// Created by ocean on 2018/11/21.
// Copyright (c) 2018 handytool. All rights reserved.
//

#include "HTBuffer.hpp"
#include "HTVKCheckUtil.hpp"

HTBuffer::HTBuffer(HTRenderDevicePtr renderDevicePtr):
    _renderDevicePtr(renderDevicePtr) {

}

void HTBuffer::copyData(uint32_t sizeInBytes, void *data, VkDeviceMemory &dstMemory) {
    void *mappingZone;
    vkMapMemory(_renderDevicePtr->vkLogicDevice, dstMemory, VkDeviceSize(0), VkDeviceSize(sizeInBytes), 0, &mappingZone);
    memcpy(mappingZone, data, sizeInBytes);
    vkUnmapMemory(_renderDevicePtr->vkLogicDevice, dstMemory);
}

void HTBuffer::createBuffer(uint32_t sizeInBytes, VkBufferUsageFlagBits usageFlagBits, VkMemoryPropertyFlagBits memoryPropertyFlagBits, VkBuffer &buffer, VkDeviceMemory &bufferMemory) {
    VkBufferCreateInfo bufferCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .size = VkDeviceSize(sizeInBytes),
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .usage = usageFlagBits
    };
    htCheckVKOp(vkCreateBuffer(_renderDevicePtr->vkLogicDevice, &bufferCreateInfo, nullptr, &buffer), "VK Buffer create fail.");

    VkMemoryRequirements memoryRequirements = {};
    vkGetBufferMemoryRequirements(_renderDevicePtr->vkLogicDevice, buffer, &memoryRequirements);
    createDeviceMemory(memoryRequirements, memoryPropertyFlagBits, bufferMemory);

    VkResult result = vkBindBufferMemory(_renderDevicePtr->vkLogicDevice, buffer, bufferMemory, VkDeviceSize(0));
    htCheckVKOp(result, "VK Buffer Memory bind fail.");
}

void HTBuffer::createDeviceMemory(VkMemoryRequirements memoryRequirements, VkMemoryPropertyFlagBits memoryPropertyFlagBits, VkDeviceMemory &bufferMemory) {
    int32_t memoryTypeIndex = -1;
    VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties = {};
    vkGetPhysicalDeviceMemoryProperties(_renderDevicePtr->vkPhysicsDevice, &physicalDeviceMemoryProperties);
    int32_t shouldSupportBits = memoryPropertyFlagBits;
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
}