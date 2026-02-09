#pragma once
#include "Mat4.h"
#include "Vector.h"
#include "Quaternion.h"

namespace sge {

	/*
	Mat 4
	*/

	template<typename T>
	inline Vec3_T<T> Mat4_T<T>::operator*(const Vec3_T<T>& a) const
	{
		return Vec3_T<T>{
			m[0] * a.x + m[1] * a.y + m[2] * a.z + m[3],
				m[4] * a.x + m[5] * a.y + m[6] * a.z + m[7],
				m[8] * a.x + m[9] * a.y + m[10] * a.z + m[11],
		};
	}

	template<typename T>
	inline Vec4_T<T> Mat4_T<T>::operator*(const Vec4_T<T>& a) const
	{
		return Vec4_T<T>{
			m[0] * a.x + m[1] * a.y + m[2] * a.z + m[3] * a.w,
				m[4] * a.x + m[5] * a.y + m[6] * a.z + m[7] * a.w,
				m[8] * a.x + m[9] * a.y + m[10] * a.z + m[11] * a.w,
				m[12] * a.x + m[13] * a.y + m[14] * a.z + m[15] * a.w
		};
	}

	template<typename T>
	inline Mat4_T<T>& Mat4_T<T>::scale(const Vec3_T<T>& a)
	{
		*this *= scaling(a);
		return *this;
	}
	template<typename T>
	inline Mat4_T<T> Mat4_T<T>::scaled(const Vec3_T<T>& a) const
	{
		return *this * scaling(a);
	}

	template<typename T>
	inline Mat4_T<T>& Mat4_T<T>::rotate(const Quat_T<T>& a)
	{
		*this *= rotation(a);
		return *this;
	}

	template<typename T>
	inline Mat4_T<T> Mat4_T<T>::rotated(const Quat_T<T>& a) const
	{
		return *this * rotation(a);
	}

	template<typename T>
	inline Mat4_T<T>& Mat4_T<T>::translate(const Vec3_T<T>& a)
	{
		*this *= translation(a);
		return *this;
	}

	template<typename T>
	inline Mat4_T<T> Mat4_T<T>::translated(const Vec3_T<T>& a) const
	{
		return *this * translation(a);
	}

	template<typename T>
	inline Mat4_T<T> Mat4_T<T>::scaling(const Vec3_T<T>& a)
	{
		return Mat4_T(a.x, 0, 0, 0, 0, a.y, 0, 0, 0, 0, a.z, 0, 0, 0, 0, 1);
	}

	template<typename T>
	inline Mat4_T<T> Mat4_T<T>::rotation(const Quat_T<T>& a)
	{
		return Mat4_T(1 - 2 * a.y * a.y - 2 * a.z * a.z, 2 * a.x * a.y - 2 * a.w * a.z, 2 * a.x * a.z + 2 * a.w * a.y, 0,
			2 * a.x * a.y + 2 * a.w * a.z, 1 - 2 * a.x * a.x - 2 * a.z * a.z, 2 * a.y * a.z + 2 * a.w * a.x, 0,
			2 * a.x * a.z - 2 * a.w * a.y, 2 * a.y * a.z - 2 * a.w * a.x, 1 - 2 * a.x * a.x - 2 * a.y * a.y, 0,
			0, 0, 0, 1);
	}

	template<typename T>
	inline Mat4_T<T> Mat4_T<T>::translation(const Vec3_T<T>& a)
	{
		return Mat4_T(1, 0, 0, a.x, 0, 1, 0, a.y, 0, 0, 1, a.z, 0, 0, 0, 1);
	}

	template<typename T>
	inline Mat4_T<T> Mat4_T<T>::transformation(const Vec3_T<T>& t, const Quat_T<T>& r, const Vec3_T<T>& s)
	{
		return translation(t).rotate(r).scale(s);
	}

	template<typename T>
	inline Mat4_T<f32> Mat4_T<T>::perspective(const f32 _fov, const Vec2_T<f32>& res, const f32 near_z, const f32 far_z)
	{
		f32 ar = res.x / res.y;
		f32 fov = tand((_fov / 2));

		f32 a = (far_z / (near_z - far_z));
		f32 b = (far_z * near_z / (near_z - far_z));

		return Mat4_T<f32>(
			1 / ar * fov, 0, 0, 0,
			0, 0, 1 / fov, 0,
			0, -a, 0, b,
			0, 1, 0, 0
			);
	}

	template<typename T>
	inline Mat4_T<f32> Mat4_T<T>::perspectiveRZ(const f32 _fov, const Vec2_T<f32>& res, const f32 near_z, const f32 far_z)
	{
		f32 ar = res.x / res.y;
		f32 fov = tand((_fov / 2));

		f32 a = (far_z / (near_z - far_z)) + 1;
		f32 b = (far_z * near_z / (near_z - far_z));

		return Mat4_T<f32>(
			1 / ar * fov, 0, 0, 0,
			0, 0, 1 / fov, 0,
			0, a, 0, -b,
			0, 1, 0, 0
			);
	}

	template<typename T>
	inline Mat4_T<f32> Mat4_T<T>::perspectiveI(const f32 _fov, const Vec2_T<f32>& res, const f32 near_z)
	{
		f32 ar = res.x / res.y;
		f32 fov = tand((_fov / 2));

		return Mat4_T<f32>(
			1 / ar * fov, 0, 0, 0,
			0, 0, 1 / fov, 0,
			0, 1, 0, -near_z,
			0, 1, 0, 0
			);
	}

	template<typename T>
	inline Mat4_T<f32> Mat4_T<T>::perspectiveIRZ(const f32 _fov, const Vec2_T<f32>& res, const f32 near_z)
	{
		f32 ar = res.x / res.y;
		f32 fov = tand(_fov / 2);

		return Mat4_T<f32>(
			1/ar*fov,0,0,0,
			0,0,1/fov,0,
			0,0,0,near_z,
			0,1,0,0);
	}

	/*
	Quaternion
	*/

	template<typename T>
	inline Quat_T<T> Quat_T<T>::quat(const Vec3_T<T>& axis, const T angle)
	{
		T cos, sin;
		cossind(cos, sin, angle * (T)0.5);

		Quat_T<T> q;
		q.x = axis.x * sin;
		q.y = axis.y * sin;
		q.z = axis.z * sin;
		q.w = cos;
		return q;
	}
}