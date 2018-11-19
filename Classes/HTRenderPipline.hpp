//
// Created by ocean on 2018/11/8.
// Copyright (c) 2018 handytool. All rights reserved.
//

#ifndef VULKANTUTORIALMACOS_HTRENDERPIPLINE_HPP
#define VULKANTUTORIALMACOS_HTRENDERPIPLINE_HPP

#include <vulkan/vulkan.h>
#include <vector>
#include "HTRenderDevice.hpp"
#include "HTSwapchain.hpp"
#include "HTRenderPass.hpp"

class HTRenderPipline {
private:
    HTRenderDevicePtr _renderDevicePtr;
    HTSwapchainPtr _swapchainPtr;
    HTRenderPassPtr _renderPassPtr;
    std::vector<std::pair<VkShaderModule, int>> _shaderModules;

    std::vector<char> readFileContent(const char *path);
    VkShaderModule createShaderModule(std::vector<char> shaderByteCodes);
    void createPipline();
public:
    VkPipeline vkPipeline;
    VkPipelineLayout vkPiplineLayout;

    HTRenderPipline(HTRenderDevicePtr renderDevicePtr, HTSwapchainPtr swapchainPtr, HTRenderPassPtr renderPassPtr,const char *vertexShaderPath, const char *fragmentShaderPath);
    ~HTRenderPipline();
};

HTMakeClass(HTRenderPipline)
#endif //VULKANTUTORIALMACOS_HTRENDERPIPLINE_HPP
