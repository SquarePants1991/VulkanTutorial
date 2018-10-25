//
//  HTSwapchain.cpp
//  VulkanTutorialMacOS
//
//  Created by ocean on 2018/10/23.
//  Copyright © 2018 handytool. All rights reserved.
//

#include "HTSwapchain.hpp"
#include "HTVKCheckUtil.hpp"

HTSwapchain::HTSwapchain(HTRenderDevicePtr renderDevicePtr, HTRenderSurfacePtr renderSurfacePtr):
        _renderDevicePtr(renderDevicePtr),
        _renderSurfacePtr(renderSurfacePtr) {
    collectSwapchainRelateInfos();
    createSwapchain();
    getImagesOnChain();
}

HTSwapchain::~HTSwapchain() {
    if (vkSwapchain != nullptr) {
        vkDestroySwapchainKHR(_renderDevicePtr->vkLogicDevice, vkSwapchain, nullptr);
    }
    for (const auto &imageView: imageViews) {
        vkDestroyImageView(_renderDevicePtr->vkLogicDevice, imageView, nullptr);
    }
}

void HTSwapchain::collectSwapchainRelateInfos() {
    _surfaceCapabilities = {};
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_renderDevicePtr->vkPhysicsDevice, _renderSurfacePtr->vkSurface, &_surfaceCapabilities);

    _surfaceFormat = {VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};
    uint32_t surfaceFormatCount;
    VkResult result = vkGetPhysicalDeviceSurfaceFormatsKHR(_renderDevicePtr->vkPhysicsDevice, _renderSurfacePtr->vkSurface, &surfaceFormatCount, nullptr);
    if (result == VK_SUCCESS && surfaceFormatCount > 0) {
        // pick a format
        std::vector<VkSurfaceFormatKHR> surfaceFormats(surfaceFormatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(_renderDevicePtr->vkPhysicsDevice, _renderSurfacePtr->vkSurface, &surfaceFormatCount, surfaceFormats.data());
        _surfaceFormat = surfaceFormats[0];
        bool isSuitableFormatFound = false;
        for (const auto &format: surfaceFormats) {
            if (format.format == VK_FORMAT_B8G8R8A8_UNORM && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                _surfaceFormat = format;
                isSuitableFormatFound = true;
                break;
            }
        }
        if (!isSuitableFormatFound) {
            _surfaceFormat = surfaceFormats[0];
        }
    }

    uint32_t presentModeCount;
    result = vkGetPhysicalDeviceSurfacePresentModesKHR(_renderDevicePtr->vkPhysicsDevice, _renderSurfacePtr->vkSurface, &presentModeCount, nullptr);
    _presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
    if (result == VK_SUCCESS && presentModeCount > 0) {
        // pick a present mode
        std::vector<VkPresentModeKHR> presentModes(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(_renderDevicePtr->vkPhysicsDevice, _renderSurfacePtr->vkSurface, &presentModeCount, presentModes.data());
        if (std::find(presentModes.begin(), presentModes.end(), VK_PRESENT_MODE_MAILBOX_KHR) != presentModes.end()) {
            _presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
        } else if (std::find(presentModes.begin(), presentModes.end(), VK_PRESENT_MODE_FIFO_KHR) != presentModes.end()) {
            _presentMode = VK_PRESENT_MODE_FIFO_KHR;
        }
    }
}

void HTSwapchain::createSwapchain() {
    VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
    swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainCreateInfo.surface = _renderSurfacePtr->vkSurface;
    swapchainCreateInfo.minImageCount = _surfaceCapabilities.maxImageCount;// 使用系统支持的最大图片数目，比如双重或者三重缓冲
    swapchainCreateInfo.imageFormat = _surfaceFormat.format;
    swapchainCreateInfo.imageColorSpace = _surfaceFormat.colorSpace;
    swapchainCreateInfo.imageExtent = _surfaceCapabilities.currentExtent;
    swapchainCreateInfo.imageArrayLayers = 1;
    swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    if (_renderDevicePtr->graphicsQueueFamilyIndex != _renderDevicePtr->presentQueueFamilyIndex) {
        // 渲染和呈现不在一个队列，需要共享内存
        swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapchainCreateInfo.queueFamilyIndexCount = 2;
        int queueFamilyIndexes[] = {_renderDevicePtr->graphicsQueueFamilyIndex, _renderDevicePtr->presentQueueFamilyIndex};
        swapchainCreateInfo.pQueueFamilyIndices = reinterpret_cast<uint32_t *>(queueFamilyIndexes);
    } else {
        swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapchainCreateInfo.queueFamilyIndexCount = 0;
        swapchainCreateInfo.pQueueFamilyIndices = nullptr;
    }
    swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainCreateInfo.presentMode = _presentMode;
    swapchainCreateInfo.clipped = VK_TRUE;
    swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;
    VkResult result = vkCreateSwapchainKHR(_renderDevicePtr->vkLogicDevice, &swapchainCreateInfo, nullptr, &vkSwapchain);
    htCheckVKOp(result, "VK Swapchain create fail.");
    if (result == VK_SUCCESS) {
        std::cout << "VK Swapchain create success." << std::endl;
    }
}

void HTSwapchain::getImagesOnChain() {
    uint32_t imageCount;
    VkResult result = vkGetSwapchainImagesKHR(_renderDevicePtr->vkLogicDevice, vkSwapchain, &imageCount, nullptr);
    if (result == VK_SUCCESS && imageCount > 0) {
        imagesOnChain.resize(imageCount);
        vkGetSwapchainImagesKHR(_renderDevicePtr->vkLogicDevice, vkSwapchain, &imageCount, imagesOnChain.data());
    }

    // create image views for the images
    imageViews.resize(imagesOnChain.size());
    int index = 0;
    for (const auto image: imagesOnChain) {
        VkImageViewCreateInfo imageViewCreateInfo = {};
        imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewCreateInfo.format = _surfaceFormat.format;
        imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        imageViewCreateInfo.image = image;
        imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;

        imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
        imageViewCreateInfo.subresourceRange.levelCount = 1;
        imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
        imageViewCreateInfo.subresourceRange.layerCount = 1;

        result = vkCreateImageView(_renderDevicePtr->vkLogicDevice, &imageViewCreateInfo, nullptr, &imageViews[index]);
        htCheckVKOp(result, "VK Image View create fail.");
        if (result == VK_SUCCESS) {
            std::cout << "VK Image View create success." << std::endl;
        }
        index++;
    }
}