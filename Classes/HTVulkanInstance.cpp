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
    
    VkInstanceCreateInfo info = {};
    info.pApplicationInfo = &appInfo;
    VkResult result = vkCreateInstance(&info, NULL, &vkInstance);
    htCheckVKOp(result, "VK Instance create failed.");
    if (result == VK_SUCCESS) {
        std::cout << "VK Instance create success." << std::endl;
    }
}
