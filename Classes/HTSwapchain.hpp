//
//  HTSwapchain.hpp
//  VulkanTutorialMacOS
//
//  Created by ocean on 2018/10/23.
//  Copyright © 2018 handytool. All rights reserved.
//

#ifndef HTSwapchain_hpp
#define HTSwapchain_hpp

#include "HTRenderDevice.hpp"
#include "HTRenderSurface.hpp"
#include <vulkan/vulkan.h>
#include <vector>

class HTSwapchain {
private:
    HTRenderDevicePtr _renderDevicePtr;
    HTRenderSurfacePtr _renderSurfacePtr;

    VkSurfaceCapabilitiesKHR _surfaceCapabilities;
    VkSurfaceFormatKHR _surfaceFormat;
    VkPresentModeKHR _presentMode;

    void collectSwapchainRelateInfos();
    void createSwapchain();
    void getImagesOnChain();
public:
    VkSwapchainKHR vkSwapchain;
    std::vector<VkImage> imagesOnChain;
    std::vector<VkImageView> imageViews;
    
    VkFormat imageFormat;
    VkExtent2D imageExtend;

    HTSwapchain(HTRenderDevicePtr renderDevicePtr, HTRenderSurfacePtr renderSurfacePtr);
    ~HTSwapchain();
};
HTMakeClass(HTSwapchain)
#endif /* HTSwapchain_hpp */
