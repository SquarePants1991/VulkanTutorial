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
#include "HTRenderSurface.hpp"
#include <iostream>
#include <vulkan/vulkan.h>

typedef VkPhysicalDevice (* HTRenderDevicePickPhysicsDeviceCallback)(std::vector<VkPhysicalDevice> &physicsDevices);

class HTRenderDevice {
    public:
    VkPhysicalDevice vkPhysicsDevice;
    int graphicsQueueFamilyIndex;
    VkQueue graphicsQueue;
    int presentQueueFamilyIndex;
    VkQueue presentQueue;
    VkDevice vkLogicDevice;

    HTRenderDevice(HTVulkanInstancePtr vulkanInstancePtr, HTRenderSurfacePtr renderSurfacePtr, HTRenderDevicePickPhysicsDeviceCallback physicsDevicePickCallback = nullptr);
    ~HTRenderDevice();
    
    private:
    HTRenderDevicePickPhysicsDeviceCallback _physicsDevicePickCallback;
    HTVulkanInstancePtr _vulkanInstancePtr;
    HTRenderSurfacePtr _renderSurfacePtr;
    void createPhysicsDevice();
    void findQueues();
    void createLogicDevice();
};

HTMakeClass(HTRenderDevice)
#endif /* HTRenderDevice_hpp */
