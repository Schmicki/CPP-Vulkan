#include "Window.h"

#include <iostream>

#if SGE_PLATFORM_WINDOWS  // Windows only
#include <Windows.h>
#elif SGE_PLATFORM_LINUX // Linux only
#include <xcb/xcb.h>
#endif // SGE_PLATFORM

namespace sge
{

	void* windowUserPointer;
	void(*windowResizedCallback)();

	void setWindowUserPointer(void* p) { windowUserPointer = p; }

	void* getWindowUserPointer() { return windowUserPointer; }

	void setWindowResizedCallback(void(*callback)()) { windowResizedCallback = callback; }

#if SGE_PLATFORM_WINDOWS

	AppInstance getAppInstance() {
		return GetModuleHandle(NULL);
	}

	// Windows window procedure
	LRESULT windowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch (uMsg) {
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);

			// Painting
			FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
			EndPaint(hwnd, &ps);

		}
		break;

		case WM_SIZE:
			if(windowResizedCallback != nullptr)
				windowResizedCallback();
			break;

		case WM_CLOSE:
		{
			if (MessageBox(hwnd, L"Really quit?", L"Vulkan Engine", MB_OKCANCEL) == IDOK) {
				PostQuitMessage(0);
				DestroyWindow(hwnd);
			}
			return 0;
		}

		}
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}

	bool initializeWindow(Window& window, const String& title, uint width, uint height) {
		window.appInstance = getAppInstance();
		window.handle = createWindow(title, window.appInstance, width, height);

		if(!window.handle)
			return false;
		showWindow(window);
		return true;
	}

	WindowHandle sge::createWindow(const String& windowName, AppInstance appInstance, uint width, uint height) {

		wchar_t* _windowName = windowName.toWString();

		const LPCWSTR className = L"VulkanEngineWindow";

		WNDCLASS wc = {};
		wc.lpfnWndProc = windowProc;
		wc.hInstance = as<HINSTANCE>(appInstance);
		wc.lpszClassName = className;

		RegisterClass(&wc);

		HWND Ret = CreateWindowEx(
			/* dwExStyle */		0,
			/* ClassName */		className,
			/* Window Name */	_windowName,
			/* dwStyle */		WS_OVERLAPPEDWINDOW,
			/* Location X */	CW_USEDEFAULT,
			/* Location Y */	CW_USEDEFAULT,
			/* Width */			width,
			/* Height */		height,
			/* Parent */		NULL,
			/* hMenu */			NULL,
			/* hInstance */		as<HINSTANCE>(appInstance),
			/* lpParam */		NULL
		);

		return Ret;
	}

	// Windows message handling
	short updateWindow() {
		MSG msg = {};
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				return 0;
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
		return 1;
	}

	bool showWindow(Window& window) {
		return ShowWindow(as<HWND>(window.handle), SW_SHOW);
	}

	void destroyWindow(Window& window) {
		DestroyWindow(as<HWND>(window.handle));
	}

#elif SGE_PLATFORM_LINUX

	AppInstance gAppInstance = nullptr;
	xcb_intern_atom_reply_t* gReplyDestroyWindow = nullptr;

	xcb_window_t createWindow(const String& windowName, AppInstance appInstance, uint width, uint height) {
		const xcb_setup_t* setup = xcb_get_setup((xcb_connection_t*)appInstance);
		xcb_screen_iterator_t screenIterator = xcb_setup_roots_iterator(setup);
		xcb_screen_t* screen = screenIterator.data;
		
		uint mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
		uint values[2] = {
			screen->black_pixel,
			XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_KEY_RELEASE | XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE | XCB_EVENT_MASK_STRUCTURE_NOTIFY
		};

		xcb_window_t retWindow = xcb_generate_id((xcb_connection_t*)appInstance);

		xcb_create_window((xcb_connection_t*)appInstance, XCB_COPY_FROM_PARENT, retWindow, screen->root, 0, 0, width, height, 10, XCB_WINDOW_CLASS_INPUT_OUTPUT, screen->root_visual, mask, values);

		xcb_intern_atom_cookie_t cookieProtocols = xcb_intern_atom((xcb_connection_t*)appInstance, 1, 12, "WM_PROTOCOLS");
    	xcb_intern_atom_reply_t* replyProtocols = xcb_intern_atom_reply((xcb_connection_t*)appInstance, cookieProtocols, 0);

    	xcb_intern_atom_cookie_t cookieDestroyWindow = xcb_intern_atom((xcb_connection_t*)appInstance, 0, 16, "WM_DELETE_WINDOW");
    	gReplyDestroyWindow = xcb_intern_atom_reply((xcb_connection_t*)appInstance, cookieDestroyWindow, 0);

    	xcb_change_property((xcb_connection_t*)appInstance, XCB_PROP_MODE_REPLACE, retWindow, XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8, String(windowName).size, windowName);
    	xcb_change_property((xcb_connection_t*)appInstance, XCB_PROP_MODE_REPLACE, retWindow, replyProtocols->atom, 4, 32, 1, &gReplyDestroyWindow->atom);

		return retWindow;
	}

	AppInstance getAppInstance() {
		if(gAppInstance == nullptr) {
			gAppInstance = xcb_connect(nullptr, nullptr);
		}

		return gAppInstance;
	}

	bool initializeWindow(Window& window, const String& title, uint width, uint height) { 
		window.appInstance = getAppInstance();
		window.handle = createWindow(title, window.appInstance, width, height);

		if(!window.handle) {
			return false;
		}

		showWindow(window);
		return true;
	}

	bool showWindow(Window& window) { 
		xcb_map_window((xcb_connection_t*)window.appInstance, window.handle);
		xcb_flush((xcb_connection_t*)window.appInstance);

		return true; 
	}

	short updateWindow() {
		using namespace std;
		xcb_generic_event_t* event = nullptr;
		while(event = xcb_poll_for_event((xcb_connection_t*)gAppInstance))
		{
			switch(event->response_type & 127)
			{
                case XCB_CLIENT_MESSAGE:
				{
                    xcb_client_message_event_t* e = as<xcb_client_message_event_t*>(event);

                    if (e->data.data32[0] == gReplyDestroyWindow->atom)
					{
						Window window{e->window, gAppInstance};
						destroyWindow(window);

						return 0;
                    }
                }
				break;
				case XCB_CONFIGURE_NOTIFY:
				{
					xcb_configure_notify_event_t* e = (xcb_configure_notify_event_t*)event;

					static u16 width = e->width;
					static u16 height = e->height;

					if (width != e->width || height != e->height)
					{
						if(windowResizedCallback != NULL)
						{
							windowResizedCallback();
						}
						width = e->width;
						height = e->height;
					}
				}
				break;
			}
		}

		return 1;
	}

	void destroyWindow(Window& window) {
		xcb_destroy_window((xcb_connection_t*)window.appInstance, window.handle);
		xcb_flush((xcb_connection_t*)window.appInstance);
	}

#else

	AppInstance getAppInstance() {
		return 0;
	}

	bool initializeWindow(Window& window, const char* title, int width, int height) { return false; }

	bool showWindow(Window& window) { return false; }

	short updateWindow() { return 0; }

	void destroyWindow(Window& window) {}

#endif // SGE_PLATFORM_?
}