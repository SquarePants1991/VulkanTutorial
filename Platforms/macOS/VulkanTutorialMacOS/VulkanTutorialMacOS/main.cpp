//
//  main.cpp
//  VulkanTutorialMacOS
//
//  Created by ocean on 2018/10/8.
//  Copyright © 2018 handytool. All rights reserved.
//

#include <iostream>
#include <sstream>

#include "HTVulkanInstance.hpp"
#include "HTRenderDevice.hpp"
#include "HTRenderSurface.hpp"
#include "HTSwapchain.hpp"
#include "HTRenderPass.hpp"
#include "HTRenderPipline.hpp"
#include "HTFrameBufferPool.hpp"
#include "HTCommandBufferPool.hpp"
#include "HTRenderer.hpp"
#include "HTWindow.h"

class HTVulkanTutorialWindow: public HTWindow {
    HTVulkanInstancePtr vulkanInstancePtr;
    HTRenderSurfacePtr renderSurfacePtr;
    HTRenderDevicePtr renderDevicePtr;
    HTSwapchainPtr swapchainPtr;
    HTRenderPassPtr renderPassPtr;
    HTRenderPiplinePtr renderPiplinePtr;
    HTFrameBufferPoolPtr frameBufferPoolPtr;
    HTCommandBufferPoolPtr commandBufferPoolPtr;
    HTRendererPtr rendererPtr;
public:
    HTVulkanTutorialWindow(int windowWidth, int windowHeight, const char *title = "Vulkan Tutorial"): HTWindow(windowWidth, windowHeight, title) {

    }

    static void render(VkCommandBuffer commandBuffer) {

    }

    void launch() override {
        vulkanInstancePtr = HTNew(HTVulkanInstance);
        renderSurfacePtr = HTNew(HTRenderSurface, vulkanInstancePtr, metalView);
        renderDevicePtr = HTNew(HTRenderDevice, vulkanInstancePtr, renderSurfacePtr);
        swapchainPtr = HTNew(HTSwapchain, renderDevicePtr, renderSurfacePtr);
        renderPassPtr = HTNew(HTRenderPass, renderDevicePtr, swapchainPtr);
        renderPiplinePtr = HTNew(HTRenderPipline, renderDevicePtr, swapchainPtr, renderPassPtr, "./vert.spv", "./frag.spv");
        frameBufferPoolPtr = HTNew(HTFrameBufferPool, renderDevicePtr, swapchainPtr, renderPassPtr);
        commandBufferPoolPtr = HTNew(HTCommandBufferPool, renderDevicePtr, swapchainPtr);

        rendererPtr = HTNew(HTRenderer, renderDevicePtr, swapchainPtr, renderPassPtr, renderPiplinePtr, frameBufferPoolPtr, commandBufferPoolPtr);
        rendererPtr->renderHandler = render;
        rendererPtr->render();
        rendererPtr->present();
    }

    void loop() override {
        static int fpsCount = 0;
        static double totalTime = 0;
        fpsCount++;
        totalTime += deltaTime;
        if (fpsCount >= 30) {
            std::stringstream titleSS;
            titleSS << "fps: " << int(1.0 / (totalTime / fpsCount));
            setTitle(titleSS.str().data());
            fpsCount = 0;
            totalTime = 0;
        }

        if (rendererPtr) {
            rendererPtr->render();
            rendererPtr->present();
        }
    }
};

int main(int argc, const char * argv[]) {
    HTVulkanTutorialWindow window(800, 600);
    window.present();
}
