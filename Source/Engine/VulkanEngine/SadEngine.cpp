// Sad Engine entry point
#include "SadEngine.h"
#include "Types/IO/VirtualFileSystem.h"
#include "Window/Window.h"
#include "Core/Callbacks.h"
#include "VulkanRI/VulkanRI_include.h"

int main(int argc, char** args) {
	sge::VFS::init(NULL);

	sge::VFS::mount("/Shaders", "Resources/Shaders/Compiled");
	sge::VFS::mount("/Textures", "Resources/Textures");
	sge::VFS::mount("/Models", "Resources/Models");
	sge::VFS::mount("/Res", "Resources");
	
	bool g_bRunning = true;

	// create window
	sge::Window window;
	if (!sge::initializeWindow(window, "Vulkan Engine Window"))
		return 101;
	sge::setWindowResizedCallback(sge::onWindowResized);

	sge::IRenderInterface* renderInterface = createVulkanRenderInterface(sge::VFS::instance);
	if (!renderInterface->initialize(window)) return 100;
	sge::setWindowUserPointer(renderInterface);

	// main loop
	while (g_bRunning)
	{
		if (!sge::updateWindow())
		{
			g_bRunning = false;
		}
		if (g_bRunning) {
			renderInterface->drawFrame();
		}
	}

	// delete
	renderInterface->destroy();
	sge::VFS::destroy();
}