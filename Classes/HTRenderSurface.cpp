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
#if TARGET_OS_IOS
#include <vulkan/vulkan_ios.h>
#elif TARGET_OS_MAC
#include <vulkan/vulkan_macos.h>
#endif
#endif

#ifdef __APPLE__
HTRenderSurface::HTRenderSurface(HTVulkanInstancePtr vulkanInstancePtr, const void *view):
_vulkanInstancePtr(vulkanInstancePtr) {
#if TARGET_OS_IOS
    VkIOSSurfaceCreateInfoMVK surfaceCreateInfo = {};
    surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_IOS_SURFACE_CREATE_INFO_MVK;
#elif TARGET_OS_MAC
    VkMacOSSurfaceCreateInfoMVK surfaceCreateInfo = {};
    surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_MACOS_SURFACE_CREATE_INFO_MVK;
#endif
    surfaceCreateInfo.pNext = NULL;
    surfaceCreateInfo.pView = view;
    surfaceCreateInfo.flags = 0;
#if TARGET_OS_IOS
    VkResult surfaceCreateResult = vkCreateIOSSurfaceMVK(vulkanInstancePtr->vkInstance, &surfaceCreateInfo, NULL, &vkSurface);
#elif TARGET_OS_MAC
    VkResult surfaceCreateResult = vkCreateMacOSSurfaceMVK(vulkanInstancePtr->vkInstance, &surfaceCreateInfo, NULL, &vkSurface);
#endif
    htCheckVKOp(surfaceCreateResult, "VK Surface create failed.");
    if (surfaceCreateResult == VK_SUCCESS) {
        std::cout << "VK Surface create success." << std::endl;
    }
}

HTRenderSurface::~HTRenderSurface() {
    if (_vulkanInstancePtr != nullptr && vkSurface != nullptr) {
        vkDestroySurfaceKHR(_vulkanInstancePtr->vkInstance, vkSurface, NULL);
    }
}
#endif

