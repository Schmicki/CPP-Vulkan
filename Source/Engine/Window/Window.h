#pragma once

#include "Types/Types.h"

namespace sge
{

	void setWindowUserPointer(void* p);
	void* getWindowUserPointer();
	void setWindowResizedCallback(void(*callback)());

#if SGE_PLATFORM_WINDOWS

	using WindowHandle = void*;
	using AppInstance = void*;

#elif SGE_PLATFORM_LINUX

	using WindowHandle = unsigned int;
	using AppInstance = void*;

#else
	#error platform window not implemented!
#endif

	struct Window
	{
		WindowHandle handle;
		AppInstance appInstance;
	};

	WindowHandle createWindow(const String& WindowName, AppInstance appInstance, uint width, uint height);

	AppInstance getAppInstance();

	// Initialize vulkan engine window
	bool initializeWindow(Window& window, const String& title, uint width = 800, uint height = 600);

	// Windows show window
	bool showWindow(Window& window);

	// Windows message handling
	short updateWindow();

	// Windows destroy window
	void destroyWindow(Window& window);
}