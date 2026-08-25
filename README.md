# GP2 Vulkan Renderer

This project is a custom Vulkan framework created for the **Graphics Programming 2** course in the **Digital Arts & Entertainment (DAE)** curriculum.

It is designed to reduce the setup complexity of Vulkan while still exposing modern, explicit rendering concepts.  
The framework provides a structured rendering architecture and supports importing OBJ files and complete glTF scenes.

## Name & Repo
**Senne Goeminne**  
https://github.com/Howest-DAE-GD/gp2-vulkan-renderer-GoeminneSenne

## Rendering Architecture

The application implements a complete Deferred Rendering Pipeline with following stages:

1. Depth Prepass  
   Renders scene only to the depth buffer to reduce overdraw in later passes.

2. Geometry Pass 
   Writes material and pixel data (albedo, normals, metallic & roughness) into a G-Buffer.

3. Deferred Lighting Pass  
   Does a fullscreen triangle pass, samples from the G Buffer and calculates the final pixel value by doing necessary lighting calculations

4. Blitting Pass  
   Applies Reinhard tone mapping and factors in camera exposure to convert HDR output to a presentable LDR image.

## Additional Features

- Vulkan Memory Allocator (VMA)
- Synchronization 2
- Dynamic Rendering


Planned Features:
* Bindless Rendering feature (Modern Vulkan)
* Image Based Lighting
* Shadow Mapping for Directional Lights

