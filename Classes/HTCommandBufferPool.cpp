//
// Created by ocean on 2018/11/1.
// Copyright (c) 2018 handytool. All rights reserved.
//

#include "HTCommandBufferPool.hpp"
#include "HTVKCheckUtil.hpp"

HTCommandBufferPool::HTCommandBufferPool(HTRenderDevicePtr renderDevicePtr, HTSwapchainPtr swapchainPtr):
        _renderDevicePtr(renderDevicePtr),
        _swapchainPtr(swapchainPtr) {
    createCommandBuffers();
}

HTCommandBufferPool::~HTCommandBufferPool() {
    if (_renderDevicePtr && vkCommandPool != nullptr) {
        if (vkCommandBuffers.size() > 0) {
            vkFreeCommandBuffers(_renderDevicePtr->vkLogicDevice, vkCommandPool, static_cast<uint32_t>(vkCommandBuffers.size()), vkCommandBuffers.data());
        }
        if (vkCommandPool) {
            vkDestroyCommandPool(_renderDevicePtr->vkLogicDevice, vkCommandPool, nullptr);
        }
    }
}

void HTCommandBufferPool::createCommandBuffers() {
    // Create Command Pool
    VkCommandPoolCreateInfo commandPoolCreateInfo = {};
    commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolCreateInfo.queueFamilyIndex = static_cast<uint32_t>(_renderDevicePtr->graphicsQueueFamilyIndex);
    VkResult result = vkCreateCommandPool(_renderDevicePtr->vkLogicDevice, &commandPoolCreateInfo, nullptr, &vkCommandPool);
    htCheckVKOp(result, "VK Command Pool create fail.");
    if (result == VK_SUCCESS) {
        std::cout << "VK Command Pool create success." << std::endl;
    }

    // Create Command Buffers
    vkCommandBuffers.resize(_swapchainPtr->imageViews.size());
    VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
    commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocateInfo.commandBufferCount = static_cast<uint32_t>(vkCommandBuffers.size());
    commandBufferAllocateInfo.commandPool = vkCommandPool;
    commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    result = vkAllocateCommandBuffers(_renderDevicePtr->vkLogicDevice, &commandBufferAllocateInfo, vkCommandBuffers.data());
    htCheckVKOp(result, "VK Command Buffers create fail.");
    if (result == VK_SUCCESS) {
        std::cout << "VK Command Buffers create success." << std::endl;
    }
}