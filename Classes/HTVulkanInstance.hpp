//
//  HTVulkanInstance.hpp
//  VulkanTutorialiOS
//
//  Created by ocean on 2018/10/18.
//  Copyright © 2018 handytool. All rights reserved.
//

#ifndef HTVulkanInstance_hpp
#define HTVulkanInstance_hpp

#include <vulkan/vulkan.h>

class HTVulkanInstance {
public:
    VkInstance vkInstance;
    HTVulkanInstance();
    ~HTVulkanInstance();
private:
    void createInstance();
};
#endif /* HTVulkanInstance_hpp */
