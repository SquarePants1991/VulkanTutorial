//
// Created by ocean on 2018/12/18.
// Copyright (c) 2018 handytool. All rights reserved.
//

#ifndef VULKANTUTORIALMACOS_HTTEXTURE_HPP
#define VULKANTUTORIALMACOS_HTTEXTURE_HPP

#include <vulkan/vulkan.h>
#include "HTRenderDevice.hpp"
#include "HTCommandBufferPool.hpp"

class HTTexture {
private:
    VkBuffer _imageStagingBuffer;
    VkDeviceMemory _imageStagingDeviceMemory;
    HTRenderDevicePtr _renderDevicePtr;
    HTCommandBufferPoolPtr _commandBufferPoolPtr;
    void loadImage(const char *path);
    void transitionImageLayout(VkImage image, VkFormat imageFormat, VkImageLayout srcLayout, VkImageLayout dstLayout);
    void copyBufferToImage(VkImage image, VkBuffer srcBuffer, uint32_t imageWidth, uint32_t imageHeight);
    void createImageView(VkImage image, VkFormat format, VkImageView &imageView);
    void createImageSampler(VkImage image, VkSampler &sampler);
public:
    uint32_t imageWidth;
    uint32_t imageHeight;
    VkImage vkImage;
    VkDeviceMemory vkImageDeviceMemory;
    VkImageView vkImageView;
    VkSampler vkSampler;

    HTTexture(HTRenderDevicePtr renderDevicePtr, HTCommandBufferPoolPtr commandBufferPoolPtr, const char *imagePath);
    ~HTTexture();
};
HTMakeClass(HTTexture)

#endif //VULKANTUTORIALMACOS_HTTEXTURE_HPP
