//
// Created by ocean on 2018/10/18.
// Copyright (c) 2018 handytool. All rights reserved.
//

#include "HTRenderSurface.hpp"
#include "HTVKCheckUtil.hpp"
#include "HTRenderDevice.hpp"

#include <vector>
#ifdef __APPLE__
    #include <TargetConditionals.h>
    #if TARGET_OS_MAC
    #include <vulkan/vulkan_macos.h>
    #else
    #include <vulkan/vulkan_ios.h>
    #endif
#endif

#ifdef __APPLE__
HTRenderSurface::HTRenderSurface(HTRenderDevice *device, const void *view) {
    this->renderDevice = device;
#if TARGET_OS_MAC
    VkMacOSSurfaceCreateInfoMVK surfaceCreateInfo = {};
    surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_MACOS_SURFACE_CREATE_INFO_MVK;
#endif
#if TARGET_OS_IOS
    VkIOSSurfaceCreateInfoMVK surfaceCreateInfo = {};
#endif
    surfaceCreateInfo.pNext = NULL;
    surfaceCreateInfo.pView = view;
    surfaceCreateInfo.flags = 0;
#if TARGET_OS_MAC
    VkResult surfaceCreateResult = vkCreateMacOSSurfaceMVK(device->vkInstance, &surfaceCreateInfo, NULL, &vkSurface);
#endif
#if TARGET_OS_IOS
    VkResult surfaceCreateResult = vkCreateIOSSurfaceMVK(device->vkInstance, &surfaceCreateInfo, NULL, &vkSurface);
#endif
    htCheckVKOp(surfaceCreateResult, "VK Surface create failed.");
    if (surfaceCreateResult == VK_SUCCESS) {
        std::cout << "VK Surface create success." << std::endl;
    }
    
    findPresentQueue();
}

HTRenderSurface::~HTRenderSurface() {
    if (renderDevice != nullptr && vkSurface != nullptr) {
        vkDestroySurfaceKHR(renderDevice->vkInstance, vkSurface, NULL);
    }
}

void HTRenderSurface::findPresentQueue() {
    if (renderDevice == nullptr) {
        throw std::runtime_error("can not find present queue, because render device is null.");
    }
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(renderDevice->vkPhysicsDevice, &queueFamilyCount, nullptr);
    if (queueFamilyCount == 0) {
        throw std::runtime_error("None Queue Family Found!");
    }
    std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(renderDevice->vkPhysicsDevice, &queueFamilyCount, queueFamilyProperties.data());
    int32_t presentQueueFamilyIndex = 0;
    for (const auto &queueFamilyProperty: queueFamilyProperties) {
        VkBool32 supportPresent = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(renderDevice->vkPhysicsDevice, presentQueueFamilyIndex, vkSurface, &supportPresent);
        if (queueFamilyProperty.queueCount > 0
            && supportPresent) {
            this->presentQueueFamilyIndex = presentQueueFamilyIndex;
            break;
        }
        presentQueueFamilyIndex++;
    }
    std::cout << "VK Present Queue Found, Index : " << this->presentQueueFamilyIndex << std::endl;
    
    vkGetDeviceQueue(renderDevice->vkLogicDevice, presentQueueFamilyIndex, 0, &presentQueue);
}
#endif
