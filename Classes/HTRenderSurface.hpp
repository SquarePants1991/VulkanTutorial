//
// Created by ocean on 2018/10/18.
// Copyright (c) 2018 handytool. All rights reserved.
//

#ifndef VULKANTUTORIALMACOS_HTRENDERSURFACE_HPP
#define VULKANTUTORIALMACOS_HTRENDERSURFACE_HPP

#include <vulkan/vulkan.h>

class HTRenderDevice;
class HTRenderSurface {
public:
    VkSurfaceKHR vkSurface;
#ifdef __APPLE__
    HTRenderSurface(HTRenderDevice *device, const void *view);
#endif

};


#endif //VULKANTUTORIALMACOS_HTRENDERSURFACE_H
