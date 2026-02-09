#pragma once

#include "../Aliases.h"
#include <cmath>

namespace sge {
	namespace constants {


		const f32 pi_f = 3.141592f;
		const f64 pi_d = 3.141592653589793;

		const f32 degToRad_f = pi_f / 180.0f;
		const f64 degToRad_d = pi_d / 180.0;

#ifdef SGE_USE_DOUBLE
		const float_c pi_c = pi_d;
		const float_c degToRad_c = degToRad_d;
#else
		const float_c pi_c = pi_f;
		const float_c degToRad_c = degToRad_f;
#endif // SGE_USE_DOUBLE
	}

	inline f32 round(const f32 val);
	
	inline f32 trunc(const f32 val);
	
	inline f32 fmod(const f32 val, const f32 mod);


	inline f32 min(const f32 a, const f32 b);
	inline f64 min(const f64 a, const f64 b);

	inline f32 max(const f32 a, const f32 b);
	inline f64 max(const f64 a, const f64 b);

	u32 log2(u32 a);

	inline f32 degToRad(const f32 deg);
	inline f64 degToRad(const f64 deg);

	inline f32 radToDeg(const f32 rad);
	inline f64 radToDeg(const f64 rad);

	inline f32 cos(const f32 rad);
	inline f32 cosd(const f32 deg);

	inline f32 sin(const f32 rad);
	inline f64 sin(const f64 rad);

	inline f32 sind(const f32 deg);
	inline f64 sind(const f64 deg);

	inline f32 tan(const f32 rad);
	inline f64 tan(const f64 rad);

	inline f32 tand(const f32 deg);
	inline f64 tand(const f64 deg);

	inline f32 sqrt(const f32 n);
	inline f64 sqrt(const f64 n);

	inline f32 acos(const f32 cos);
	inline f64 acos(const f64 cos);

	inline f32 acosd(const f32 cos);
	inline f64 acosd(const f64 cos);

	inline f32 asin(const f32 sin);
	inline f64 asin(const f64 sin);

	inline f32 asind(const f32 sin);
	inline f64 asind(const f64 sin);

	inline f32 atan(const f32 tan);
	inline f64 atan(const f64 tan);

	inline f32 atand(const f32 tan);
	inline f64 atand(const f64 tan);

	void cossind(f32& cos, f32& sin, f32 angle);
	void cossind(f64& cos, f64& sin, f64 angle);

	/*
	* f32 operations
	*/

	inline f32 round(const f32 f) {
		return static_cast<f32>(static_cast<long>(f + 0.5f));
	}

	inline f32 trunc(const f32 f) {
		return static_cast<f32>(static_cast<long>(f));
	}

	inline f32 fmod(const f32 val, const f32 mod) {
		return val - trunc(val / mod) * mod;
	}


	inline f32 min(const f32 a, const f32 b) {
		return a < b ? a : b;
	}

	inline f32 max(const f32 a, const f32 b) {
		return a > b ? a : b;
	}


	inline f32 degToRad(const f32 deg) {
		return deg * constants::degToRad_f;
	}

	inline f32 radToDeg(const f32 rad) {
		return rad / constants::pi_f * 180;
	}

#ifdef SGE_PLATFORM_WINDOWS

	inline f32 cos(const f32 rad) {
		return std::cosf(rad);
	}
	inline f32 cosd(const f32 deg) {
		return std::cosf(degToRad(deg));
	}
	inline f32 sin(const f32 rad) {
		return std::sinf(rad);
	}
	inline f32 sind(const f32 deg) {
		return std::sinf(degToRad(deg));
	}
	inline f32 tan(const f32 rad) {
		return std::tanf(rad);
	}
	inline f32 tand(const f32 deg) {
		return std::tanf(degToRad(deg));
	}
	inline f32 sqrt(const f32 n) {
		return std::sqrtf(n);
	}

	inline f32 acos(const f32 cos) {
		return std::acosf(cos);
	}
	inline f32 acosd(const f32 cos) {
		return radToDeg(std::acosf(cos));
	}
	inline f32 asin(const f32 sin) {
		return std::asinf(sin);
	}
	inline f32 asind(const f32 sin) {
		return radToDeg(std::asinf(sin));
	}

	inline f32 atan(const f32 tan) {
		return std::atanf(tan);
	}
	inline f32 atand(const f32 tan) {
		return radToDeg(std::atanf(tan));
	}

#else

inline f32 cos(const f32 rad) {
		return std::cos(rad);
	}
	inline f32 cosd(const f32 deg) {
		return std::cos(degToRad(deg));
	}
	inline f32 sin(const f32 rad) {
		return std::sin(rad);
	}
	inline f32 sind(const f32 deg) {
		return std::sin(degToRad(deg));
	}
	inline f32 tan(const f32 rad) {
		return std::tan(rad);
	}
	inline f32 tand(const f32 deg) {
		return std::tan(degToRad(deg));
	}
	inline f32 sqrt(const f32 n) {
		return std::sqrt(n);
	}

	inline f32 acos(const f32 cos) {
		return std::acos(cos);
	}
	inline f32 acosd(const f32 cos) {
		return radToDeg(std::acos(cos));
	}
	inline f32 asin(const f32 sin) {
		return std::asin(sin);
	}
	inline f32 asind(const f32 sin) {
		return radToDeg(std::asin(sin));
	}

	inline f32 atan(const f32 tan) {
		return std::atan(tan);
	}
	inline f32 atand(const f32 tan) {
		return radToDeg(std::atan(tan));
	}

#endif

	/*
	* Double operations
	*/


	inline f64 min(const f64 a, const f64 b) {
		return a < b ? a : b;
	}

	inline f64 max(const f64 a, const f64 b) {
		return a > b ? a : b;
	}


	inline f64 degToRad(const f64 deg) {
		return deg * constants::degToRad_d;
	}

	inline f64 radToDeg(const f64 rad) {
		return rad / constants::pi_d * 180;
	}

	inline f64 cos(const f64 rad) {
		return std::cos(rad);
	}
	inline f64 cosd(const f64 deg) {
		return std::cos(degToRad(deg));
	}
	inline f64 sin(const f64 rad) {
		return std::sin(rad);
	}
	inline f64 sind(const f64 deg) {
		return std::sin(degToRad(deg));
	}
	inline f64 tan(const f64 rad) {
		return std::tan(rad);
	}
	inline f64 tand(const f64 deg) {
		return std::tan(degToRad(deg));
	}
	inline f64 sqrt(const f64 n) {
		return std::sqrt(n);
	}

	inline f64 acos(const f64 cos) {
		return std::acos(cos);
	}
	inline f64 acosd(const f64 cos) {
		return radToDeg(std::acos(cos));
	}
	inline f64 asin(const f64 sin) {
		return std::asin(sin);
	}
	inline f64 asind(const f64 sin) {
		return radToDeg(std::asin(sin));
	}
	inline f64 atan(const f64 tan) {
		return std::atan(tan);
	}
	inline f64 atand(const f64 tan) {
		return radToDeg(std::atan(tan));
	}

	/*
	* Integer operations
	*/

	inline u32 max(const u32 a, const u32 b) {
		return a > b ? a : b;
	}
}