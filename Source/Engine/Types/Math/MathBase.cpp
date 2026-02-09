#include "MathBase.h"

namespace sge {

#ifdef SGE_PLATFORM_WINDOWS

	void cossind(f32& cos, f32& sin, f32 angle)
	{
		cos = cosd(angle);
		sin = std::sqrtf(1 - cos * cos);
		angle = fmod(angle, 360.0f);
		if ((angle < 0.0f && angle > -180.0f) || (angle > 180 && angle < 360))
			sin *= -1;
	}

#else

void cossind(f32& cos, f32& sin, f32 angle)
	{
		cos = cosd(angle);
		sin = std::sqrt(1 - cos * cos);
		angle = fmod(angle, 360.0f);
		if ((angle < 0.0f && angle > -180.0f) || (angle > 180 && angle < 360))
			sin *= -1;
	}

#endif
	void cossind(f64& cos, f64& sin, f64 angle)
	{
		cos = cosd(angle);
		sin = std::sqrt(1 - cos * cos);
		angle = fmod(angle, 360.0f);
		if ((angle < 0.0f && angle > -180.0f) || (angle > 180 && angle < 360))
			sin *= -1;
	}

	u32 log2(u32 a) {
		u32 i = 0;
		while (a % 2 == 0) i++, a /= 2;
		return i;
	}
}