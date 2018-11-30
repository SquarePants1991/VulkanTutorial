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
#include "HTUniformBufferPool.hpp"

typedef void (* HTRendererRenderHandler)(VkCommandBuffer commandBuffer, HTUniformBufferPtr uniformBufferPtr, void *renderContext);

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
    HTUniformBufferPoolPtr _uniformBufferPoolPtr;

    void createSemaphores();
public:
    HTRendererRenderHandler renderHandler;
    void *renderContext;

    HTRenderer(HTRenderDevicePtr renderDevicePtr, HTSwapchainPtr swapchainPtr, HTRenderPassPtr renderPassPtr, HTRenderPiplinePtr renderPiplinePtr, HTFrameBufferPoolPtr frameBufferPoolPtr, HTCommandBufferPoolPtr commandBufferPoolPtr, HTUniformBufferPoolPtr uniformBufferPoolPtr);
    ~HTRenderer();

    void render();
    void present();
};

HTMakeClass(HTRenderer)
#endif //VULKANTUTORIALMACOS_HTRENDERER_HPP
