//
// Created by ocean on 2018/11/1.
// Copyright (c) 2018 handytool. All rights reserved.
//

#include "HTRenderer.hpp"
#include "HTVKCheckUtil.hpp"

HTRenderer::HTRenderer(HTRenderDevicePtr renderDevicePtr, HTSwapchainPtr swapchainPtr, HTRenderPassPtr renderPassPtr, HTFrameBufferPoolPtr frameBufferPoolPtr, HTCommandBufferPoolPtr commandBufferPoolPtr):
        _renderDevicePtr(renderDevicePtr),
        _swapchainPtr(swapchainPtr),
        _renderPassPtr(renderPassPtr),
        _frameBufferPoolPtr(frameBufferPoolPtr),
        _commandBufferPoolPtr(commandBufferPoolPtr),
        renderHandler(nullptr) {
    createSemaphores();
}

HTRenderer::~HTRenderer() {

}

void HTRenderer::createSemaphores() {
    VkSemaphoreCreateInfo semaphoreCreateInfo = {};
    semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_EXPORT_SEMAPHORE_CREATE_INFO;
    VkResult result;
    result = vkCreateSemaphore(_renderDevicePtr->vkLogicDevice, &semaphoreCreateInfo, nullptr, &_waitImageReadySemaphore);
    htCheckVKOp(result, "VK Wait Image Semaphore fail.");
    result = vkCreateSemaphore(_renderDevicePtr->vkLogicDevice, &semaphoreCreateInfo, nullptr, &_commandBufferDidRenderSemaphore);
    htCheckVKOp(result, "VK Command Buffer Did Render Semaphore fail.");
}

void HTRenderer::render() {
    // do some draw
    int index = 0;
    for (const auto &commandBuffer: _commandBufferPoolPtr->vkCommandBuffers) {
        VkCommandBufferBeginInfo commandBufferBeginInfo = {};
        commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
        vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo);

        VkRenderPassBeginInfo renderPassBeginInfo = {};
        VkRect2D rect = {};
        VkClearValue clearColor = {0.7, 0.6, 0.0, 1.0};
        renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassBeginInfo.renderPass = _renderPassPtr->vkRenderPass;
        renderPassBeginInfo.clearValueCount = 1;
        renderPassBeginInfo.pClearValues = &clearColor;
        renderPassBeginInfo.framebuffer = _frameBufferPoolPtr->vkFramebuffers[index];
        renderPassBeginInfo.renderArea.offset = {0, 0};
        renderPassBeginInfo.renderArea.extent = _swapchainPtr->imageExtend;

        vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
        if (renderHandler != nullptr) {
            renderHandler(commandBuffer);
        }
        vkCmdEndRenderPass(commandBuffer);

        vkEndCommandBuffer(commandBuffer);
        index++;
    }
}

void HTRenderer::present() {
    uint32_t imageIndex;
    vkAcquireNextImageKHR(_renderDevicePtr->vkLogicDevice, _swapchainPtr->vkSwapchain, std::numeric_limits<uint64_t >::max(), _waitImageReadySemaphore, VK_NULL_HANDLE, &imageIndex);

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    const VkPipelineStageFlags waitStages[] = {
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
    };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &_waitImageReadySemaphore;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &_commandBufferPoolPtr->vkCommandBuffers[imageIndex];

    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &_commandBufferDidRenderSemaphore;

    VkResult result = vkQueueSubmit(_renderDevicePtr->graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    htCheckVKOp(result, "VK Queue submit fail.");

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &_commandBufferDidRenderSemaphore;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &_swapchainPtr->vkSwapchain;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = nullptr;

    result = vkQueuePresentKHR(_renderDevicePtr->presentQueue, &presentInfo);
    htCheckVKOp(result, "VK Queue present fail.");
}