#include "Callbacks.h"
#include "Interfaces/RenderInterface.h"
#include "Window/Window.h"

namespace sge {

	void onWindowResized()
	{
		as<IRenderInterface*>(getWindowUserPointer())->onWindowResized();
	}
}