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
#include "HTRenderPipline.hpp"
#include "HTFrameBufferPool.hpp"
#include "HTCommandBufferPool.hpp"

typedef void (* HTRendererRenderHandler)(VkCommandBuffer commandBuffer);

class HTRenderer {
private:
    enum {
        MaxFrameInFlight = 3
    };
    static int _currentFrameIndex;
    std::vector<VkFence> _renderCompleteFences;
    std::vector<VkSemaphore> _waitImageReadySemaphores;
    std::vector<VkSemaphore> _commandBufferDidRenderSemaphores;

    HTRenderDevicePtr _renderDevicePtr;
    HTSwapchainPtr _swapchainPtr;
    HTRenderPassPtr _renderPassPtr;
    HTRenderPiplinePtr _renderPiplinePtr;
    HTFrameBufferPoolPtr _frameBufferPoolPtr;
    HTCommandBufferPoolPtr _commandBufferPoolPtr;

    void createSemaphores();
public:
    HTRendererRenderHandler renderHandler;

    HTRenderer(HTRenderDevicePtr renderDevicePtr, HTSwapchainPtr swapchainPtr, HTRenderPassPtr renderPassPtr, HTRenderPiplinePtr renderPiplinePtr, HTFrameBufferPoolPtr frameBufferPoolPtr, HTCommandBufferPoolPtr commandBufferPoolPtr);
    ~HTRenderer();

    void render();
    void present();
};

HTMakeClass(HTRenderer)
#endif //VULKANTUTORIALMACOS_HTRENDERER_HPP
