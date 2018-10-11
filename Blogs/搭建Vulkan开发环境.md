# 初识Vulkan
Vulkan是[Khronos group](https://www.khronos.org/)制定的适用于现代GPU的一套图形处理API接口，OpenGL正是出自于这个组织。Vulkan相对于OpenGL的优势在于对现代GPU的特性提供的更好的支持，并且仅仅在GPU之上提供了一层很薄的封装，这有助于开发者开发出效率更高的代码。不过于此同时，开发者也需要自行处理更多的底层操作，并且在某些地方丧失一些灵活性。

# 下载并安装VulkanSDK
本文将搭建macOS和iOS下的Vulkan开发环境，在开始正式搭建Vulkan开发环境之前，我们需要下载Vulkan的SDK，这是[下载地址](https://vulkan.lunarg.com/sdk/home)，下载Mac下的SDK，这个SDK可以在MacOSX和iOS上使用。下载后解压，可以看到`Applications`，`Documentation`，`macOS`，`MoltenVK`几个目录。我们在`/usr/local`下创建`vulkansdk`目录，将前面4个目录拷贝到`/usr/local/vulkansdk`下备用。最后我稍微介绍下这4个目录分别是什么。

* `Applications`，包含用于演示的Vulkan示例
* `Documentation`，包含Vulkan的相关文档
* `macOS`，包含Vulkan在macOS上需要的静态库，动态库，配置等等
* `MoltenVK`，可能苹果想要推行自己的Metal图形渲染框架，所以并没有实现Vulkan的相关驱动，MoltenVK正是基于Metal为Vulkan实现的驱动。`MoltenVK`里包含了iOS和macOS需要的MoltenVK相关的静态库和动态库。

既然`MoltenVK`包含了iOS和macOS的Vulkan驱动，那么`macOS`里面的代码又有何用呢？`macOS`里面主要包含Vulkan Loader和各种Layer，为macOS上的Vulkan开发增加了各种Layer的支持，比如错误校验，日志输出等等。目前iOS上是无法使用这些功能的。关于Layer和Vulkan Loader会在后面详细介绍。

> MoltenVK一开始是收费的商业软件，由The Brenwill Workshop 开发。后来Valve Corporation使用了MoltenVK对macOS下的Dota2进行了底层优化，最终在Valve Corporation的协商下，The Brenwill Workshop开源了MoltenVK。

# 搭建Vulkan开发环境
Vulkan是一个完全跨平台的图形处理库，所以我使用C++作为示例的开发语言，这样Vulkan相关的示例代码可以方便的移植到Linux和Windows平台。下面是示例代码的基础目录结构。

- Classes （存放平台无关的示例代码）
- Platforms （存放平台相关的项目以及代码）
  * macOS （存放macOS的项目）
  * iOS （存放iOS的项目）

如果后面需要在Linux下使用，可以在Platforms里面增加Linux目录。

## MacOS
在MacOS上，我将使用动态库进行环境的搭建。首先我们使用Xcode建立一个命令行项目，选择语言为C++
![](https://upload-images.jianshu.io/upload_images/2949750-9e2df9dc4eb127c9.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

将用到的动态库和资源文件拷贝到项目下并添加到项目中。需要拷贝的有`/usr/local/vulkansdk/macOS/lib/libvulkan.1.1.82.dylib`，`/usr/local/vulkansdk/macOS/lib/libMoltenVK.dylib`，`/usr/local/vulkansdk/macOS/etc/vulkan/icd.d/MoltenVK_icd.json`。最后项目目录如下。

- Frameworks
  * libMoltenVK.dylib
  * libvulkan.1.dylib （由libvulkan.1.1.82.dylib重命名为libvulkan.1.dylib）
- Resources
  * MoltenVK_icd.json
- main.cpp

然后我们需要将动态库和资源设置到`Build Phase`的`Copy File`中，设置如下图。
![image.png](https://upload-images.jianshu.io/upload_images/2949750-e0aeb09e9bae310e.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

这样设置后，Build时，动态库和资源会被拷贝到可执行文件同一目录下，可执行文件就可以找到它们了。接着需要修改一下`MoltenVK_icd.json`，它指定了`libMoltenVK.dylib`所在的位置。修改前，它的内容是这样的
```json
{
    "file_format_version" : "1.0.0",
    "ICD": {
        "library_path": "../../../lib/libMoltenVK.dylib",
        "api_version" : "1.0.0"
    }
}
```
需要修改成
```json
{
    "file_format_version" : "1.0.0",
    "ICD": {
        "library_path": "./libMoltenVK.dylib",
        "api_version" : "1.0.0"
    }
}
```
因为我们的项目里，MoltenVK_icd.json和libMoltenVK.dylib在同一目录下。最后我们添加额外的头文件搜索路径：`/usr/local/vulkansdk/macOS/include`，这样我们就可以正确的导入Vulkan相关的头文件了。
![](https://upload-images.jianshu.io/upload_images/2949750-a705057b1b88d749.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)


MacOS的开发环境到此基本设置完毕了。

## MacOS开发环境验证
为了验证Vulkan的开发环境是否没有问题，我们编写一些代码，创建一个Vulkan的Instance，看看有没有问题。在`Classes`目录中创建C++类`HTRenderDevice`，这个类后面会用来创建管理Vulkan的设备，目前仅仅用来创建Vulkan实例。下面是创建Vulkan实例的代码
```cpp
void HTRenderDevice::createInstance() {
    // VK Application Info
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Vulkan Tutorial";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo info = {};
    info.pApplicationInfo = &appInfo;
    VkResult result = vkCreateInstance(&info, NULL, &vkInstance);
    htCheckVKOp(result, "VK Instance create failed.");
    if (result == VK_SUCCESS) {
        std::cout << "VK Instance create success." << std::endl;
    }
}
```
当然，你想用Objective-C编写`HTRenderDevice`类也是完全可以的，正如前面提到，使用C++仅仅是为了跨平台。最后在MacOS的项目中通过Group的方式包含`Classes`目录，并在main.cpp中初始化一个`HTRenderDevice`类。如果一切正常，示例程序输出里将会看到`VK Instance create success.`。如果你遇到了抛出异常，那你就要检查一下前面的步骤是否有遗漏或者错误了。

## iOS
在iOS上我将使用静态库的方式集成Vulkan，由于iOS上还没有提供Vulkan Loader，所以我们只需要集成MoltenVK即可，不过需要注意的是，SDK中给的静态库只提供了arm64架构的，模拟器下是无法编译通过的。目前我还没有找出在模拟器上调试的方案，只能在真机上调试。

新建一个单页面的iOS项目，语言使用Objective-C，然后把MoltenVK的framework拷贝到iOS项目目录下，framework位于`/usr/local/vulkansdk/MoltenVK/iOS/MoltenVK.framework`，我们把MoltenVK.framework加入项目，Xcode便会自动链接这个framework。接下来将上面用到的`Classes`文件夹加入项目，在`ViewController.m`的`- (void)viewDidLoad `方法中加入`HTRenderDevice renderDevice;`创建Vulkan Instance。别忘了把`ViewController.m`重命名为`ViewController.mm`，这样才能在oc类中使用C++。运行项目，如果一切正常的话，可以看到输出`VK Instance create success.`，当然别忘了只能在真机上调试。

# 示例地址
[https://github.com/SquarePants1991/VulkanTutorial](https://github.com/SquarePants1991/VulkanTutorial)，分支为`chapter1`。