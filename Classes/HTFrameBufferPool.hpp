//
// Created by ocean on 2018/11/1.
// Copyright (c) 2018 handytool. All rights reserved.
//

#ifndef VULKANTUTORIALMACOS_HTFRAMEBUFFER_HPP
#define VULKANTUTORIALMACOS_HTFRAMEBUFFER_HPP

#include <vulkan/vulkan.h>
#include "HTSwapchain.hpp"
#include "HTRenderPass.hpp"

class HTFrameBufferPool {
private:
    HTRenderDevicePtr _renderDevicePtr;
    HTSwapchainPtr _swapchainPtr;
    HTRenderPassPtr _renderPassPtr;

    void createFramebuffers();
public:
    std::vector<VkFramebuffer> vkFramebuffers;

    HTFrameBufferPool(HTRenderDevicePtr renderDevicePtr, HTSwapchainPtr swapchainPtr, HTRenderPassPtr renderPassPtr);
    ~HTFrameBufferPool();

    VkFramebuffer operator [](int index) {
        return vkFramebuffers[index];
    }
};

HTMakeClass(HTFrameBufferPool)
#endif //VULKANTUTORIALMACOS_HTFRAMEBUFFER_HPP
