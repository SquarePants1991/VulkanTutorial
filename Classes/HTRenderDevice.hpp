//
//  HTRenderDevice.hpp
//  VulkanTutorialMacOS
//
//  Created by ocean on 2018/10/8.
//  Copyright © 2018 handytool. All rights reserved.
//

#ifndef HTRenderDevice_hpp
#define HTRenderDevice_hpp

#include "HTVulkanInstance.hpp"
#include <iostream>
#include <vulkan/vulkan.h>

typedef VkPhysicalDevice (* HTRenderDevicePickPhysicsDeviceCallback)(std::vector<VkPhysicalDevice> &physicsDevices);

class HTRenderDevice {
    public:
    VkPhysicalDevice vkPhysicsDevice;
    int graphicsQueueFamilyIndex;
    VkQueue graphicsQueue;
    VkDevice vkLogicDevice;

    HTRenderDevice(HTVulkanInstancePtr vulkanInstancePtr, HTRenderDevicePickPhysicsDeviceCallback physicsDevicePickCallback = nullptr);
    ~HTRenderDevice();
    
    private:
    HTRenderDevicePickPhysicsDeviceCallback _physicsDevicePickCallback;
    HTVulkanInstancePtr _vulkanInstancePtr;
    void createPhysicsDevice();
    void findGraphicsQueue();
    void createLogicDevice();
};

HTMakeClass(HTRenderDevice)
#endif /* HTRenderDevice_hpp */
