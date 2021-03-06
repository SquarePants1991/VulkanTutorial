//
// Created by ocean on 2018/11/27.
// Copyright (c) 2018 handytool. All rights reserved.
//

#include "HTUniformBufferPool.hpp"
#include "HTVKCheckUtil.hpp"

HTUniformBufferPool::HTUniformBufferPool(HTRenderDevicePtr renderDevicePtr, HTSwapchainPtr swapchainPtr, HTRenderPiplinePtr renderPiplinePtr):
        _renderDevicePtr(renderDevicePtr),
        _swapchainPtr(swapchainPtr),
        _renderPiplinePtr(renderPiplinePtr) {
    createUniformBuffers();
}

HTUniformBufferPool::~HTUniformBufferPool() {
    if (vkDescriptorPool) {
        vkFreeDescriptorSets(_renderDevicePtr->vkLogicDevice, vkDescriptorPool, static_cast<uint32_t>(vkDescriptorSets.size()), vkDescriptorSets.data());
        vkDestroyDescriptorPool(_renderDevicePtr->vkLogicDevice, vkDescriptorPool, nullptr);
    }
}

void HTUniformBufferPool::UpdateTextures() {
    uint32_t bufferCount = static_cast<uint32_t>(_swapchainPtr->imageViews.size());
    for (int i = 0; i < bufferCount; ++i) {
        HTUniformBufferPtr uniformBufferPtr = uniformBuffers[i];
        if (uniformBufferPtr->diffuseMap == nullptr) {
            continue;
        }
        VkDescriptorSet descriptorSet = vkDescriptorSets[i];

        VkDescriptorImageInfo descriptorImageInfo = {
                .sampler = uniformBufferPtr->diffuseMap->vkSampler,
                .imageView = uniformBufferPtr->diffuseMap->vkImageView,
                .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
        };

        VkWriteDescriptorSet diffuseMapWriteDescriptorSet = {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .descriptorCount = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .dstBinding = 1,
                .dstArrayElement = 0,
                .dstSet = descriptorSet,
                .pImageInfo = &descriptorImageInfo
        };

        VkWriteDescriptorSet writeDescriptorSets[] = {
                diffuseMapWriteDescriptorSet
        };

        vkUpdateDescriptorSets(_renderDevicePtr->vkLogicDevice, sizeof(writeDescriptorSets) / sizeof(VkWriteDescriptorSet), writeDescriptorSets, 0, nullptr);
    }
}

void HTUniformBufferPool::createUniformBuffers() {
    uint32_t bufferCount = static_cast<uint32_t>(_swapchainPtr->imageViews.size());
    uniformBuffers.resize(bufferCount);
    vkDescriptorSets.resize(bufferCount);

    for (int i = 0; i < bufferCount; ++i) {
        uniformBuffers[i] = HTNew(HTUniformBuffer, _renderDevicePtr);
    }

    VkDescriptorPoolSize descriptorPoolSize[] = {
            {
                    .descriptorCount = bufferCount,
                    .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
            },
            {
                    .descriptorCount = bufferCount,
                    .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
            },
    };
    VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            .maxSets = bufferCount,
            .poolSizeCount = sizeof(descriptorPoolSize) / sizeof(VkDescriptorPoolSize),
            .pPoolSizes = descriptorPoolSize
    };
    VkResult result = vkCreateDescriptorPool(_renderDevicePtr->vkLogicDevice, &descriptorPoolCreateInfo, nullptr, &vkDescriptorPool);
    htCheckVKOp(result, "VK Descriptor Pool create fail.");
    if (result == VK_SUCCESS) {
        std::cout << "VK Descriptor Pool create success." << std::endl;
    }

    std::vector<VkDescriptorSetLayout> setLayouts(bufferCount, _renderPiplinePtr->vkDescriptorSetLayout);
    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .pSetLayouts = setLayouts.data(),
            .descriptorPool = vkDescriptorPool,
            .descriptorSetCount = static_cast<uint32_t>(vkDescriptorSets.size())
    };
    result = vkAllocateDescriptorSets(_renderDevicePtr->vkLogicDevice, &descriptorSetAllocateInfo, vkDescriptorSets.data());

    htCheckVKOp(result, "VK Descriptor Sets create fail.");
    if (result == VK_SUCCESS) {
        std::cout << "VK Descriptor Sets create success." << std::endl;
    }

    // Bind Uniform Buffer & Set
    for (int i = 0; i < bufferCount; ++i) {
        HTUniformBufferPtr uniformBufferPtr = uniformBuffers[i];
        VkDescriptorSet descriptorSet = vkDescriptorSets[i];

        VkDescriptorBufferInfo descriptorBufferInfo = {
                .offset = VkDeviceSize(0),
                .range = VkDeviceSize(uniformBufferPtr->size()),
                .buffer = uniformBufferPtr->vkBuffer
        };

        VkWriteDescriptorSet uniformBufferWriteDescriptorSet = {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .descriptorCount = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .dstBinding = 0,
                .dstArrayElement = 0,
                .dstSet = descriptorSet,
                .pBufferInfo = &descriptorBufferInfo
        };

        VkWriteDescriptorSet writeDescriptorSets[] = {
            uniformBufferWriteDescriptorSet
        };
        vkUpdateDescriptorSets(_renderDevicePtr->vkLogicDevice, sizeof(writeDescriptorSets) / sizeof(VkWriteDescriptorSet), writeDescriptorSets, 0, nullptr);
        UpdateTextures();
    }
}
