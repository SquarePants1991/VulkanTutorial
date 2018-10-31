//
// Created by ocean on 2018/10/29.
// Copyright (c) 2018 handytool. All rights reserved.
//

#ifndef VULKANTUTORIALMACOS_HTRENDERPASS_HPP
#define VULKANTUTORIALMACOS_HTRENDERPASS_HPP

#include <vulkan/vulkan.h>

class HTRenderPass {
private:
    void createRenderPass();
public:
    HTRenderPass();
    ~HTRenderPass();
    VkRenderPass vkRenderPass;
};


#endif //VULKANTUTORIALMACOS_HTRENDERPASS_HPP
