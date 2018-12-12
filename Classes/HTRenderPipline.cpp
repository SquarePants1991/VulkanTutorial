//
// Created by ocean on 2018/11/8.
// Copyright (c) 2018 handytool. All rights reserved.
//

#include <fstream>
#include "HTRenderPipline.hpp"
#include "HTVKCheckUtil.hpp"

HTRenderPipline::HTRenderPipline(HTRenderDevicePtr renderDevicePtr,
        HTSwapchainPtr swapchainPtr,
        HTRenderPassPtr renderPassPtr,
        const char *vertexShaderPath,
        const char *fragmentShaderPath,
        HTVertexInputDescriptions vertexInputDescriptions):
    _renderDevicePtr(renderDevicePtr),
    _swapchainPtr(swapchainPtr),
    _renderPassPtr(renderPassPtr),
    _vertexInputDescriptions(vertexInputDescriptions) {
    std::vector<char> vertexShaderByteCodes = readFileContent(vertexShaderPath);
    std::vector<char> fragmentShaderByteCodes = readFileContent(fragmentShaderPath);
    _shaderModules.push_back(std::make_pair(createShaderModule(vertexShaderByteCodes), VK_SHADER_STAGE_VERTEX_BIT));
    _shaderModules.push_back(std::make_pair(createShaderModule(fragmentShaderByteCodes), VK_SHADER_STAGE_FRAGMENT_BIT));
    createPipline();
}

HTRenderPipline::~HTRenderPipline() {
    for (const auto &shaderModule : _shaderModules) {
        vkDestroyShaderModule(_renderDevicePtr->vkLogicDevice, shaderModule.first, nullptr);
    }
    if (vkPiplineLayout) {
        vkDestroyPipelineLayout(_renderDevicePtr->vkLogicDevice, vkPiplineLayout, nullptr);
    }
    if (vkDescriptorSetLayout) {
        vkDestroyDescriptorSetLayout(_renderDevicePtr->vkLogicDevice, vkDescriptorSetLayout, nullptr);
    }
}

VkShaderModule HTRenderPipline::createShaderModule(std::vector<char> shaderByteCodes) {
    VkShaderModule vkShaderModule;
    VkShaderModuleCreateInfo shaderModuleCreateInfo = {};
    shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shaderModuleCreateInfo.codeSize = shaderByteCodes.size();
    shaderModuleCreateInfo.pCode = reinterpret_cast<uint32_t *>(shaderByteCodes.data());
    VkResult result = vkCreateShaderModule(_renderDevicePtr->vkLogicDevice, &shaderModuleCreateInfo, nullptr, &vkShaderModule);
    htCheckVKOp(result, "VK Shader Module create failed.");
    if (result == VK_SUCCESS) {
        std::cout << "VK Shader Module create success." << std::endl;
    }
    return vkShaderModule;
}

void HTRenderPipline::createPipline() {
    std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
    for (const auto &shaderModule: _shaderModules) {
        VkPipelineShaderStageCreateInfo shaderStageCreateInfo = {};
        shaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStageCreateInfo.module = shaderModule.first;
        shaderStageCreateInfo.stage = static_cast<VkShaderStageFlagBits>(shaderModule.second);
        shaderStageCreateInfo.pName = "main";
        shaderStages.push_back(shaderStageCreateInfo);
    }

    // 顶点数据布局，类似于VAO
    VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo = {};
    pipelineVertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    pipelineVertexInputStateCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(_vertexInputDescriptions.attributeDescriptions.size());;
    pipelineVertexInputStateCreateInfo.pVertexAttributeDescriptions = _vertexInputDescriptions.attributeDescriptions.data();
    pipelineVertexInputStateCreateInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(_vertexInputDescriptions.bindingDescriptions.size());
    pipelineVertexInputStateCreateInfo.pVertexBindingDescriptions = _vertexInputDescriptions.bindingDescriptions.data();

    VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo = {};
    pipelineInputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    pipelineInputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

    VkViewport viewport = {};
    viewport.x = 0;
    viewport.y = 0;
    viewport.width = _swapchainPtr->imageExtend.width;
    viewport.height = _swapchainPtr->imageExtend.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor = {};
    scissor.offset = {0, 0};
    scissor.extent = _swapchainPtr->imageExtend;

    VkPipelineViewportStateCreateInfo pipelineViewportStateCreateInfo = {};
    pipelineViewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    pipelineViewportStateCreateInfo.viewportCount = 1;
    pipelineViewportStateCreateInfo.pViewports = &viewport;
    pipelineViewportStateCreateInfo.scissorCount = 1;
    pipelineViewportStateCreateInfo.pScissors = &scissor;

    VkPipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
            .cullMode = VK_CULL_MODE_NONE,
            .depthBiasClamp = 0,
            .depthBiasConstantFactor = 0,
            .depthBiasEnable = VK_FALSE,
            .depthBiasSlopeFactor = 0,
            .depthClampEnable = VK_FALSE,
            .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
            .lineWidth = 1.0,
            .polygonMode = VK_POLYGON_MODE_FILL,
            .rasterizerDiscardEnable = VK_FALSE
    };

    VkPipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
            .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
            .alphaToCoverageEnable = VK_FALSE,
            .alphaToOneEnable = VK_FALSE,
            .minSampleShading = 1.0f,
            .pSampleMask = nullptr,
            .sampleShadingEnable = VK_FALSE
    };

    VkPipelineColorBlendAttachmentState colorBlendAttachmentState = {
            .blendEnable = VK_FALSE,
            .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
            .colorBlendOp = VK_BLEND_OP_ADD,
            .srcColorBlendFactor = VK_BLEND_FACTOR_ONE,
            .dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
            .alphaBlendOp = VK_BLEND_OP_ADD,
            .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
            .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
    };

    VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
            .pAttachments = &colorBlendAttachmentState,
            .attachmentCount = 1,
            .logicOpEnable = VK_FALSE,
            .logicOp = VK_LOGIC_OP_COPY,
            .blendConstants = {0.0f, 0.0f, 0.0f, 0.0f}
    };

    VkDynamicState dynamicStates[] = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_LINE_WIDTH};
    VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
            .dynamicStateCount = sizeof(dynamicStates) / sizeof(VkDynamicState),
            .pDynamicStates = dynamicStates,
    };

    VkDescriptorSetLayoutBinding uniformLayoutBinding = {
            .binding = 0,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .pImmutableSamplers = nullptr,
            .stageFlags = VK_SHADER_STAGE_VERTEX_BIT
    };
    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .bindingCount = 1,
            .pBindings = &uniformLayoutBinding,
    };

    htCheckVKOp(
            vkCreateDescriptorSetLayout(_renderDevicePtr->vkLogicDevice, &descriptorSetLayoutCreateInfo, nullptr, &vkDescriptorSetLayout),
            "VK Descriptor Set Layout create fail.");

    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .pushConstantRangeCount = 0,
            .pPushConstantRanges = nullptr,
            .pSetLayouts = &vkDescriptorSetLayout,
            .setLayoutCount = 1
    };
    htCheckVKOp(vkCreatePipelineLayout(_renderDevicePtr->vkLogicDevice, &pipelineLayoutCreateInfo, nullptr, &vkPiplineLayout), "VK Pipline Layout create fail.");

    VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
            .renderPass = _renderPassPtr->vkRenderPass,
            .subpass = 0,
            .layout = vkPiplineLayout,
            .basePipelineHandle = VK_NULL_HANDLE,
            .basePipelineIndex = -1,
            .pColorBlendState = &colorBlendStateCreateInfo,
            .pDynamicState = &dynamicStateCreateInfo,
            .pInputAssemblyState = &pipelineInputAssemblyStateCreateInfo,
            .pMultisampleState = &pipelineMultisampleStateCreateInfo,
            .pRasterizationState = &pipelineRasterizationStateCreateInfo,
            .pVertexInputState = &pipelineVertexInputStateCreateInfo,
            .pStages = shaderStages.data(),
            .stageCount = static_cast<uint32_t >(shaderStages.size()),
            .pViewportState = &pipelineViewportStateCreateInfo
    };

    VkResult result = vkCreateGraphicsPipelines(_renderDevicePtr->vkLogicDevice, VK_NULL_HANDLE, 1, &graphicsPipelineCreateInfo, nullptr, &vkPipeline);
    htCheckVKOp(result, "VK Pipeline create fail.");
    if (result == VK_SUCCESS) {
        std::cout << "VK Pipeline create success." << std::endl;
    }
}

std::vector<char> HTRenderPipline::readFileContent(const char *path) {
    std::vector<char> bytes;
    std::ifstream file;
    file.open(path, std::ios_base::binary | std::ios_base::ate);
    if (file.is_open()) {
        file.seekg(0, std::ios_base::end);
        unsigned long fileSize = static_cast<unsigned long >(file.tellg());
        bytes.resize(fileSize);
        file.seekg(0, std::ios_base::beg);
        file.read(bytes.data(), fileSize);
    }
    return bytes;
}

