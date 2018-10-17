//
//  HTRenderDevice.hpp
//  VulkanTutorialMacOS
//
//  Created by ocean on 2018/10/8.
//  Copyright © 2018 handytool. All rights reserved.
//

#ifndef HTRenderDevice_hpp
#define HTRenderDevice_hpp

#include <iostream>
#include <vulkan/vulkan.h>

typedef VkPhysicalDevice (* HTRenderDevicePickPhysicsDeviceCallback)(std::vector<VkPhysicalDevice> &physicsDevices);

class HTRenderDevice {
    public:
    VkInstance vkInstance;
    VkPhysicalDevice vkPhysicsDevice;
    int graphicsQueueFamilyIndex;
    int presentQueueFamilyIndex;
    VkDevice vkLogicDevice;
    VkQueue graphicsQueue;
    VkQueue presentQueue;

    HTRenderDevice(HTRenderDevicePickPhysicsDeviceCallback physicsDevicePickCallback = nullptr);
    
    private:
    HTRenderDevicePickPhysicsDeviceCallback _physicsDevicePickCallback;
    void createInstance();
    void createPhysicsDevice();
    void findQueueFamilies();
    void createLogicDevice();

};
#endif /* HTRenderDevice_hpp */
