//
// Created by ocean on 2018/10/29.
// Copyright (c) 2018 handytool. All rights reserved.
//

#include "HTRenderPass.hpp"
#include "HTVKCheckUtil.hpp"

HTRenderPass::HTRenderPass(HTRenderDevicePtr renderDevicePtr, HTSwapchainPtr swapchainPtr):
        _renderDevicePtr(renderDevicePtr),
        _swapchainPtr(swapchainPtr) {
    createRenderPass();
}

HTRenderPass::~HTRenderPass() {
    if (_renderDevicePtr && vkRenderPass != nullptr) {
        vkDestroyRenderPass(_renderDevicePtr->vkLogicDevice, vkRenderPass, nullptr);
    }
}

void HTRenderPass::createRenderPass() {
    VkAttachmentReference attachmentReference = {};
    attachmentReference.attachment = 0;
    attachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription attachmentDescription = {};
    attachmentDescription.format = _swapchainPtr->imageFormat;
    attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    attachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;

    VkSubpassDescription subpassDescription = {};
    subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassDescription.colorAttachmentCount = 1;
    subpassDescription.pColorAttachments = &attachmentReference;

    VkRenderPassCreateInfo renderPassCreateInfo = {};
    renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassCreateInfo.attachmentCount = 1;
    renderPassCreateInfo.pAttachments = &attachmentDescription;
    renderPassCreateInfo.subpassCount = 1;
    renderPassCreateInfo.pSubpasses = &subpassDescription;

    VkResult result = vkCreateRenderPass(_renderDevicePtr->vkLogicDevice, &renderPassCreateInfo, nullptr, &vkRenderPass);
    htCheckVKOp(result, "VK Render Pass create failed.");
    if (result == VK_SUCCESS) {
        std::cout << "VK Render Pass create success." << std::endl;
    }
}