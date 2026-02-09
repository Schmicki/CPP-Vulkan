#pragma once

#include "MathBase.h"

namespace sge {

	template<typename T> struct Mat4_T;
	template<typename T> struct Quat_T;

	template<typename T>
	struct Vec3_T {
		T x;
		T y;
		T z;

		Vec3_T operator + (const Vec3_T& a) const {
			return Vec3_T{ x + a.x, y + a.y, z + a.z };
		}

		Vec3_T& operator += (const Vec3_T& a) {
			x += a.x; y += a.y; z += a.z;
			return *this;
		}

		Vec3_T operator - (const Vec3_T& a) const {
			return { x - a.x, y - a.y, z - a.z };
		}

		Vec3_T operator - () const {
			return { -x, -y, -z };
		}

		Vec3_T operator -= (const Vec3_T& a) {
			x -= a.x; y -= a.y; z -= a.z;
			return *this;
		}


		Vec3_T operator * (const T a) const {
			return { x * a, y * a, z * a };
		}

		Vec3_T& operator *= (const T a) {
			x *= a; y *= a; z *= a;
			return *this;
		}

		Vec3_T operator / (const T a) const {
			return { x / a, y / a, z / a };
		}

		Vec3_T& operator /= (const T a) {
			x /= a; y /= a; z /= a;
			return *this;
		}

		bool operator == (const Vec3_T<T>& other) {
			return x == other.x && y == other.y && z == other.z;
		}

		bool operator != (const Vec3_T<T>& other) {
			return x != other.x || y != other.y || z != other.z;
		}

		T lenSq() const {
			return x * x + y * y + z * z;
		}

		static f32 length(const Vec3_T<f32>& a) {
			return sqrt(a.lenSq());
		}

		static f64 length(const Vec3_T<f64>& a) {
			return sqrt(a.lenSq());
		}

		Vec3_T normalized() const {
			T l = Vec3_T::length(*this);
			return { x / l, y / l, z / l };
		}

		Vec3_T& normalize() {
			T l = Vec3_T::length(*this);
			x /= l, y /= l, z /= l;
			return *this;
		}

		T dot(const Vec3_T& a) const {
			return x * a.x + a * a.y + z * a.z;
		}

		Vec3_T cross(const Vec3_T& a) const {
			return Vec3_T<T>{
				y* a.z - z * a.y,
					z* a.x - x * a.z,
					x* a.y - y * a.x
			};
		}
	};

	template<typename T>
	struct Vec2_T {
		T x;
		T y;

		Vec2_T operator + (const Vec2_T& a) const {
			return { x + a.x, y + y };
		}

		Vec2_T& operator += (const Vec2_T& a) {
			x += a.x; y += a.y;
			return *this;
		}

		Vec2_T operator - (const Vec2_T& a) const {
			return { x - a.x, y - a.y };
		}

		Vec2_T operator - () const {
			return { -x, -y };
		}

		Vec2_T operator -= (const Vec2_T& a) {
			x -= a.x; y -= a.y;
			return *this;
		}


		Vec2_T operator * (const T a) const {
			return { x * a, y * a };
		}

		Vec2_T& operator *= (const T a) {
			x *= a; y *= a;
			return *this;
		}

		Vec2_T operator / (const T a) const {
			return { x / a, y / a };
		}

		Vec2_T& operator /= (const T a) {
			x /= a; y /= a;
			return *this;
		}

		bool operator == (const Vec2_T<T>& other) {
			return x == other.x && y == other.y;
		}

		bool operator != (const Vec2_T<T>& other) {
			return x != other.x || y != other.y;
		}

		T lenSq() const {
			return x * x + y * y;
		}

		static f32 length(const Vec2_T<f32>& a) {
			return sqrt(a.lenSq());
		}

		static f64 length(const Vec2_T<f64>& a) {
			return sqrt(a.lenSq());
		}

		Vec2_T normalized() const {
			T l = Vec2_T::length(*this);
			return { x / l, y / l };
		}

		Vec2_T& normalize() {
			T l = Vec2_T::length(*this);
			x /= l, y /= l;
			return *this;
		}

		T dot(const Vec2_T& a) const {
			return x * a.x + a * a.y;
		}
	};

	template <typename T>
	struct Vec4_T {
		T x;
		T y;
		T z;
		T w;

		Vec4_T(const T _x, const T _y, const T _z, const T _w) {
			x = _x; y = _y; z = _z; w = _w;
		}
		Vec4_T(const Vec3_T<T>& a, const T _w) {
			x = a.x; y = a.y; z = a.z; w = _w;
		}
	};

	typedef Vec2_T<float_c> Vec2;
	typedef Vec2_T<f32> FVec2;
	typedef Vec2_T<f64> DVec2;

	typedef Vec3_T<float_c> Vec3;
	typedef Vec3_T<f32> FVec3;
	typedef Vec3_T<f64> DVec3;

	typedef Vec4_T<float_c> Vec4;
	typedef Vec4_T<f32> FVec4;
	typedef Vec4_T<f64> DVec4;
}