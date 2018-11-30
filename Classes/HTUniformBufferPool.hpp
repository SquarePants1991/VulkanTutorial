//
// Created by ocean on 2018/11/27.
// Copyright (c) 2018 handytool. All rights reserved.
//

#ifndef VULKANTUTORIALMACOS_HTUNIFORMBUFFERPOOL_HPP
#define VULKANTUTORIALMACOS_HTUNIFORMBUFFERPOOL_HPP

#include "HTRenderDevice.hpp"
#include "HTSwapchain.hpp"
#include "HTRenderPipline.hpp"
#include "HTUniformBuffer.hpp"

class HTUniformBufferPool {
private:
    HTRenderDevicePtr _renderDevicePtr;
    HTSwapchainPtr _swapchainPtr;
    HTRenderPiplinePtr _renderPiplinePtr;
    void createUniformBuffers();
public:
    VkDescriptorPool vkDescriptorPool;
    std::vector<HTUniformBufferPtr> uniformBuffers;
    std::vector<VkDescriptorSet > vkDescriptorSets;
    HTUniformBufferPool(HTRenderDevicePtr renderDevicePtr, HTSwapchainPtr swapchainPtr, HTRenderPiplinePtr renderPiplinePtr);
    ~HTUniformBufferPool();
};

HTMakeClass(HTUniformBufferPool)
#endif //VULKANTUTORIALMACOS_HTUNIFORMBUFFERPOOL_HPP
