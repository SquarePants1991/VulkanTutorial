//
//  ViewController.m
//  VulkanTutorialiOS
//
//  Created by ocean on 2018/10/9.
//  Copyright © 2018 handytool. All rights reserved.
//

#import "ViewController.h"
#import "HTVulkanInstance.hpp"
#import "HTRenderSurface.hpp"
#import "HTRenderDevice.hpp"
#import "HTSwapchain.hpp"
#import "HTRenderPass.hpp"
#import "HTRenderPipline.hpp"
#import "HTFrameBufferPool.hpp"
#import "HTCommandBufferPool.hpp"
#import "HTRenderer.hpp"
#import "HTVertexBuffer.hpp"
#import "HTUniformBufferPool.hpp"
#import <MetalKit/MetalKit.h>
#import <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#import <glm/gtx/transform.hpp>

@interface ViewController () {
    MTKView *_metalView;
    HTRendererPtr rendererPtr;
    HTSwapchainPtr swapchainPtr;
    HTVertexBufferPtr vertexBufferPtr;
    
    CADisplayLink *_renderLoopTimer;
    NSTimeInterval _lastUpdateTime;
    NSTimeInterval _elapsedTime;
}
@end

@implementation ViewController

static void render(VkCommandBuffer commandBuffer, HTUniformBufferPtr uniformBuffer, void *renderContext) {
    ViewController *vc = (__bridge ViewController *)renderContext;
    [vc render:commandBuffer uniformBuffer:uniformBuffer];
}

- (void)viewDidLoad {
    [super viewDidLoad];
    [self prepareMetalView];
    
    HTVulkanInstancePtr vulkanInstancePtr = HTNew(HTVulkanInstance);
    HTRenderSurfacePtr renderSurfacePtr = HTNew(HTRenderSurface, vulkanInstancePtr, (__bridge void *)_metalView);
    HTRenderDevicePtr renderDevicePtr = HTNew(HTRenderDevice, vulkanInstancePtr, renderSurfacePtr);
    swapchainPtr = HTNew(HTSwapchain, renderDevicePtr, renderSurfacePtr);
    HTRenderPassPtr renderPassPtr = HTNew(HTRenderPass, renderDevicePtr, swapchainPtr);
    
    NSString *vertexShaderPath = [[NSBundle mainBundle] pathForResource:@"vert" ofType:@"spv"];
    NSString *fragShaderPath = [[NSBundle mainBundle] pathForResource:@"frag" ofType:@"spv"];
    HTVertexInputDescriptions vertexInputDescriptions = {
        .attributeDescriptions = HTVertex::getAttributeDescriptions(),
        .bindingDescriptions = HTVertex::getBindingDescriptions()
    };
    HTRenderPiplinePtr renderPipelinePtr = HTNew(HTRenderPipline, renderDevicePtr, swapchainPtr, renderPassPtr, [vertexShaderPath UTF8String], [fragShaderPath UTF8String], vertexInputDescriptions);
    HTFrameBufferPoolPtr frameBufferPoolPtr = HTNew(HTFrameBufferPool, renderDevicePtr, swapchainPtr, renderPassPtr);
    HTCommandBufferPoolPtr commandBufferPoolPtr = HTNew(HTCommandBufferPool, renderDevicePtr, swapchainPtr);
    HTUniformBufferPoolPtr uniformBufferPoolPtr = HTNew(HTUniformBufferPool, renderDevicePtr, swapchainPtr, renderPipelinePtr);
    
    HTVertex vertices[] = {
        {{-0.5, 0.5, 0}, {1.0, 0.0, 1.0}},
        {{0.5, 0.5, 0}, {1.0, 0.0, 1.0}},
        {{0.5, -0.5, 0}, {0.0, 1.0, 1.0}},
        {{-0.5, -0.5, 0}, {1.0, 1.0, 0.0}},
    };
    
    uint16_t indexes[] = {
        0, 1, 2,
        2, 3, 0
    };
    vertexBufferPtr = HTNew(HTVertexBuffer, renderDevicePtr, vertices, sizeof(vertices) / sizeof(HTVertex), indexes, sizeof(indexes) / sizeof(uint16_t));
    
    rendererPtr = HTNew(HTRenderer, renderDevicePtr, swapchainPtr, renderPassPtr, renderPipelinePtr, frameBufferPoolPtr, commandBufferPoolPtr, uniformBufferPoolPtr);
    rendererPtr->renderHandler = render;
    rendererPtr->renderContext = (__bridge void *)self;
    rendererPtr->render();
    rendererPtr->present();
}

- (void)prepareMetalView {
    _metalView = [MTKView.alloc initWithFrame:self.view.bounds];
    [self.view addSubview:_metalView];
    
    _lastUpdateTime = -1;
    _renderLoopTimer = [CADisplayLink displayLinkWithTarget:self selector:@selector(renderLoop:)];
    [_renderLoopTimer addToRunLoop:[NSRunLoop mainRunLoop] forMode:NSDefaultRunLoopMode];
}

- (void)renderLoop:(CADisplayLink *)displayLink {
    NSTimeInterval now = [[NSDate date] timeIntervalSince1970];
    if (_lastUpdateTime < 0) {
        _lastUpdateTime = now;
        _elapsedTime = 0;
    }
    _elapsedTime += now - _lastUpdateTime;
    _lastUpdateTime = now;
    rendererPtr->render();
    rendererPtr->present();
}

- (void)render:(VkCommandBuffer)commandBuffer uniformBuffer:(HTUniformBufferPtr)uniformBufferPtr {
    // Update Uniform Data
    float screenW = float(swapchainPtr->imageExtend.width);
    float screenH = float(swapchainPtr->imageExtend.height);
    float animateFactor = (sin(_elapsedTime) + 1) * 0.5f;
    
#define USE_PERSPECTIVE 1
#if USE_PERSPECTIVE
    float aspect = screenW / screenH;
    uniformBufferPtr->uniformData.projection = glm::perspective(glm::radians(60.0f), aspect, 0.1f, 1000.0f);
    uniformBufferPtr->uniformData.view = glm::lookAt(glm::vec3(0, 0, -3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    uniformBufferPtr->uniformData.model = glm::rotate<float, glm::highp>(animateFactor, glm::vec3(1.0f, 0.0f, 0.0f));
#else
    float scale = 100.0f * animateFactor;
    uniformBufferPtr->uniformData.projection = glm::ortho(-screenW / 2.0f, screenW / 2.0f, -screenH / 2.0f, screenH / 2.0f, 0.1f, 1000.0f);
    uniformBufferPtr->uniformData.view = glm::lookAt(glm::vec3(0, 0, -1), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    uniformBufferPtr->uniformData.model = glm::scale<float, glm::highp>(glm::vec3(scale, scale, scale));
#endif
    uniformBufferPtr->flush();
    
    if (vertexBufferPtr) {
        VkBuffer buffers[] = {vertexBufferPtr->vkVertexBuffer};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
        if (vertexBufferPtr->supportIndexMode()) {
            vkCmdBindIndexBuffer(commandBuffer, vertexBufferPtr->vkIndexBuffer, VkDeviceSize(0), VK_INDEX_TYPE_UINT16);
            vkCmdDrawIndexed(commandBuffer, vertexBufferPtr->indexCount(), 1, 0, 0, 0);
        } else {
            vkCmdDraw(commandBuffer, vertexBufferPtr->vertexCount(), 1, 0, 0);
        }
    }
}
@end
