//
//  HTRenderDevice.cpp
//  VulkanTutorialMacOS
//
//  Created by ocean on 2018/10/8.
//  Copyright © 2018 handytool. All rights reserved.
//

#include "HTRenderDevice.hpp"
#include "HTVKCheckUtil.hpp"
#include <vector>

#ifdef __APPLE__
#include <TargetConditionals.h>
#if TARGET_OS_MAC
#include <vulkan/vulkan_macos.h>
#else
#include <vulkan/vulkan_ios.h>
#endif
#endif

HTRenderDevice::HTRenderDevice(HTRenderDevicePickPhysicsDeviceCallback physicsDevicePickCallback):
        _physicsDevicePickCallback(physicsDevicePickCallback),
        graphicsQueueFamilyIndex(-1)
{
    createInstance();
    createPhysicsDevice();
    findGraphicsQueue();
    createLogicDevice();
}

HTRenderDevice::~HTRenderDevice() {
    if (vkInstance != nullptr) {
        vkDestroyInstance(vkInstance, NULL);
    }
    if (vkLogicDevice != nullptr) {
        vkDestroyDevice(vkLogicDevice, NULL);
    }
}

void HTRenderDevice::createInstance() {
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

void HTRenderDevice::createPhysicsDevice() {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(vkInstance, &deviceCount, nullptr);
    if (deviceCount == 0) {
        throw std::runtime_error("None Physics Device Found!");
    }
    std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
    vkEnumeratePhysicalDevices(vkInstance, &deviceCount, physicalDevices.data());
    if (_physicsDevicePickCallback != nullptr) {
        vkPhysicsDevice = _physicsDevicePickCallback(physicalDevices);
    } else {
        vkPhysicsDevice = physicalDevices[0];
    }
    VkPhysicalDeviceProperties physicalDeviceProperties;
    vkGetPhysicalDeviceProperties(vkPhysicsDevice, &physicalDeviceProperties);
    std::cout << "VK Physics Device: <" << physicalDeviceProperties.deviceName << "> create success." << std::endl;
}

void HTRenderDevice::findGraphicsQueue() {
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicsDevice, &queueFamilyCount, nullptr);
    if (queueFamilyCount == 0) {
        throw std::runtime_error("None Queue Family Found!");
    }
    std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicsDevice, &queueFamilyCount, queueFamilyProperties.data());
    int32_t graphicsQueueFamilyIndex = 0;
    for (const auto &queueFamilyProperty: queueFamilyProperties) {
        if (queueFamilyProperty.queueCount > 0
                && queueFamilyProperty.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            this->graphicsQueueFamilyIndex = graphicsQueueFamilyIndex;
            break;
        }
        graphicsQueueFamilyIndex++;
    }
    std::cout << "VK Graphics Queue Found, Index : " << this->graphicsQueueFamilyIndex << std::endl;
}

void HTRenderDevice::createLogicDevice() {
    VkDeviceQueueCreateInfo deviceQueueCreateInfo = {};
    deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    deviceQueueCreateInfo.queueFamilyIndex = uint32_t(graphicsQueueFamilyIndex);
    deviceQueueCreateInfo.queueCount = 1;
    float queuePriority = 1.0f;
    deviceQueueCreateInfo.pQueuePriorities = &queuePriority;

    VkDeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.pQueueCreateInfos = &deviceQueueCreateInfo;
    deviceCreateInfo.queueCreateInfoCount = 1;
    VkResult result = vkCreateDevice(vkPhysicsDevice, &deviceCreateInfo, NULL, &vkLogicDevice);
    htCheckVKOp(result, "VK Logic Device create failed.");
    if (result == VK_SUCCESS) {
        std::cout << "VK Logic Device create success." << std::endl;
    }

    // 获取用于绘制的Graphics队列的VKQueue对象
    vkGetDeviceQueue(vkLogicDevice, uint32_t(graphicsQueueFamilyIndex), 0, &graphicsQueue);
}
