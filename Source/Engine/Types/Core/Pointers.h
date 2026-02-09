#pragma once

template<class T> class SelfDeletingPointer {
public:
	T* data;

	SelfDeletingPointer() :data(nullptr) {

	}

	SelfDeletingPointer(const SelfDeletingPointer<T>& other) noexcept
		:data(other.data) {

	}

	SelfDeletingPointer(SelfDeletingPointer<T>&& other) noexcept
	:data(other.data){
		other.data = nullptr;
	}

	SelfDeletingPointer(T* ptr) :data(ptr) {

	}

	~SelfDeletingPointer() {
		if (data)
			delete[] data;
	}

	SelfDeletingPointer<T>& operator = (SelfDeletingPointer<T>&& other) noexcept {
		if (data)
			delete[] data;

		data = other.data;
		other.data = nullptr;
		return *this;
	}

	SelfDeletingPointer<T>& operator = (const SelfDeletingPointer<T>& other) noexcept {
		if (data)
			delete[] data;
		data = other.data;
		return *this;
	}

	operator T* () noexcept {
		return data;
	}
};