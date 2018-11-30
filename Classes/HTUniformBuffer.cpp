//
// Created by ocean on 2018/11/21.
// Copyright (c) 2018 handytool. All rights reserved.
//

#include "HTUniformBuffer.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/vec3.hpp>

HTUniformBuffer::HTUniformBuffer(HTRenderDevicePtr renderDevicePtr):
    HTBuffer(renderDevicePtr) {
    uniformData = {
            .projection = glm::identity<glm::mat4>(),
            .model = glm::identity<glm::mat4>(),
            .view = glm::identity<glm::mat4>()
    };
    createBuffer(sizeof(HTUniformBuffer), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, vkBuffer, vkDeviceMemory);
    copyData(sizeof(HTUniformBuffer), &uniformData, vkDeviceMemory);
}

HTUniformBuffer::~HTUniformBuffer() {
    if (vkBuffer) {
        vkDestroyBuffer(_renderDevicePtr->vkLogicDevice, vkBuffer, nullptr);
    }
    if (vkDeviceMemory) {
        vkFreeMemory(_renderDevicePtr->vkLogicDevice, vkDeviceMemory, nullptr);
    }
}

uint32_t HTUniformBuffer::size() {
    return sizeof(HTUniformBuffer);
}

void HTUniformBuffer::flush() {
    copyData(sizeof(HTUniformBuffer), &uniformData, vkDeviceMemory);
}