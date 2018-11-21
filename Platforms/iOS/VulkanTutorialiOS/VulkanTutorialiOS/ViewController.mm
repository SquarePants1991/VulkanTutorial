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
#import <MetalKit/MetalKit.h>

@interface ViewController () {
    MTKView *_metalView;
    HTRendererPtr rendererPtr;
    HTVertexBufferPtr vertexBufferPtr;
}
@end

@implementation ViewController

static void render(VkCommandBuffer commandBuffer, void *renderContext) {
    ViewController *vc = (__bridge ViewController *)renderContext;
    [vc render:commandBuffer];
}

- (void)viewDidLoad {
    [super viewDidLoad];
    [self prepareMetalView];
    
    HTVulkanInstancePtr vulkanInstancePtr = HTNew(HTVulkanInstance);
    HTRenderSurfacePtr renderSurfacePtr = HTNew(HTRenderSurface, vulkanInstancePtr, (__bridge void *)_metalView);
    HTRenderDevicePtr renderDevicePtr = HTNew(HTRenderDevice, vulkanInstancePtr, renderSurfacePtr);
    HTSwapchainPtr swapchainPtr = HTNew(HTSwapchain, renderDevicePtr, renderSurfacePtr);
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
    
    rendererPtr = HTNew(HTRenderer, renderDevicePtr, swapchainPtr, renderPassPtr, renderPipelinePtr, frameBufferPoolPtr, commandBufferPoolPtr);
    rendererPtr->renderHandler = render;
    rendererPtr->renderContext = (__bridge void *)self;
    rendererPtr->render();
    rendererPtr->present();
}

- (void)prepareMetalView {
    _metalView = [MTKView.alloc initWithFrame:self.view.bounds];
    [self.view addSubview:_metalView];
}

- (void)render:(VkCommandBuffer)commandBuffer {
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
