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

HTRenderDevice::HTRenderDevice(HTVulkanInstancePtr vulkanInstancePtr, HTRenderSurfacePtr renderSurfacePtr, HTRenderDevicePickPhysicsDeviceCallback physicsDevicePickCallback):
        _physicsDevicePickCallback(physicsDevicePickCallback),
        _vulkanInstancePtr(vulkanInstancePtr),
        _renderSurfacePtr(renderSurfacePtr),
        graphicsQueueFamilyIndex(-1),
        presentQueueFamilyIndex(-1)
{
    createPhysicsDevice();
    findQueues();
    createLogicDevice();
}

HTRenderDevice::~HTRenderDevice() {
    if (vkLogicDevice != nullptr) {
        vkDestroyDevice(vkLogicDevice, NULL);
    }
}

void HTRenderDevice::createPhysicsDevice() {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(_vulkanInstancePtr->vkInstance, &deviceCount, nullptr);
    if (deviceCount == 0) {
        throw std::runtime_error("None Physics Device Found!");
    }
    std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
    vkEnumeratePhysicalDevices(_vulkanInstancePtr->vkInstance, &deviceCount, physicalDevices.data());
    if (_physicsDevicePickCallback != nullptr) {
        vkPhysicsDevice = _physicsDevicePickCallback(physicalDevices);
    } else {
        vkPhysicsDevice = physicalDevices[0];
    }
    VkPhysicalDeviceProperties physicalDeviceProperties;
    vkGetPhysicalDeviceProperties(vkPhysicsDevice, &physicalDeviceProperties);
    std::cout << "VK Physics Device: <" << physicalDeviceProperties.deviceName << "> create success." << std::endl;
}

void HTRenderDevice::findQueues() {
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicsDevice, &queueFamilyCount, nullptr);
    if (queueFamilyCount == 0) {
        throw std::runtime_error("None Queue Family Found!");
    }
    std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicsDevice, &queueFamilyCount, queueFamilyProperties.data());
    int32_t queueFamilyIndex = 0;
    for (const auto &queueFamilyProperty: queueFamilyProperties) {
        if (queueFamilyProperty.queueCount > 0) {
            // find graphics queue family
            if (queueFamilyProperty.queueFlags & VK_QUEUE_GRAPHICS_BIT
                && this->graphicsQueueFamilyIndex < 0) {
                this->graphicsQueueFamilyIndex = queueFamilyIndex;
            }
            
            // find present queue family
            if (this->presentQueueFamilyIndex < 0) {
                VkBool32 supportPresent = false;
                vkGetPhysicalDeviceSurfaceSupportKHR(vkPhysicsDevice, queueFamilyIndex, _renderSurfacePtr->vkSurface, &supportPresent);
                if (supportPresent) {
                    this->presentQueueFamilyIndex = queueFamilyIndex;
                }
            }
        }
        queueFamilyIndex++;
    }
    std::cout << "VK Graphics Queue Found, Index : " << this->graphicsQueueFamilyIndex << std::endl;
    std::cout << "VK Present Queue Found, Index : " << this->presentQueueFamilyIndex << std::endl;
}

void HTRenderDevice::createLogicDevice() {
    int queueFamilyIndexes[] = {graphicsQueueFamilyIndex, presentQueueFamilyIndex};
    uint32_t queueFamilyCount = sizeof(queueFamilyIndexes) / sizeof(uint32_t);
    std::vector<VkDeviceQueueCreateInfo> deviceQueueCreateInfos;
    float queuePriority = 1.0f;
    for (int i = 0; i < queueFamilyCount; ++i) {
        VkDeviceQueueCreateInfo deviceQueueCreateInfo = {};
        deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        deviceQueueCreateInfo.queueFamilyIndex = uint32_t(queueFamilyIndexes[i]);
        deviceQueueCreateInfo.queueCount = 1;
        deviceQueueCreateInfo.pQueuePriorities = &queuePriority;
        deviceQueueCreateInfos.push_back(deviceQueueCreateInfo);
    }
    
    VkDeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.pQueueCreateInfos = deviceQueueCreateInfos.data();
    deviceCreateInfo.queueCreateInfoCount = queueFamilyCount;
    VkResult result = vkCreateDevice(vkPhysicsDevice, &deviceCreateInfo, NULL, &vkLogicDevice);
    htCheckVKOp(result, "VK Logic Device create failed.");
    if (result == VK_SUCCESS) {
        std::cout << "VK Logic Device create success." << std::endl;
    }

    vkGetDeviceQueue(vkLogicDevice, uint32_t(graphicsQueueFamilyIndex), 0, &graphicsQueue);
    vkGetDeviceQueue(vkLogicDevice, uint32_t(presentQueueFamilyIndex), 0, &presentQueue);
}
