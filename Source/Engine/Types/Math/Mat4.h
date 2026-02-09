#pragma once

#include "MathBase.h"

namespace sge {

	template<typename T> struct Quat_T;
	template<typename T> struct Vec2_T;
	template<typename T> struct Vec3_T;
	template<typename T> struct Vec4_T;

	template<typename T>
	struct Mat4_T {
		T m[16];

		Mat4_T() { *this = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; };

		Mat4_T(
			const T m00, const T m01, const T m02, const T m03,
			const T m10, const T m11, const T m12, const T m13,
			const T m20, const T m21, const T m22, const T m23,
			const T m30, const T m31, const T m32, const T m33
		) {
			m[00] = m00; m[01] = m01; m[02] = m02; m[03] = m03;
			m[04] = m10; m[05] = m11; m[06] = m12; m[07] = m13;
			m[8] = m20; m[9] = m21; m[10] = m22; m[11] = m23;
			m[12] = m30; m[13] = m31; m[14] = m32; m[15] = m33;
		}

		Mat4_T operator + (const Mat4_T& a) const {
			return {
				m[0] + a[0], m[1] + a.m[1], m[2] + a.m[2], m[3] + a.m[3],
				m[8] + a.m[8], m[9] + a.m[9], m[10] + a.m[10], m[11] + a.m[11],
				m[4] + a.m[4], m[5] + a.m[5], m[6] + a.m[6], m[7] + a.m[7],
				m[12] + a.m[12], m[13] + a.m[13], m[14] + a.m[14], m[15] + a.m[15],
			};
		}

		Mat4_T& operator += (const Mat4_T& a) {
			m[0] += a.m[0]; m[1] += a.m[1]; m[2] += a.m[2]; m[3] += a.m[3];
			m[4] += a.m[4]; m[5] += a.m[5]; m[6] += a.m[6]; m[7] += a.m[7];
			m[8] += a.m[8]; m[9] += a.m[9]; m[10] += a.m[10]; m[11] += a.m[11];
			m[12] += a.m[12]; m[13] += a.m[13]; m[14] += a.m[14]; m[15] += a.m[15];
			return *this;
		}

		Mat4_T operator - (const Mat4_T& a) const {
			return {
				m[0] - a[0], m[1] - a.m[1], m[2] - a.m[2], m[3] - a.m[3],
				m[8] - a.m[8], m[9] - a.m[9], m[10] - a.m[10], m[11] - a.m[11],
				m[4] - a.m[4], m[5] - a.m[5], m[6] - a.m[6], m[7] - a.m[7],
				m[12] - a.m[12], m[13] - a.m[13], m[14] - a.m[14], m[15] - a.m[15],
			};
		}

		Mat4_T& operator -= (const Mat4_T& a) {
			m[0] -= a.m[0]; m[1] -= a.m[1]; m[2] -= a.m[2]; m[3] -= a.m[3];
			m[4] -= a.m[4]; m[5] -= a.m[5]; m[6] -= a.m[6]; m[7] -= a.m[7];
			m[8] -= a.m[8]; m[9] -= a.m[9]; m[10] -= a.m[10]; m[11] -= a.m[11];
			m[12] -= a.m[12]; m[13] -= a.m[13]; m[14] -= a.m[14]; m[15] -= a.m[15];

			return *this;
		}

		Mat4_T operator * (const Mat4_T& a) const {
			return{
				m[0] * a.m[0] + m[1] * a.m[4] + m[2] * a.m[8] + m[3] * a.m[12],
				m[0] * a.m[1] + m[1] * a.m[5] + m[2] * a.m[9] + m[3] * a.m[13],
				m[0] * a.m[2] + m[1] * a.m[6] + m[2] * a.m[10] + m[3] * a.m[14],
				m[0] * a.m[3] + m[1] * a.m[7] + m[2] * a.m[11] + m[3] * a.m[15],

				m[4] * a.m[0] + m[5] * a.m[4] + m[6] * a.m[8] + m[7] * a.m[12],
				m[4] * a.m[1] + m[5] * a.m[5] + m[6] * a.m[9] + m[7] * a.m[13],
				m[4] * a.m[2] + m[5] * a.m[6] + m[6] * a.m[10] + m[7] * a.m[14],
				m[4] * a.m[3] + m[5] * a.m[7] + m[6] * a.m[11] + m[7] * a.m[15],

				m[8] * a.m[0] + m[9] * a.m[4] + m[10] * a.m[8] + m[11] * a.m[12],
				m[8] * a.m[1] + m[9] * a.m[5] + m[10] * a.m[9] + m[11] * a.m[13],
				m[8] * a.m[2] + m[9] * a.m[6] + m[10] * a.m[10] + m[11] * a.m[14],
				m[8] * a.m[3] + m[9] * a.m[7] + m[10] * a.m[11] + m[11] * a.m[15],

				m[12] * a.m[0] + m[13] * a.m[4] + m[14] * a.m[8] + m[15] * a.m[12],
				m[12] * a.m[1] + m[13] * a.m[5] + m[14] * a.m[9] + m[15] * a.m[13],
				m[12] * a.m[2] + m[13] * a.m[6] + m[14] * a.m[10] + m[15] * a.m[14],
				m[12] * a.m[3] + m[13] * a.m[7] + m[14] * a.m[11] + m[15] * a.m[15],
			};
		}

		Mat4_T& operator *= (const Mat4_T& a) {
			T c0, c1, c2, c3;
			c0 = m[0] * a.m[0] + m[1] * a.m[4] + m[2] * a.m[8] + m[3] * a.m[12];
			c1 = m[0] * a.m[1] + m[1] * a.m[5] + m[2] * a.m[9] + m[3] * a.m[13];
			c2 = m[0] * a.m[2] + m[1] * a.m[6] + m[2] * a.m[10] + m[3] * a.m[14];
			c3 = m[0] * a.m[3] + m[1] * a.m[7] + m[2] * a.m[11] + m[3] * a.m[15];

			m[0] = c0; m[1] = c1; m[2] = c2; m[3] = c3;

			c0 = m[4] * a.m[0] + m[5] * a.m[4] + m[6] * a.m[8] + m[7] * a.m[12];
			c1 = m[4] * a.m[1] + m[5] * a.m[5] + m[6] * a.m[9] + m[7] * a.m[13];
			c2 = m[4] * a.m[2] + m[5] * a.m[6] + m[6] * a.m[10] + m[7] * a.m[14];
			c3 = m[4] * a.m[3] + m[5] * a.m[7] + m[6] * a.m[11] + m[7] * a.m[15];

			m[4] = c0; m[5] = c1; m[6] = c2; m[7] = c3;

			c0 = m[8] * a.m[0] + m[9] * a.m[4] + m[10] * a.m[8] + m[11] * a.m[12];
			c1 = m[8] * a.m[1] + m[9] * a.m[5] + m[10] * a.m[9] + m[11] * a.m[13];
			c2 = m[8] * a.m[2] + m[9] * a.m[6] + m[10] * a.m[10] + m[11] * a.m[14];
			c3 = m[8] * a.m[3] + m[9] * a.m[7] + m[10] * a.m[11] + m[11] * a.m[15];

			m[8] = c0; m[9] = c1; m[10] = c2; m[11] = c3;

			c0 = m[12] * a.m[0] + m[13] * a.m[4] + m[14] * a.m[8] + m[15] * a.m[12];
			c1 = m[12] * a.m[1] + m[13] * a.m[5] + m[14] * a.m[9] + m[15] * a.m[13];
			c2 = m[12] * a.m[2] + m[13] * a.m[6] + m[14] * a.m[10] + m[15] * a.m[14];
			c3 = m[12] * a.m[3] + m[13] * a.m[7] + m[14] * a.m[11] + m[15] * a.m[15];

			m[12] = c0; m[13] = c1; m[14] = c2; m[15] = c3;
			return *this;
		}

		Vec3_T<T> operator * (const Vec3_T<T>& a) const;
		Vec4_T<T> operator * (const Vec4_T<T>& a) const;

		Mat4_T& scale(const Vec3_T<T>& a);
		Mat4_T scaled(const Vec3_T<T>& a)const;
		Mat4_T& rotate(const Quat_T<T>& a);
		Mat4_T rotated(const Quat_T<T>& a)const;
		Mat4_T& translate(const Vec3_T<T>& a);
		Mat4_T translated(const Vec3_T<T>& a)const;

		static Mat4_T scaling(const Vec3_T<T>& a);
		static Mat4_T rotation(const Quat_T<T>& a);
		static Mat4_T translation(const Vec3_T<T>& a);
		static Mat4_T transformation(const Vec3_T<T>& t, const Quat_T<T>& r, const Vec3_T<T>& s);

		static Mat4_T identity(const T a) {
			return{
				a, 0, 0, 0,
				0, a, 0, 0,
				0, 0, a, 0,
				0, 0, 0, a
			};
		}

		static Mat4_T transposed(const Mat4_T& m) {
			return {
				m.m[0], m.m[4], m.m[8], m.m[12],
				m.m[1], m.m[5], m.m[9], m.m[13],
				m.m[2], m.m[6], m.m[10], m.m[14],
				m.m[3], m.m[7], m.m[11], m.m[15]
			};
		}

		static Mat4_T<f32> perspective(const f32 _fov, const Vec2_T<f32>& res, const f32 near_z = 0.1f, const f32 far_z = 1000.0f);
		static Mat4_T<f32> perspectiveRZ(const f32 _fov, const Vec2_T<f32>& res, const f32 near_z = 0.1f, const f32 far_z = 1000.0f);
		static Mat4_T<f32> perspectiveI(const f32 _fov, const Vec2_T<f32>& res, const f32 near_z = 0.1f);
		static Mat4_T<f32> perspectiveIRZ(const f32 _fov, const Vec2_T<f32>& res, const f32 near_z = 0.1f);
	};

	typedef Mat4_T<float_c> Mat4;
	typedef Mat4_T<f32> FMat4;
	typedef Mat4_T<f64> DMat4;
}