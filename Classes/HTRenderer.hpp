//
// Created by ocean on 2018/11/1.
// Copyright (c) 2018 handytool. All rights reserved.
//

#ifndef VULKANTUTORIALMACOS_HTRENDERER_HPP
#define VULKANTUTORIALMACOS_HTRENDERER_HPP

#include <vulkan/vulkan.h>
#include "HTSwapchain.hpp"
#include "HTRenderDevice.hpp"
#include "HTRenderPass.hpp"
#include "HTFrameBufferPool.hpp"
#include "HTCommandBufferPool.hpp"

typedef void (* HTRendererRenderHandler)(VkCommandBuffer commandBuffer);

class HTRenderer {
private:
    VkSemaphore _waitImageReadySemaphore;
    VkSemaphore _commandBufferDidRenderSemaphore;

    HTRenderDevicePtr _renderDevicePtr;
    HTSwapchainPtr _swapchainPtr;
    HTRenderPassPtr _renderPassPtr;
    HTFrameBufferPoolPtr _frameBufferPoolPtr;
    HTCommandBufferPoolPtr _commandBufferPoolPtr;

    void createSemaphores();
public:
    HTRendererRenderHandler renderHandler;

    HTRenderer(HTRenderDevicePtr renderDevicePtr, HTSwapchainPtr swapchainPtr, HTRenderPassPtr renderPassPtr, HTFrameBufferPoolPtr frameBufferPoolPtr, HTCommandBufferPoolPtr commandBufferPoolPtr);
    ~HTRenderer();

    void render();
    void present();
};

HTMakeClass(HTRenderer)
#endif //VULKANTUTORIALMACOS_HTRENDERER_HPP
