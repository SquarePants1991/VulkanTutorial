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
#include "HTFrameBufferPool.hpp"
#include "HTCommandBufferPool.hpp"
#include "HTWindow.h"

class HTVulkanTutorialWindow: public HTWindow {
public:
    HTVulkanTutorialWindow(int windowWidth, int windowHeight, const char *title = "Vulkan Tutorial"): HTWindow(windowWidth, windowHeight, title) {

    }

    void launch() override {
        HTVulkanInstancePtr vulkanInstancePtr = HTNew(HTVulkanInstance);
        HTRenderSurfacePtr renderSurfacePtr = HTNew(HTRenderSurface, vulkanInstancePtr, metalView);
        HTRenderDevicePtr renderDevicePtr = HTNew(HTRenderDevice, vulkanInstancePtr, renderSurfacePtr);
        HTSwapchainPtr swapchainPtr = HTNew(HTSwapchain, renderDevicePtr, renderSurfacePtr);
        HTRenderPassPtr renderPassPtr = HTNew(HTRenderPass, renderDevicePtr, swapchainPtr);
        HTFrameBufferPoolPtr frameBufferPoolPtr = HTNew(HTFrameBufferPool, renderDevicePtr, swapchainPtr, renderPassPtr);
        HTCommandBufferPoolPtr commandBufferPoolPtr = HTNew(HTCommandBufferPool, renderDevicePtr, swapchainPtr);
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
    }
};

int main(int argc, const char * argv[]) {
    HTVulkanTutorialWindow window(800, 600);
    window.present();
}
