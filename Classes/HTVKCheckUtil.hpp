#ifndef HTVKCheckUtil_hpp
#define HTVKCheckUtil_hpp

#include <vulkan/vulkan.h>

inline void htCheckVKOp(VkResult result, const char *errMsg) {
    if (result != VK_SUCCESS) {
        throw std::runtime_error(errMsg);
    }
}

#endif