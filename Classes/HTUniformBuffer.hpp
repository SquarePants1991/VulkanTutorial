//
// Created by ocean on 2018/11/21.
// Copyright (c) 2018 handytool. All rights reserved.
//

#ifndef VULKANTUTORIALMACOS_HTUNIFORMBUFFER_HPP
#define VULKANTUTORIALMACOS_HTUNIFORMBUFFER_HPP

#include "HTBuffer.hpp"
#include <glm/glm.hpp>

struct HTUniformData {
    glm::mat4 projection;
    glm::mat4 view;
    glm::mat4 model;
};

class HTUniformBuffer: public HTBuffer {
public:
    HTUniformData uniformData;
    VkBuffer vkBuffer;
    VkDeviceMemory vkDeviceMemory;

    HTUniformBuffer(HTRenderDevicePtr renderDevicePtr);
    ~HTUniformBuffer();

    void flush();
    uint32_t size();
};
HTMakeClass(HTUniformBuffer)
#endif //VULKANTUTORIALMACOS_HTUNIFORMBUFFER_HPP
