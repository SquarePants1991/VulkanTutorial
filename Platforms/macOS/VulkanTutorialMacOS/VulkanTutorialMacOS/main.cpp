//
//  main.cpp
//  VulkanTutorialMacOS
//
//  Created by ocean on 2018/10/8.
//  Copyright © 2018 handytool. All rights reserved.
//

#include <iostream>
#include <sstream>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include "HTVulkanInstance.hpp"
#include "HTRenderDevice.hpp"
#include "HTRenderSurface.hpp"
#include "HTSwapchain.hpp"
#include "HTRenderPass.hpp"
#include "HTRenderPipline.hpp"
#include "HTFrameBufferPool.hpp"
#include "HTCommandBufferPool.hpp"
#include "HTUniformBufferPool.hpp"
#include "HTRenderer.hpp"
#include "HTTexture.hpp"
#include "HTWindow.h"

#include "HTVertexBuffer.hpp"

class HTVulkanTutorialWindow: public HTWindow {
    HTVulkanInstancePtr vulkanInstancePtr;
    HTRenderSurfacePtr renderSurfacePtr;
    HTRenderDevicePtr renderDevicePtr;
    HTSwapchainPtr swapchainPtr;
    HTRenderPassPtr renderPassPtr;
    HTRenderPiplinePtr renderPiplinePtr;
    HTFrameBufferPoolPtr frameBufferPoolPtr;
    HTCommandBufferPoolPtr commandBufferPoolPtr;
    HTUniformBufferPoolPtr uniformBufferPoolPtr;
    HTRendererPtr rendererPtr;

    HTVertexBufferPtr vertexBufferPtr;
    std::vector<HTTexturePtr> diffuseTexturePtrs;

    float switchImageTimer;
    float switchImageInterval;
    int currentImageIndex;
public:
    HTVulkanTutorialWindow(int windowWidth, int windowHeight, const char *title = "Vulkan Tutorial"):
            HTWindow(windowWidth, windowHeight, title),
            switchImageInterval(3),
            switchImageTimer(0),
            currentImageIndex(0) {

    }

    static void render(VkCommandBuffer commandBuffer, HTUniformBufferPtr uniformBufferPtr, void *renderContext) {
        HTVulkanTutorialWindow *window = (HTVulkanTutorialWindow *)renderContext;
        window->render(commandBuffer, uniformBufferPtr);
    }

    void launch() override {
        vulkanInstancePtr = HTNew(HTVulkanInstance);
        renderSurfacePtr = HTNew(HTRenderSurface, vulkanInstancePtr, metalView);
        renderDevicePtr = HTNew(HTRenderDevice, vulkanInstancePtr, renderSurfacePtr);
        swapchainPtr = HTNew(HTSwapchain, renderDevicePtr, renderSurfacePtr);
        renderPassPtr = HTNew(HTRenderPass, renderDevicePtr, swapchainPtr);
        HTVertexInputDescriptions vertexInputDescriptions = {
                .attributeDescriptions = HTVertex::getAttributeDescriptions(),
                .bindingDescriptions = HTVertex::getBindingDescriptions()
        };
        renderPiplinePtr = HTNew(HTRenderPipline, renderDevicePtr, swapchainPtr, renderPassPtr, "./vert.spv", "./frag.spv", vertexInputDescriptions);
        frameBufferPoolPtr = HTNew(HTFrameBufferPool, renderDevicePtr, swapchainPtr, renderPassPtr);
        commandBufferPoolPtr = HTNew(HTCommandBufferPool, renderDevicePtr, swapchainPtr);
        uniformBufferPoolPtr = HTNew(HTUniformBufferPool, renderDevicePtr, swapchainPtr, renderPiplinePtr);

        rendererPtr = HTNew(HTRenderer, renderDevicePtr, swapchainPtr, renderPassPtr, renderPiplinePtr, frameBufferPoolPtr, commandBufferPoolPtr, uniformBufferPoolPtr);
        rendererPtr->renderHandler = render;
        rendererPtr->renderContext = reinterpret_cast<void *>(this);

        HTVertex vertices[] = {
                {{-0.5, 0.5, 0}, {1.0, 0.0, 1.0}, {0.0, 0.0}},
                {{0.45, 0.5, 0}, {1.0, 0.0, 1.0}, {1.0, 0.0}},
                {{0.5, -0.5, 0}, {0.0, 1.0, 1.0}, {1.0, 1.0}},
                {{-0.5, -0.5, 0}, {1.0, 1.0, 0.0}, {0.0, 1.0}},
        };

        uint16_t indexes[] = {
                0, 1, 2,
                2, 3, 0
        };
        vertexBufferPtr = HTNew(HTVertexBuffer, renderDevicePtr, vertices, sizeof(vertices) / sizeof(HTVertex), indexes, sizeof(indexes) / sizeof(uint16_t));

        HTTexturePtr diffuseTexturePtr1 = HTNew(HTTexture, renderDevicePtr, commandBufferPoolPtr, "./1.jpg");
        HTTexturePtr diffuseTexturePtr2 = HTNew(HTTexture, renderDevicePtr, commandBufferPoolPtr, "./2.jpg");
        diffuseTexturePtrs.push_back(diffuseTexturePtr1);
        diffuseTexturePtrs.push_back(diffuseTexturePtr2);

        elapsedTime = 0.0f;
    }

    void render(VkCommandBuffer commandBuffer, HTUniformBufferPtr uniformBufferPtr) {
        // Update Uniform Data
        float screenW = float(swapchainPtr->imageExtend.width);
        float screenH = float(swapchainPtr->imageExtend.height);
        float animateFactor = (sin(elapsedTime) + 1) * 0.5f;

#define USE_PERSPECTIVE 0
#if USE_PERSPECTIVE
        float aspect = screenW / screenH;
        uniformBufferPtr->uniformData.projection = glm::perspective(glm::radians(60.0f), aspect, 0.1f, 1000.0f);
        uniformBufferPtr->uniformData.view = glm::lookAt(glm::vec3(0, 0, -1), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
        uniformBufferPtr->uniformData.model = glm::rotate<float, glm::highp>(animateFactor, glm::vec3(1.0f, 0.0f, 0.0f));
#else
        float scale = 100.0f * animateFactor;
        uniformBufferPtr->uniformData.projection = glm::ortho(-screenW / 2.0f, screenW / 2.0f, -screenH / 2.0f, screenH / 2.0f, 0.1f, 1000.0f);
        uniformBufferPtr->uniformData.view = glm::lookAt(glm::vec3(0, 0, -1), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
        uniformBufferPtr->uniformData.model = glm::scale<float, glm::highp>(glm::vec3(scale, scale, scale));
#endif

        switchImageTimer += deltaTime;
        if (switchImageTimer > switchImageInterval) {
            currentImageIndex++;
            if (currentImageIndex > diffuseTexturePtrs.size() - 1) {
                currentImageIndex = 0;
            }
            switchImageTimer = 0;
        }
        uniformBufferPtr->diffuseMap = diffuseTexturePtrs[currentImageIndex];
        uniformBufferPtr->flush();

        if (vertexBufferPtr) {
            VkBuffer buffers[] = {vertexBufferPtr->vkVertexBuffer};
            VkDeviceSize offsets[] = {0};
            vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
            if (vertexBufferPtr->supportIndexMode()) {
                vkCmdBindIndexBuffer(commandBuffer, vertexBufferPtr->vkIndexBuffer, VkDeviceSize(0), VK_INDEX_TYPE_UINT16);
                vkCmdDrawIndexed(commandBuffer, vertexBufferPtr->indexCount(), 1, 0, 0, 0);
            } else {
                vkCmdDraw(commandBuffer, vertexBufferPtr->vertexCount(), 1, 0, 0);
            }
        }
    }

    void loop() override {
        static int fpsCount = 0;
        static double totalTime = 0;
        fpsCount++;
        totalTime += deltaTime;
        elapsedTime += deltaTime;
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
