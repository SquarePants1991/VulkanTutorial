//
// Created by ocean on 2018/11/1.
// Copyright (c) 2018 handytool. All rights reserved.
//

#include "HTFrameBufferPool.hpp"
#include "HTVKCheckUtil.hpp"

HTFrameBufferPool::HTFrameBufferPool(HTRenderDevicePtr renderDevicePtr, HTSwapchainPtr swapchainPtr, HTRenderPassPtr renderPassPtr):
        _renderDevicePtr(renderDevicePtr),
        _swapchainPtr(swapchainPtr),
        _renderPassPtr(renderPassPtr) {
    createFramebuffers();
}

HTFrameBufferPool::~HTFrameBufferPool() {

}

void HTFrameBufferPool::createFramebuffers() {
    vkFramebuffers.resize(_swapchainPtr->imageViews.size());
    int index = 0;
    for (const auto &imageView: _swapchainPtr->imageViews) {
        VkFramebufferCreateInfo framebufferCreateInfo = {};
        framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferCreateInfo.width = _swapchainPtr->imageExtend.width;
        framebufferCreateInfo.height = _swapchainPtr->imageExtend.height;
        framebufferCreateInfo.attachmentCount = 1;
        framebufferCreateInfo.pAttachments = &imageView;
        framebufferCreateInfo.renderPass = _renderPassPtr->vkRenderPass;

        VkResult result = vkCreateFramebuffer(_renderDevicePtr->vkLogicDevice, &framebufferCreateInfo, nullptr, &vkFramebuffers[index]);
        htCheckVKOp(result, "VK Framebuffer create fail.");
        if (result == VK_SUCCESS) {
            std::cout << "VK Framebuffer create success." << std::endl;
        }
        index++;
    }
}