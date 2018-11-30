//
// Created by ocean on 2018/11/1.
// Copyright (c) 2018 handytool. All rights reserved.
//

#include "HTRenderer.hpp"
#include "HTVKCheckUtil.hpp"

int HTRenderer::_currentFrameIndex = 0;

HTRenderer::HTRenderer(HTRenderDevicePtr renderDevicePtr, HTSwapchainPtr swapchainPtr, HTRenderPassPtr renderPassPtr, HTRenderPiplinePtr renderPiplinePtr, HTFrameBufferPoolPtr frameBufferPoolPtr, HTCommandBufferPoolPtr commandBufferPoolPtr, HTUniformBufferPoolPtr uniformBufferPoolPtr):
        _renderDevicePtr(renderDevicePtr),
        _swapchainPtr(swapchainPtr),
        _renderPassPtr(renderPassPtr),
        _renderPiplinePtr(renderPiplinePtr),
        _frameBufferPoolPtr(frameBufferPoolPtr),
        _commandBufferPoolPtr(commandBufferPoolPtr),
        _uniformBufferPoolPtr(uniformBufferPoolPtr),
        renderHandler(nullptr) {
    createSemaphores();
}

HTRenderer::~HTRenderer() {
    for (int i = 0; i < MaxFrameInFlight; ++i) {
        vkDestroyFence(_renderDevicePtr->vkLogicDevice, _renderCompleteFences[i], nullptr);
        vkDestroySemaphore(_renderDevicePtr->vkLogicDevice, _waitImageReadySemaphores[i], nullptr);
        vkDestroySemaphore(_renderDevicePtr->vkLogicDevice, _commandBufferDidRenderSemaphores[i], nullptr);
    }
}

void HTRenderer::createSemaphores() {
    _renderCompleteFences.resize(MaxFrameInFlight);
    _waitImageReadySemaphores.resize(MaxFrameInFlight);
    _commandBufferDidRenderSemaphores.resize(MaxFrameInFlight);

    for (int i = 0; i < MaxFrameInFlight; ++i) {
        VkResult result;
        VkFenceCreateInfo fenceCreateInfo = {};
        fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
        result = vkCreateFence(_renderDevicePtr->vkLogicDevice, &fenceCreateInfo, nullptr, &_renderCompleteFences[i]);
        htCheckVKOp(result, "VK Render Complete Fence create fail.");

        VkSemaphoreCreateInfo semaphoreCreateInfo = {};
        semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_EXPORT_SEMAPHORE_CREATE_INFO;
        result = vkCreateSemaphore(_renderDevicePtr->vkLogicDevice, &semaphoreCreateInfo, nullptr, &_waitImageReadySemaphores[i]);
        htCheckVKOp(result, "VK Wait Image Semaphore create fail.");
        result = vkCreateSemaphore(_renderDevicePtr->vkLogicDevice, &semaphoreCreateInfo, nullptr, &_commandBufferDidRenderSemaphores[i]);
        htCheckVKOp(result, "VK Command Buffer Did Render Semaphore create fail.");
    }
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
        VkClearValue clearColor = {0.9, 0.6, 0.0, 1.0};
        renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassBeginInfo.renderPass = _renderPassPtr->vkRenderPass;
        renderPassBeginInfo.clearValueCount = 1;
        renderPassBeginInfo.pClearValues = &clearColor;
        renderPassBeginInfo.framebuffer = _frameBufferPoolPtr->vkFramebuffers[index];
        renderPassBeginInfo.renderArea.offset = {0, 0};
        renderPassBeginInfo.renderArea.extent = _swapchainPtr->imageExtend;

        vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _renderPiplinePtr->vkPipeline);
        VkDescriptorSet currentDescriptorSet = _uniformBufferPoolPtr->vkDescriptorSets[index];
        HTUniformBufferPtr currentUniformBuffer = _uniformBufferPoolPtr->uniformBuffers[index];
        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _renderPiplinePtr->vkPiplineLayout, 0, 1, &currentDescriptorSet, 0, nullptr);
        if (renderHandler != nullptr) {
            renderHandler(commandBuffer, currentUniformBuffer, renderContext);
        }
        vkCmdEndRenderPass(commandBuffer);

        vkEndCommandBuffer(commandBuffer);
        index++;
    }
}

void HTRenderer::present() {
    vkWaitForFences(_renderDevicePtr->vkLogicDevice, 1, &_renderCompleteFences[_currentFrameIndex], VK_TRUE, std::numeric_limits<uint64_t >::max());
    vkResetFences(_renderDevicePtr->vkLogicDevice, 1, &_renderCompleteFences[_currentFrameIndex]);

    uint32_t imageIndex;
    vkAcquireNextImageKHR(_renderDevicePtr->vkLogicDevice, _swapchainPtr->vkSwapchain, std::numeric_limits<uint64_t >::max(), _waitImageReadySemaphores[_currentFrameIndex], VK_NULL_HANDLE, &imageIndex);

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    const VkPipelineStageFlags waitStages[] = {
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
    };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &_waitImageReadySemaphores[_currentFrameIndex];
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &_commandBufferPoolPtr->vkCommandBuffers[imageIndex];

    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &_commandBufferDidRenderSemaphores[_currentFrameIndex];

    VkResult result = vkQueueSubmit(_renderDevicePtr->graphicsQueue, 1, &submitInfo, _renderCompleteFences[_currentFrameIndex]);
    htCheckVKOp(result, "VK Queue submit fail.");

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &_commandBufferDidRenderSemaphores[_currentFrameIndex];
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &_swapchainPtr->vkSwapchain;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = nullptr;

    result = vkQueuePresentKHR(_renderDevicePtr->presentQueue, &presentInfo);
    htCheckVKOp(result, "VK Queue present fail.");

    _currentFrameIndex = (_currentFrameIndex + 1) % MaxFrameInFlight;
}
