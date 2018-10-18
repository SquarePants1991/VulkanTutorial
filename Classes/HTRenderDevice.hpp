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

class HTRenderDevice {
    public:
    VkInstance vkInstance;
    
    HTRenderDevice();
    ~HTRenderDevice();
    
    private:
    void createInstance();
};
#endif /* HTRenderDevice_hpp */
