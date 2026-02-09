#pragma once
#include "TypeInfo.h"
#include "../Aliases.h"
#include <limits>


namespace sge {

	template<typename T> using CalcConFunc = T(*)(const T);

	template<typename T> T calcConSize(const T target) noexcept;
	template<typename T> T incConSize(const T target) noexcept;


	template<typename T, typename Ty>
	class Container_T {
	public:
		static_assert(TypesEqual<Ty, u16>::value() || TypesEqual<Ty, u32>::value() || TypesEqual<Ty, szt>::value(), "tried to compile string with wrong size type! Use String, String32 or String16 instead.");

		Ty size;
		Ty conSize;
		T* data;

		inline T* copyd() const noexcept {
			if (!data)
				return nullptr;

			T* _data = new T[conSize];
			for (Ty i = 0; i < size; i++) {
				_data[i] = data[i];
			}

			return _data;
		}

		Container_T<T, Ty>() noexcept
			: size(0), conSize(0), data(nullptr) {}

		Container_T<T, Ty>(Ty _size, Ty _conSize, T* _data) noexcept
			: size(_size), conSize(_conSize), data(_data) {}

		Container_T<T, Ty>(const Container_T<T, Ty>& other) noexcept
			: size(other.size), conSize(other.conSize), data(other.copyd()) {}

		Container_T<T, Ty>(Container_T<T, Ty>&& other) noexcept
			: size(other.size), conSize(other.conSize), data(other.copyd()) {
			other.conSize = 0;
			other.size = 0;
			other.data = nullptr;
		}

		~Container_T<T, Ty>() {
			if (data)
				delete[] data;
		}

		Container_T<T, Ty>& operator = (const Container_T<T, Ty>& other) noexcept {
			if (data)
				delete[] data;
			size = other.size;
			conSize = other.conSize;
			data = other.copyd();
			return *this;
		}

		Container_T<T, Ty>& operator = (Container_T<T, Ty>&& other) noexcept {
			if (data)
				delete[] data;
			size = other.size;
			conSize = other.conSize;
			data = other.data;

			other.data = nullptr;
			other.size = 0;
			other.conSize = 0;
			return *this;
		}
	};


	template<typename T>
	T calcConSize(const T target) noexcept {
		const T halfOfMax = std::numeric_limits<T>::max() / 2;

		T i = 4;
		while (i < target) {
			if (i > halfOfMax) {
				i = std::numeric_limits<T>::max();
				break;
			}
			else {
				i *= 2;
			}
		}
		return i;
	}

	template<typename T>
	T incConSize(const T _conSize) noexcept {

		if (_conSize == 0) {
			return 4;
		}

		const T halfOfMax = std::numeric_limits<T>::max() / 2;

		T i = _conSize;
		if (i > halfOfMax) {
			i = std::numeric_limits<T>::max();
		}
		else {
			i *= 2;
		}
		return i;
	}
}