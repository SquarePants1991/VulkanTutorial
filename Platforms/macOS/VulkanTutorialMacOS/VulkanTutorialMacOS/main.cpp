//
//  main.cpp
//  VulkanTutorialMacOS
//
//  Created by ocean on 2018/10/8.
//  Copyright © 2018 handytool. All rights reserved.
//

#include <iostream>
#include "HTRenderDevice.hpp"

int main(int argc, const char * argv[]) {
    HTRenderDevice renderDevice;
    std::cout << renderDevice.vkInstance;
}
