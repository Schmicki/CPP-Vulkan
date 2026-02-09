#pragma once
#include "MathBase.h"

namespace sge {
	template<class T>
	struct Color_T {
		T r, g, b, a;
	};

	using Color = Color_T<u8>;
}