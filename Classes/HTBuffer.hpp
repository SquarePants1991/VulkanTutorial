//
// Created by ocean on 2018/11/21.
// Copyright (c) 2018 handytool. All rights reserved.
//

#ifndef VULKANTUTORIALMACOS_HTBUFFER_HPP
#define VULKANTUTORIALMACOS_HTBUFFER_HPP

#include <vulkan/vulkan.h>
#include "HTRenderDevice.hpp"

class HTBuffer {
protected:
    HTRenderDevicePtr _renderDevicePtr;
    HTBuffer(HTRenderDevicePtr renderDevicePtr);
    void copyData(uint32_t sizeInBytes, void *data, VkDeviceMemory &dstMemory);
    void createBuffer(uint32_t sizeInBytes, VkBufferUsageFlagBits usageFlagBits, VkBuffer &buffer, VkDeviceMemory &bufferMemory);
};


#endif //VULKANTUTORIALMACOS_HTBUFFER_HPP
