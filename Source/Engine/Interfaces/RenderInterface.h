#pragma once

namespace sge {

	struct Window;

	class IRenderInterface {
	public:
		virtual ~IRenderInterface() = 0;
		virtual bool initialize(sge::Window& window) = 0;
		virtual void drawFrame() = 0;
		virtual void onWindowResized() = 0;
		virtual void destroy() = 0;
	};
	inline IRenderInterface::~IRenderInterface() { }
}