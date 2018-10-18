//
// Created by ocean on 2018/10/18.
// Copyright (c) 2018 handytool. All rights reserved.
//

#include "HTRenderSurface.hpp"
#include "HTVKCheckUtil.hpp"
#include "HTRenderDevice.hpp"

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
}
#endif