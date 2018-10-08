//
//  main.cpp
//  VulkanTutorialMacOS
//
//  Created by ocean on 2018/10/8.
//  Copyright © 2018 handytool. All rights reserved.
//

#include <iostream>
#include "HTRenderDevice.hpp"
#include <vector>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_macos.h>

int main(int argc, const char * argv[]) {
    // create VK instance
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;
    
    VkInstance _instance;
    std::vector<const char *> instanceExtNames;
    instanceExtNames.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
    instanceExtNames.push_back(VK_MVK_MACOS_SURFACE_EXTENSION_NAME);
    VkInstanceCreateInfo info = {};
    info.enabledExtensionCount = (int)instanceExtNames.size();
    info.ppEnabledExtensionNames = instanceExtNames.data();
    info.pApplicationInfo = &appInfo;
    VkResult result = vkCreateInstance(&info, NULL, &_instance);
    if (result == VK_SUCCESS) {
        std::cout << "Create VK Instance success~" << std::endl;
    }
    return 0;
}
