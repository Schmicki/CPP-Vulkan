#pragma once

#include "Container.h"
#include "Array.h"


namespace sge {
	template<class KeyType, class DataType, class SizeType>
	class Map_T : public Container_T<DataType, SizeType> {
	public:

		using Tx = KeyType;
		using T = DataType;
		using Ty = SizeType;

		Tx* keys;

		Tx* copyKeys() const {
			Tx* _kys = new Tx[this->conSize];
			for (Ty i = 0; i < this->size; i++) {
				_kys[i] = keys[i];
			}
		}

		Map_T() noexcept :keys(nullptr) {}

		Map_T(const Map_T<Tx, T, Ty>& other) noexcept
			:Container_T<T, Ty>(other), keys(other.copyKeys()) {}

		Map_T(Map_T<Tx, T, Ty>&& other) noexcept
			:Container_T<T, Ty>(mov(other)), keys(other.keys) {
			if (other.keys)
				delete[] other.keys;
			other.keys = nullptr;
		}

		~Map_T() {
			if (keys)
				delete[] keys;
		}

		Map_T<Tx, T, Ty>& operator=(const Map_T<Tx, T, Ty>& other) noexcept {
			Container_T<T, Ty>::operator=(other);
			if (keys)
				delete[] keys;
			keys = other.copyKeys();
			return *this;
		}

		Map_T<Tx, T, Ty>& operator=(Map_T<Tx, T, Ty>&& other) noexcept {
			Container_T<T, Ty>::operator=(mov(other));
			if (this != &other) {
				if (keys)
					delete[] keys;
				keys = other.keys;
				other.keys = nullptr;
			}
			return *this;
		}

		bool findFirstIndex(const Tx& key, Ty& index) const {
			index = 0;

			if (this->size > 0) {
				
				do {
					index += 100000;
				} while (this->size > index && keys[index] < key);
				index -= 100000;

				do {
					index += 10000;
				} while (this->size > index && keys[index] < key);
				index -= 10000;

				do {
					index += 1000;
				} while (this->size > index && keys[index] < key);
				index -= 1000;

				do {
					index += 100;
				} while (this->size > index && keys[index] < key);
				index -= 100;

				do {
					index += 10;
				} while (this->size > index && keys[index] < key);
				index -= 10;

				while (this->size > index && keys[index] < key) {
					index += 1;
				};

				if (index < this->size) {
					return !(key < keys[index]);
				}
			}
			return false;
		}

		bool findFirstAndLastIndex(const Tx& key, Ty& first, Ty& last) const {

			if (findFirstIndex(key, first)) {
				last = first;
				do {
					last += 10000;
				} while (this->size > last && !(key < keys[last]));
				last -= 10000;

				do {
					last += 1000;
				} while (this->size > last && !(key < keys[last]));
				last -= 1000;

				do {
					last += 100;
				} while (this->size > last && !(key < keys[last]));
				last -= 100;

				do {
					last += 10;
				} while (this->size > last && !(key < keys[last]));
				last -= 10;

				while (this->size > last && !(key < keys[last])) {
					last += 1;
				};

				if (last) {
					last -= 1;
				}
				return true;
			}
			last = first;
			return false;
		}

		inline T& getIndex(const Ty index) const {
			return this->data[index];
		}

		void getAll(const Tx& key, Array_T<T*, Ty>& dst) const {
			dst.clear();
			Ty index;
			if (findFirstIndex(key, index)) {
				dst.add(&this->data[index]);
				index++;
				while (!(key < keys[index])) {
					dst.add(&this->data[index]);
					index++;
				}
			}
		}

		// * Do not use this function unless you know what youre doing!
		// * This function does not apply sorting!
		// * Use insert(), add() or addUnique() to apply the neccessery sorting algorithms
		void _insert(const Ty index, const Tx& key, const T& value) {
			if (this->size == this->conSize) {
				this->conSize = incConSize(this->conSize);
				Tx* nkeys = new Tx[this->conSize];
				T* ndata = new T[this->conSize];

				for (Ty i = 0; i < this->size; i++) {
					ndata[i] = mov(this->data[i]);
					nkeys[i] = mov(keys[i]);
				}

				if (this->data) {
					delete[] this->data;
					delete[] this->keys;
				}
				this->data = ndata;
				keys = nkeys;
			}
			for (Ty i = this->size; i > index; i--) {
				this->data[i] = mov(this->data[i - 1]);
				keys[i] = mov(keys[i - 1]);
			}

			keys[index] = key;
			this->data[index] = value;
			this->size++;
		}

		inline Ty add(const Tx& key, const T& value) {
			Ty index;
			findFirstIndex(key, index);
			_insert(index, key, value);
			return index;
		}

		inline Ty addUnique(const Tx& key, const T& value) {
			Ty index;
			if (!findFirstIndex(key, index))
				_insert(index, key, value);
			return index;
		}

		inline void insert(const Tx& key, const T& value) {
			add(key, value);
		}

		void _remove(const Ty startIndex, const Ty _size) {
			this->size -= _size;
			for (Ty i = startIndex; i < this->size; i++) {
				this->data[i] = mov(this->data[i + _size]);
				this->keys[i] = mov(this->keys[i + _size]);
			}

			if (this->size < (this->conSize / 2)) {
				this->conSize = calcConSize(this->size);

				T* _data = new T[this->conSize];
				Tx* _keys = new Tx[this->conSize];
				for (Ty i = 0; i < this->size; i++) {
					_data[i] = mov(this->data[i]);
					_keys[i] = mov(keys[i]);
				}

				if (this->data) {
					delete[] keys;
					delete[] this->data;
				}
				this->data = _data;
				keys = _keys;
			}
		}

		inline void remove(const Tx& key) {
			Ty index;
			if (findFirstIndex(key, index)) {
				_remove(index, 1);
			}
		}

		void removeAll(const Tx& key) {
			Ty index;
			if (findFirstIndex(key, index)) {
				Ty index2 = index + 1;
				while (this->size > index2 && !(key < keys[index2])) {
					index2 += 1;
				};
				_remove(index, index2 - index);
			}
		}

		inline void removeIndex(const Ty index) {
			if (index < this->size) {
				_remove(index, 1);
			}
		}
	};

	template<class KeyType, class DataType> using Map64 = Map_T<KeyType, DataType, szt>;
	template<class KeyType, class DataType> using Map = Map_T<KeyType, DataType, u32>;
	template<class KeyType, class DataType> using Map16 = Map_T<KeyType, DataType, u16>;
}