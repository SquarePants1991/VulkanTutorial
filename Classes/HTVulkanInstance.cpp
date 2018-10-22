//
//  HTVulkanInstance.cpp
//  VulkanTutorialiOS
//
//  Created by ocean on 2018/10/18.
//  Copyright © 2018 handytool. All rights reserved.
//

#include "HTVulkanInstance.hpp"
#include "HTVKCheckUtil.hpp"

#include <iostream>
#include <vector>
#ifdef __APPLE__
#include <TargetConditionals.h>
#if TARGET_OS_MAC
#include <vulkan/vulkan_macos.h>
#endif
#if TARGET_OS_IOS
#include <vulkan/vulkan_ios.h>
#endif
#endif

HTVulkanInstance::HTVulkanInstance() {
    createInstance();
}

HTVulkanInstance::~HTVulkanInstance() {
    if (vkInstance != nullptr) {
        vkDestroyInstance(vkInstance, nullptr);
    }
}

void HTVulkanInstance::createInstance() {
    // VK Application Info
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Vulkan Tutorial";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;
    
    std::vector<const char *> instanceExtNames;
    instanceExtNames.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
#ifdef __APPLE__
#if TARGET_OS_MAC
    instanceExtNames.push_back(VK_MVK_MACOS_SURFACE_EXTENSION_NAME);
#endif
#if TARGET_OS_IOS
    instanceExtNames.push_back(VK_MVK_IOS_SURFACE_EXTENSION_NAME);
#endif
#endif
    VkInstanceCreateInfo info = {};
    info.pApplicationInfo = &appInfo;
    info.enabledExtensionCount = uint32_t(instanceExtNames.size());
    info.ppEnabledExtensionNames = instanceExtNames.data();
    VkResult result = vkCreateInstance(&info, NULL, &vkInstance);
    htCheckVKOp(result, "VK Instance create failed.");
    if (result == VK_SUCCESS) {
        std::cout << "VK Instance create success." << std::endl;
    }
}
