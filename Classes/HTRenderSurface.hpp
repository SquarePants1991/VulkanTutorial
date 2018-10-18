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
    int presentQueueFamilyIndex;
    VkQueue presentQueue;
    
    HTRenderDevice *renderDevice;
#ifdef __APPLE__
    HTRenderSurface(HTRenderDevice *device, const void *view);
#endif
    ~HTRenderSurface();

private:
    void findPresentQueue();
};


#endif //VULKANTUTORIALMACOS_HTRENDERSURFACE_H
