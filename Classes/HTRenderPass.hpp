//
// Created by ocean on 2018/10/29.
// Copyright (c) 2018 handytool. All rights reserved.
//

#ifndef VULKANTUTORIALMACOS_HTRENDERPASS_HPP
#define VULKANTUTORIALMACOS_HTRENDERPASS_HPP

#include <vulkan/vulkan.h>
#import "HTSwapchain.hpp"

class HTRenderPass {
private:
    HTSwapchainPtr _swapchainPtr;
    HTRenderDevicePtr _renderDevicePtr;
    void createRenderPass();
public:
    HTRenderPass(HTRenderDevicePtr renderDevicePtr, HTSwapchainPtr swapchainPtr);
    ~HTRenderPass();
    VkRenderPass vkRenderPass;
};
HTMakeClass(HTRenderPass)
#endif //VULKANTUTORIALMACOS_HTRENDERPASS_HPP
