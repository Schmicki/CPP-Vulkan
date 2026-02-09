#pragma once

#include "Container.h"
#include <initializer_list>


namespace sge {

	template<typename T, typename Ty>
	class Array_T : public Container_T<T, Ty> {
	public:

		inline Ty length() const {
			return this->size;
		}

		Array_T<T, Ty>()
			: Container_T<T, Ty>() {}

		Array_T<T, Ty>(std::initializer_list<T> items) noexcept
			:Container_T<T, Ty>(static_cast<Ty>(items.size()), calcConSize(static_cast<Ty>(items.size())), nullptr) {
			this->data = new T[this->conSize];

			for (Ty i = 0; i < this->size; i++) {
				this->data[i] = items.begin()[i];
			}
		}

		Array_T<T, Ty>(const Array_T<T, Ty>& other) noexcept
			:Container_T<T, Ty>(other) {}

		Array_T<T, Ty>(Array_T<T, Ty>&& other) noexcept
			:Container_T<T, Ty>(other) {}

		Array_T<T, Ty>& operator = (const Array_T<T, Ty>& other) {
			Container_T<T, Ty>::operator=(other);
			return *this;
		}

		Array_T<T, Ty>& operator = (Array_T<T, Ty>&& other) {
			if (&other != this) {
				Container_T<T, Ty>::operator=(mov(other));
			}
			return *this;
		}

		T& operator[](Ty index) const {
			return this->data[index];
		}

		inline T& get(Ty index) const {
			return this->data[index];
		}

		inline void clear() {
			if (this->data)
				delete[] this->data;
			this->size = 0;
			this->conSize = 0;
		}

		inline void expand() {
			this->conSize = incConSize(this->size);

			T* _data = new T[this->conSize];
			for (Ty i = 0; i < this->size; i++) {
				_data[i] = mov(this->data[i]);
			}

			if (this->data)
				delete[] this->data;
			this->data = _data;
		}

		inline void shrink() {
			this->conSize = calcConSize(this->size);

			T* _data = new T[this->conSize];
			for (Ty i = 0; i < this->size; i++) {
				_data[i] = mov(this->data[i]);
			}

			if (this->data)
				delete[] this->data;
			this->data = _data;
		}

		inline void resize(const Ty count, const T& defaultElem) noexcept {
			if (count == 0) {
				if (this->data)
					delete[] this->data;
				this->size = 0;
				this->conSize = 0;
				return;
			}
			else if (count == this->size) {
				return;
			}

			this->conSize = calcConSize(count);
			T* newData = new T[this->conSize];

			if (count > this->size) {
				for (Ty i = 0; i < this->size; i++) {
					newData[i] = mov(this->data[i]);
				}
				for (Ty i = this->size; i < count; i++) {
					newData[i] = defaultElem;
				}
			}
			else {
				for (Ty i = 0; i < count; i++) {
					newData[i] = mov(this->data[i]);
				}
			}

			if (this->data)
				delete[] this->data;
			this->data = newData;
			this->size = count;
		}

		inline void resize(const Ty count) noexcept {
			if (count == 0) {
				if (this->data)
					delete[] this->data;
				this->size = 0;
				this->conSize = 0;
				return;
			}
			else if (count == this->size) {
				return;
			}

			this->conSize = calcConSize(count);
			T* newData = new T[this->conSize];

			if (count > this->size) {
				for (Ty i = 0; i < this->size; i++) {
					newData[i] = mov(this->data[i]);
				}
			}
			else {
				for (Ty i = 0; i < count; i++) {
					newData[i] = mov(this->data[i]);
				}
			}

			if (this->data)
				delete[] this->data;
			this->data = newData;
			this->size = count;
		}

		inline bool isIndexValid(const Ty index) const {
			return index < this->size;
		}

		inline bool contains(const T& item) const {
			for (Ty i = 0; i < this->size; i++) {
				if (item == this->data[i])
					return true;
			}
			return false;
		}

		inline Ty find(const T& item) const {
			for (Ty i = 0; i < this->size; i++) {
				if (item == this->data[i])
					return i;
			}
			return 0;
		}

		inline void add(const T& item) {
			if (this->size == this->conSize) {
				expand();
			}
			this->data[this->size] = item;
			this->size++;
		}

		inline void addM(T&& item) {
			if (this->size == this->conSize) {
				expand();
			}
			this->data[this->size] = mov(item);
			this->size++;
		}

		inline void addUnique(const T& item) {
			if (contains(item))
				return;
			add(item);
		}

		inline void addUniqueM(T&& item) {
			if (contains(item))
				return;
			addM(item);
		}

		inline void append(const Array_T<T, Ty>& other) {
			if (&other != this && other.size != 0) {
				for (Ty i = 0; i < other.size; i++) {
					add(other.get(i));
				}
			}
		}

		inline void appendM(Array_T<T, Ty>&& other) {
			if (&other != this && other.size != 0) {
				for (Ty i = 0; i < other.size; i++) {
					addM(mov(other.get(i)));
				}
				delete[] other.data;
				other.data = nullptr;
				other.size = 0;
				other.conSize = 0;
			}
		}

		inline void remove(const Ty index) {
			if (isIndexValid(index)) {
				--this->size;
				for (Ty i = index; i < this->size; i++) {
					this->data[i] = this->data[i + 1];
				}
			}

			if (this->size < (this->conSize / 2)) {
				shrink();
			}
		}

		inline void removeI(const T& item) {
			Ty offset = 0;
			for (Ty i = 0; i < this->size; i++) {
				while ((&this->data[offset])[i] == item) {
					++offset;
				}
				this->data[i] = (&this->data[offset])[i];
				this->size -= offset;
			}

			if (this->size < (this->conSize / 2)) {
				shrink();
			}
		}

		inline void insert(const T& item, const Ty index) {
			if (this->size == this->conSize) {
				expand();
			}

			if (isIndexValid(index)) {
				for (Ty i = this->size; i > index; i--) {
					this->data[i] = mov(this->data[i - 1]);
				}
				this->data[index] = item;
			}
			else {
				this->data[this->size] = item;
			}
			++this->size;
		}

		inline void insertM(T&& item, const Ty index) {
			if (this->size == this->conSize) {
				expand();
			}

			if (isIndexValid(index)) {
				for (Ty i = this->size; i > index; i--) {
					this->data[i] = mov(this->data[i - 1]);
				}
				this->data[index] = mov(item);
			}
			else {
				this->data[this->size] = mov(item);
			}
			++this->size;
		}

		inline void swap(const Ty indexA, const Ty indexB) {
			if (isIndexValid(indexA) && isIndexValid(indexB)) {
				T cache = mov(this->data[indexA]);
				this->data[indexA] = mov(this->data[indexB]);
				this->data[indexB] = mov(cache);
			}
		}
	};


	template<typename T> using Array64 = Array_T<T, szt>;
	template<typename T> using Array = Array_T<T, u32>;
	template<typename T> using Array16 = Array_T<T, u16>;
}