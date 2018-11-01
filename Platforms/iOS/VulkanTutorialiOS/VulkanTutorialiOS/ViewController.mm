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
#import "HTFrameBufferPool.hpp"
#import "HTCommandBufferPool.hpp"
#import <MetalKit/MetalKit.h>

@interface ViewController () {
    MTKView *_metalView;
}
@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    [self prepareMetalView];
    
    HTVulkanInstancePtr vulkanInstancePtr = HTNew(HTVulkanInstance);
    HTRenderSurfacePtr renderSurfacePtr = HTNew(HTRenderSurface, vulkanInstancePtr, (__bridge void *)_metalView);
    HTRenderDevicePtr renderDevicePtr = HTNew(HTRenderDevice, vulkanInstancePtr, renderSurfacePtr);
    HTSwapchainPtr swapchainPtr = HTNew(HTSwapchain, renderDevicePtr, renderSurfacePtr);
    HTRenderPassPtr renderPassPtr = HTNew(HTRenderPass, renderDevicePtr, swapchainPtr);
    HTFrameBufferPoolPtr frameBufferPoolPtr = HTNew(HTFrameBufferPool, renderDevicePtr, swapchainPtr, renderPassPtr);
    HTCommandBufferPoolPtr commandBufferPoolPtr = HTNew(HTCommandBufferPool, renderDevicePtr, swapchainPtr);
}

- (void)prepareMetalView {
    _metalView = [MTKView.alloc initWithFrame:self.view.bounds];
    [self.view addSubview:_metalView];
}

@end
