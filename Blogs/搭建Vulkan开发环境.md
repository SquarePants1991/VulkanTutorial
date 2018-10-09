# 初识Vulkan
Vulkan是[Khronos group](https://www.khronos.org/)制定的适用于现代GPU的一套图形处理API接口，OpenGL正是出自于这个组织。Vulkan相对于OpenGL的优势在于对现代GPU的特性提供的更好的支持，并且仅仅在GPU之上提供了一层很薄的封装，这有助于开发者开发出效率更高的代码。不过于此同时，开发者也需要自行处理更多的底层操作，并且在某些地方丧失一些灵活性。

# 下载并安装VulkanSDK
本文将搭建macOS和iOS下的Vulkan开发环境，在开始正式搭建Vulkan开发环境之前，我们需要下载Vulkan的SDK，这是[下载地址](https://vulkan.lunarg.com/sdk/home)，下载Mac下的SDK，这个SDK可以在MacOSX和iOS上使用。下载后解压，可以看到`Applications`，`Documentation`，`macOS`，`MoltenVK`几个目录。我们在`/usr/local`下创建`vulkansdk`目录，将前面4个目录拷贝到`/usr/local/vulkansdk`下备用。最后我稍微介绍下这4个目录分别是什么。

* `Applications`，包含用于演示的Vulkan示例
* `Documentation`，包含Vulkan的相关文档
* `macOS`，

# 搭建Vulkan开发环境
Vulkan是一个完全跨平台的图形处理库，所以我使用C++作为示例的开发语言，这样Vulkan相关的示例代码可以方便的移植到Linux和Windows平台。下面是示例代码的基础目录结构。

- Classes （存放平台无关的示例代码）
- Platforms （存放平台相关的项目以及代码）
  * macOS （存放macOS的项目）
  * iOS （存放iOS的项目）

如果后面需要在Linux下使用，可以在Platforms里面增加Linux目录。

