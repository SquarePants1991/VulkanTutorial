//
// Created by ocean on 2018/11/8.
// Copyright (c) 2018 handytool. All rights reserved.
//

#ifndef VULKANTUTORIALMACOS_HTRENDERPIPLINE_HPP
#define VULKANTUTORIALMACOS_HTRENDERPIPLINE_HPP

#include <vulkan/vulkan.h>
#include <vector>

class HTRenderPipline {
private:
    std::vector<VkShaderModule> _shaderModules;

    VkShaderModule createShaderModule();
    std::vector<unsigned char> readFileContent(const char *path);
public:
    VkPipeline vkPipeline;

    HTRenderPipline(const char *vertexShaderPath, const char *fragmentShaderPath);
    ~HTRenderPipline();
};


#endif //VULKANTUTORIALMACOS_HTRENDERPIPLINE_HPP
