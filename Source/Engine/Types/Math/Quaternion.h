#pragma once
#include "MathBase.h"

namespace sge {

	template<typename T> struct Vec3_T;

	template<typename T>
	struct Quat_T {
		T x, y, z, w;
		Quat_T() { x = 0; y = 0; z = 0; w = 0; };

		Quat_T(const T _x, const T _y, const T _z, const T _w) {
			x = _x; y = _y; z = _z; w = _w;
		}

		static Quat_T<T> quat(const T eulerX, const T eulerY, const T eulerZ) {
			T sx, sy, sz, cx, cy, cz;

			cossind(cx, sx, eulerX * 0.5f);
			cossind(cy, sy, eulerY * 0.5f);
			cossind(cz, sz, eulerZ * 0.5f);
			return Quat_T<T>{
				cx* sy* sz - sx * cy * cz,
					-cx * sy * cz - sx * cy * sz,
					cx* cy* sz - sx * sy * cz,
					cx* cy* cz + sx * sy * sz};
		}

		static Quat_T<T> quat(const Vec3_T<T>& axis, const T angle);

		Quat_T operator * (const Quat_T& q) {
			return {
			w * q.x + x * q.w + y * q.z - z * q.y,
			w * q.y - x * q.z + y * q.w + z * q.x,
			w * q.z + x * q.y - y * q.x + z * q.w,
			w * q.w - x * q.x - y * q.y - z * q.z };
		}

		Quat_T& operator *= (const Quat_T& q) {
			*this = {
			w * q.x + x * q.w + y * q.z - z * q.y,
			w * q.y - x * q.z + y * q.w + z * q.x,
			w * q.z + x * q.y - y * q.x + z * q.w,
			w * q.w - x * q.x - y * q.y - z * q.z };
			return *this;
		}

		Quat_T operator / (const T a) {
			return { x / a, y / a, z / a, w / a };
		}

		Quat_T& operator /= (const T a) {
			x /= a; y /= a; z /= a; w /= a;
			return *this;
		}

		T lenSq() const {
			return x * x + y * y + z * z + w * w;
		}

		static f32 length(Quat_T<f32>& q) {
			return sqrt(q.lenSq());
		}

		static f64 length(Quat_T<f64>& q) {
			return sqrt(q.lenSq());
		}

		Quat_T normalized() {
			T l = Quat_T::length(*this);
			return { x / l, y / l, z / l, w / l };
		}

		Quat_T& normalize() {
			T l = Quat_T::length(*this);
			x /= l, y /= l, z /= l, w /= l;
			return *this;
		}

		Quat_T negatedW() {
			return { x, y, z, -w };
		}
	};

	typedef Quat_T<float_c> Quat;
	typedef Quat_T<f32> FQuat;
	typedef Quat_T<f64> DQuat;
}