# C++ Vulkan Engine

⚠️ Status: Archived ⚠️ This project was created for educational purposes and is no longer maintained.

A small vulkan rendering engine written in C++.

## Demo

![Demo](Source\Resources\demo.gif)

## Features

- multi-platform: Windows and Linux.
  - [window creation](Source\Engine\Window\Window.cpp)
  - [thread](Source\Engine\Types\Platforms\Thread.cpp), [mutex](Source\Engine\Types\Platforms\Mutex.cpp) and [semaphore](Source\Engine\Types\Platforms\Semaphore.cpp)
  - [reading and writing files](Source\Engine\Types\Platforms\FileSystem.cpp)
- [virtual file system](Source\Engine\Types\IO\VirtualFileSystem.cpp)
- [custom file format](ThirdParty/SadFileFormat/SadFileFormat.h) '.sad' for [3d models](Source\Engine\Types\IO\Load.cpp#L52)
- [vulkan rendering](Source\Engine\VulkanRI\VulkanRI.cpp)
- custom stl: [Array](Source\Engine\Types\Core\Array.h), [String](Source\Engine\Types\Core\String.h) (utf-8 with conversion to utf-16 and utf-32), [Map](Source\Engine\Types\Core\Map.h)
- math library
  - [Vec2](Source\Engine\Types\Math\Vector.h#L102), [Vec3](Source\Engine\Types\Math\Vector.h#L11), [Vec4](Source\Engine\Types\Math\Vector.h#L184), [Quat](Source\Engine\Types\Math\Quaternion.h#L9) (Quaternion), [Mat4](Source\Engine\Types\Math\Mat4.h#L13)
  - perspective projection matrices: [near/far](Source\Engine\Types\Math\ExtraMath.h#L99), [infinite far plane](Source\Engine\Types\Math\ExtraMath.h#L133), [reversed z](Source\Engine\Types\Math\ExtraMath.h#L147)
  - operator overloading

## Building

### Dependencies

- [Vulkan-SDK](https://vulkan.lunarg.com/sdk/home)
- [XCB](https://xcb.freedesktop.org) (Linux Only): Is already installed on most linux systems
- [Windows SDK](https://developer.microsoft.com/de-de/windows/downloads/windows-10-sdk/) (Windows Only): Is part of Visual Studio

### Linux

1. run `bash ./build.sh editor debug linux -start`

### Windows

1. run `build.bat editor debug windows -start`
2. If something fails try opening `premake5.lua`, go to project `VulkanRI` and set the `includedirs` and `libdirs` vulkan paths to your Vulkan SDK paths. Open `build.bat` and set the `VULKAN_SDK_PATH` to be your vulkan sdk path

## License
[MIT](./LICENSE)
