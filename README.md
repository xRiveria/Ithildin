# Ithildin
<img src="https://github.com/xRiveria/Ithildin/blob/master/Documentation/CornellBox.PNG" width="49%"></img> <img src="https://github.com/xRiveria/Ithildin/blob/master/Documentation/OneWeekend.PNG" width="49%"></img>

Built with heavy references to Peter Shirley's [Raytracing in One Weekend](https://raytracing.github.io) book series, Ithildin is my own take on the subject using [Vulkan's Raytracing Pipeline](https://docs.vulkan.org/guide/latest/extensions/ray_tracing.html#:~:text=The%20VK_KHR_ray_tracing_pipeline%20extension%20introduces%20ray,vertex%2Fgeometry%2Ffragment%20stages.) 
extension. Augumented with a simple control panel for the tweaking of various rendering parameters including sample count, ray bounces and camera behavior, a combination of techniques (acceleration structures, variable rate shading, etc.) were used to keep frame rates consistent for real-time simulations.

Without proper [denoising](https://blogs.nvidia.com/blog/what-is-denoising/) techniques, a good camera shake will cause visible noise that quickly filters out over time. That said, the application is able to sustain upwards of 160 FPS using an RTX 3080 with 12 rays per pixel and 16 bounces on a 1920x1080 window. While I'm somewhat satisfied with the current implementation, 
I hope to find time in the future to push performance further and follow through with the rest of Peter Shirley's remaining books.

## Compilation

There are no hardware prerequisites, although a somewhat modern GPU will help with frame rates nonetheless. 

To build the project, simply navigate to the `Scripts` folder and run `IthildinBuildWindows.bat`. This will leverage Premake and automatically generate a C++17 solution in the project's root directory.

## Performance

While the current implementation is already significantly faster than traditional CPU-based raytracing implementations (in part due to Vulkan), there are several areas which I believe can further improve performance outside of hardware limitations:
* Multithreading Support (Transforms, Commands)
* Geometry Instancing
* [Distinct Fast Build/Fast Trace Acceleration Structures](https://docs.vulkan.org/samples/latest/samples/extensions/ray_tracing_extended/README.html)
* Batching Objects into BLAS Instances

## References
* [Vulkan Tutorial](https://vulkan-tutorial.com/)
* [Introduction to Real-Time Ray Tracing with Vulkan](https://devblogs.nvidia.com/vulkan-raytracing)
* [NVIDIA Vulkan Ray Tracing Tutorial](https://developer.nvidia.com/rtx/raytracing/vkray)
* [Raytracing in Vulkan](https://github.com/GPSnoopy/RayTracingInVulkan)
* [Ray Tracing Validation for DirectX 12 and Vulkan](https://developer.nvidia.com/blog/ray-tracing-validation-at-the-driver-level)
* [Vulkan Ray Tracing Final Specification Release](https://www.khronos.org/blog/vulkan-ray-tracing-final-specification-release)
