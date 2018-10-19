//
//  HTMacros.h
//  VulkanTutorialiOS
//
//  Created by ocean on 2018/10/19.
//  Copyright © 2018 handytool. All rights reserved.
//

#ifndef HTMacros_h
#define HTMacros_h

#include <memory>
#define HTMakeClass(className) \
typedef std::shared_ptr<className> className##Ptr; \
typedef std::weak_ptr<className> className##WeakPtr;
#define HTNew(className, ...) std::make_shared<className>(__VA_ARGS__)

#endif /* HTMacros_h */
