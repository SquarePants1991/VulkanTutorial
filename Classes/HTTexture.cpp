//
// Created by ocean on 2018/12/18.
// Copyright (c) 2018 handytool. All rights reserved.
//

#include "HTTexture.hpp"
#include "HTBuffer.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "./ThirdParty/stb/stb_image.h"
#include "HTVKCheckUtil.hpp"

HTTexture::HTTexture(HTRenderDevicePtr renderDevicePtr, HTCommandBufferPoolPtr commandBufferPoolPtr, const char *imagePath):
    _renderDevicePtr(renderDevicePtr),
    _commandBufferPoolPtr(commandBufferPoolPtr) {
    loadImage(imagePath);
}

HTTexture::~HTTexture() {
    if (vkImage) {
        vkDestroyImage(_renderDevicePtr->vkLogicDevice, vkImage, nullptr);
    }
    if (vkImageDeviceMemory) {
        vkFreeMemory(_renderDevicePtr->vkLogicDevice, vkImageDeviceMemory, nullptr);
    }
    if (vkImageView) {
        vkDestroyImageView(_renderDevicePtr->vkLogicDevice, vkImageView, nullptr);
    }
}

void HTTexture::loadImage(const char *path) {
    // load pixel to staging bufferA
    int imageChannel;
    stbi_uc *pixels = stbi_load(path, reinterpret_cast<int *>(&imageWidth), reinterpret_cast<int *>(&imageHeight), &imageChannel, STBI_rgb_alpha);
    if (pixels) {
        uint32_t sizeOfImageInBytes = static_cast<uint32_t>(imageWidth * imageHeight * 4);
        HTBuffer buffer(_renderDevicePtr);
        VkMemoryPropertyFlagBits memoryPropertyFlagBits = VkMemoryPropertyFlagBits(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        buffer.createBuffer(sizeOfImageInBytes, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, memoryPropertyFlagBits, _imageStagingBuffer, _imageStagingDeviceMemory);
        buffer.copyData(sizeOfImageInBytes, pixels, _imageStagingDeviceMemory);
        stbi_image_free(pixels);
    }

    VkExtent3D imageExtent = {
            .width = static_cast<uint32_t >(imageWidth),
            .height = static_cast<uint32_t >(imageHeight),
            .depth = 1
    };
    // create image object
    VkFormat defaultImageFormat = VK_FORMAT_R8G8B8A8_UNORM;
    VkImageCreateInfo imageCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            .usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .format = defaultImageFormat,
            .extent = imageExtent,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .imageType = VK_IMAGE_TYPE_2D,
            .mipLevels = 1,
            .arrayLayers = 1
    };
    VkResult result = vkCreateImage(_renderDevicePtr->vkLogicDevice, &imageCreateInfo, nullptr, &vkImage);
    htCheckVKOp(result, "VK Image create fail.");
    HTBuffer buffer(_renderDevicePtr);
    VkMemoryRequirements imageMemoryRequirements = {};
    vkGetImageMemoryRequirements(_renderDevicePtr->vkLogicDevice, vkImage, &imageMemoryRequirements);
    buffer.createDeviceMemory(imageMemoryRequirements, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vkImageDeviceMemory);
    vkBindImageMemory(_renderDevicePtr->vkLogicDevice, vkImage, vkImageDeviceMemory, VkDeviceSize(0));

    transitionImageLayout(vkImage, defaultImageFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    copyBufferToImage(vkImage, _imageStagingBuffer, imageWidth, imageHeight);
    transitionImageLayout(vkImage, defaultImageFormat, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    createImageView(vkImage, defaultImageFormat, vkImageView);
    createImageSampler(vkImage, vkSampler);

    vkDestroyBuffer(_renderDevicePtr->vkLogicDevice, _imageStagingBuffer, nullptr);
    vkFreeMemory(_renderDevicePtr->vkLogicDevice, _imageStagingDeviceMemory, nullptr);
}


void HTTexture::transitionImageLayout(VkImage image, VkFormat imageFormat, VkImageLayout srcLayout, VkImageLayout dstLayout) {
    VkCommandBuffer commandBuffer = _commandBufferPoolPtr->beginOneTimeCommands();

    VkPipelineStageFlags srcStageMask, dstStageMask;
    VkAccessFlags srcAccessMask, dstAccessMask;
    switch (srcLayout) {
        case VK_IMAGE_LAYOUT_UNDEFINED:
            srcStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            srcAccessMask = 0;
            break;
        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
            srcStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
            srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        default:
            break;
    }

    switch (dstLayout) {
        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
            dstStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
            dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            break;
        case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
            dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        default:
            break;
    }

    VkImageMemoryBarrier imageMemoryBarrier = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
            .image = image,
            .oldLayout = srcLayout,
            .newLayout = dstLayout,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .subresourceRange.layerCount = 1,
            .subresourceRange.baseArrayLayer = 0,
            .subresourceRange.levelCount = 1,
            .subresourceRange.baseMipLevel = 0,
            .srcAccessMask = srcAccessMask,
            .dstAccessMask = dstAccessMask
    };
    vkCmdPipelineBarrier(commandBuffer, srcStageMask, dstStageMask, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);

    _commandBufferPoolPtr->endOneTimeCommands(commandBuffer);
}

void HTTexture::copyBufferToImage(VkImage image, VkBuffer srcBuffer, uint32_t imageWidth, uint32_t imageHeight) {
    VkCommandBuffer commandBuffer = _commandBufferPoolPtr->beginOneTimeCommands();

    VkExtent3D imageExtent = {
            .depth = 1,
            .width = imageWidth,
            .height = imageHeight
    };
    VkOffset3D imageOffset = { 0, 0, 0 };
    VkBufferImageCopy region = {
            .imageExtent = imageExtent,
            .imageOffset = imageOffset,
            .imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .imageSubresource.mipLevel = 0,
            .imageSubresource.layerCount = 1,
            .imageSubresource.baseArrayLayer = 0,
            .bufferOffset = VkDeviceSize(0),
            .bufferRowLength = 0,
            .bufferImageHeight = 0
    };
    vkCmdCopyBufferToImage(commandBuffer, srcBuffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    _commandBufferPoolPtr->endOneTimeCommands(commandBuffer);
}

void HTTexture::createImageView(VkImage image, VkFormat format, VkImageView &imageView) {
    VkImageViewCreateInfo imageViewCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = image,
            .subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .subresourceRange.layerCount = 1,
            .subresourceRange.baseArrayLayer = 0,
            .subresourceRange.levelCount = 1,
            .subresourceRange.baseMipLevel = 0,
            .format = format,
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .components.a = VK_COMPONENT_SWIZZLE_IDENTITY,
            .components.r = VK_COMPONENT_SWIZZLE_IDENTITY,
            .components.g = VK_COMPONENT_SWIZZLE_IDENTITY,
            .components.b = VK_COMPONENT_SWIZZLE_IDENTITY
    };
    VkResult result = vkCreateImageView(_renderDevicePtr->vkLogicDevice, &imageViewCreateInfo, nullptr, &imageView);
    htCheckVKOp(result, "VK Image View create fail.");
}

void HTTexture::createImageSampler(VkImage image, VkSampler &sampler) {
    VkSamplerCreateInfo samplerCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
            .addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
            .addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
            .addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
            .anisotropyEnable = VK_FALSE,
            .maxAnisotropy = 6,
            .compareEnable = VK_FALSE,
            .compareOp = VK_COMPARE_OP_ALWAYS,
            .unnormalizedCoordinates = VK_FALSE,
            .minFilter = VK_FILTER_LINEAR,
            .magFilter = VK_FILTER_LINEAR,
            .borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK,
            .minLod = 0.0,
            .maxLod = 0.0,
            .mipLodBias = 0.0,
            .mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR
    };
    VkResult result = vkCreateSampler(_renderDevicePtr->vkLogicDevice, &samplerCreateInfo, nullptr, &sampler);
    htCheckVKOp(result, "VK Image Sampler create fail.");
}
