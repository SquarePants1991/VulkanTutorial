//
// Created by ocean on 2018/11/1.
// Copyright (c) 2018 handytool. All rights reserved.
//

#ifndef VULKANTUTORIALMACOS_HTCOMMANDBUFFERPOOL_HPP
#define VULKANTUTORIALMACOS_HTCOMMANDBUFFERPOOL_HPP

#include <vulkan/vulkan.h>
#include "HTRenderDevice.hpp"
#include "HTSwapchain.hpp"

class HTCommandBufferPool {
private:
    HTRenderDevicePtr _renderDevicePtr;
    HTSwapchainPtr _swapchainPtr;
    void createCommandBuffers();
public:
    VkCommandPool vkCommandPool;
    std::vector<VkCommandBuffer> vkCommandBuffers;
    HTCommandBufferPool(HTRenderDevicePtr renderDevicePtr, HTSwapchainPtr swapchainPtr);
    ~HTCommandBufferPool();
};

HTMakeClass(HTCommandBufferPool)
#endif //VULKANTUTORIALMACOS_HTCOMMANDBUFFERPOOL_HPP
