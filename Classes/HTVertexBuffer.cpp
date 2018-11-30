//
// Created by ocean on 2018/11/19.
// Copyright (c) 2018 handytool. All rights reserved.
//

#include "HTVertexBuffer.hpp"
#include "HTVKCheckUtil.hpp"

HTVertexBuffer::HTVertexBuffer(HTRenderDevicePtr renderDevicePtr, HTVertex *vertices, uint32_t vertexCount, uint16_t *indexes, uint32_t indexCount, bool shouldCopyData):
        HTBuffer(renderDevicePtr),
        _vertices(vertices),
        _vertexCount(vertexCount),
        _indexes(indexes),
        _indexCount(indexCount),
        _shouldCopyData(shouldCopyData) {
    if (_shouldCopyData) {
        if (vertices != nullptr) {
            _vertices = (HTVertex *)malloc(vertexCount * sizeof(HTVertex));
            memcpy(_vertices, vertices, vertexCount * sizeof(HTVertex));
        }
        if (indexes != nullptr) {
            _indexes = (uint16_t *) malloc(indexCount * sizeof(uint16_t));
            memcpy(_indexes, indexes, indexCount * sizeof(uint16_t));
        }
    }
    createBuffers();
}

HTVertexBuffer::~HTVertexBuffer() {
    if (_shouldCopyData && _vertices) {
        free(_vertices);
    }
    if (vkVertexBuffer) {
        vkDestroyBuffer(_renderDevicePtr->vkLogicDevice, vkVertexBuffer, nullptr);
    }
    if (vkVertexBufferDeviceMemory) {
        vkFreeMemory(_renderDevicePtr->vkLogicDevice, vkVertexBufferDeviceMemory, nullptr);
    }
    if (vkIndexBuffer) {
        vkDestroyBuffer(_renderDevicePtr->vkLogicDevice, vkIndexBuffer, nullptr);
    }
    if (vkIndexBufferDeviceMemory) {
        vkFreeMemory(_renderDevicePtr->vkLogicDevice, vkIndexBufferDeviceMemory, nullptr);
    }
}

void HTVertexBuffer::createBuffers() {
    uint32_t vertexBufferSize = sizeof(HTVertex) * _vertexCount;
    createBuffer(vertexBufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, vkVertexBuffer, vkVertexBufferDeviceMemory);
    copyData(vertexBufferSize, reinterpret_cast<void *>(_vertices), vkVertexBufferDeviceMemory);
    uint32_t indexBufferSize = sizeof(uint16_t) * _indexCount;
    createBuffer(indexBufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, vkIndexBuffer, vkIndexBufferDeviceMemory);
    copyData(indexBufferSize, reinterpret_cast<void *>(_indexes), vkIndexBufferDeviceMemory);
}