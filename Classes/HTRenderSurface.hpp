//
// Created by ocean on 2018/10/18.
// Copyright (c) 2018 handytool. All rights reserved.
//

#ifndef VULKANTUTORIALMACOS_HTRENDERSURFACE_HPP
#define VULKANTUTORIALMACOS_HTRENDERSURFACE_HPP

#include "HTVulkanInstance.hpp"
#include <vulkan/vulkan.h>

class HTRenderDevice;
class HTRenderSurface {
public:
    VkSurfaceKHR vkSurface;
#ifdef __APPLE__
    HTRenderSurface(HTVulkanInstancePtr vulkanInstancePtr, const void *view);
#endif
    ~HTRenderSurface();
private:
    HTVulkanInstancePtr _vulkanInstancePtr;
};

HTMakeClass(HTRenderSurface)
#endif //VULKANTUTORIALMACOS_HTRENDERSURFACE_H
