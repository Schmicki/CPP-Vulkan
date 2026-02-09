#pragma once
#include "Interfaces/RenderInterface.h"
#include "Types/Aliases.h"

#ifdef SGE_PLATFORM_WINDOWS

#ifdef _VulkanRI // export
#define VKRENDERAPI _declspec(dllexport)
#else // import
#define VKRENDERAPI _declspec(dllimport)
#endif // _VulkanRI

#else

#define VKRENDERAPI

#endif

namespace sge {
class VirtualFileSystem;
}

// main app does not need to know vulkan sdk
VKRENDERAPI sge::IRenderInterface* createVulkanRenderInterface(sge::VirtualFileSystem* vfs);