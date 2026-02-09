#include "VulkanRI_include.h"
#include "VulkanRI.h"
#include "Types/IO/VirtualFileSystem.h"

VKRENDERAPI sge::IRenderInterface* createVulkanRenderInterface(sge::VirtualFileSystem* vfs) {
	sge::VFS::init(vfs);
	return new sge::VulkanRenderInterface();
}