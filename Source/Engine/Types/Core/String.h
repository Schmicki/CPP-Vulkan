#pragma once

#include "Container.h"
#include "Array.h"
#include "Unicode.h"

namespace sge {
	template<typename Ty>
	class String_T : public Container_T<char, Ty> {
	public:

		static Ty length(const char* string) {
			Ty i = 0;
			for (; string[i] != 0; i++);
			return i;
		}

		inline Ty length() {
			return this->size;
		}

		void append(const char* string, const Ty len) {
			Ty nlen = this->size + len;

			if (nlen >= this->conSize) {
				this->conSize = calcConSize(nlen + 1);
				char* newData = new char[this->conSize];
				for (Ty i = 0; i < this->size; i++) {
					newData[i] = this->data[i];
				}
				for (Ty i = 0; i < len; i++) {
					(&newData[this->size])[i] = string[i];
				}
				newData[nlen] = 0;
				if (this->data)
					delete[] this->data;
				this->data = newData;
			}
			else {
				for (Ty i = 0; i < len; i++) {
					(&this->data[this->size])[i] = string[i];
				}
				this->data[nlen] = 0;
			}

			this->size = nlen;
		}

		inline void append(const char* string) {
			Ty len = length(string);
			append(string, len);
		}

		u16* toUTF16() const {
			if (!this->data)
				return nullptr;

			Ty len = 0;
			for (Ty i = 0; this->data[i] != 0;) {
				Ty ix = 1 + (as<unsigned char*>(this->data)[i] > 0xBF) + (as<unsigned char*>(this->data)[i] > 0xDF) + (as<unsigned char*>(this->data)[i] > 0xEF);
				i += ix;
				len += 1 + (ix > 3);
			}
			len++;
			u16* ret = new u16[len];
			ret[len - 1] = 0;

			for (Ty i = 0, ix = 0; this->data[i] != 0;) {
				Ty iy = 1 + (as<unsigned char*>(this->data)[i] > 0xBF) + (as<unsigned char*>(this->data)[i] > 0xDF) + (as<unsigned char*>(this->data)[i] > 0xEF);
				u32 codePoint = 0;

				if (iy == 1) {
					codePoint = this->data[i];
				}
				else if (iy == 2) {
					codePoint = utf8DecodeTwo(this->data + i);
				}
				else if (iy == 3) {
					codePoint = utf8DecodeThree(this->data + i);
				}
				else if (iy == 4) {
					codePoint = utf8DecodeFour(this->data + i);
				}

				i += iy;
				ix += utf16EncodeCodePoint<Ty>(ret + ix, codePoint);
			}

			return ret;
		}

		u32* toUTF32() const {
			if (!this->data)
				return nullptr;

			Ty count = utf8Count(this->data);
			u32* ret = new u32[count + 1];
			ret[count] = 0;

			for (Ty i = 0, ix = 0; this->data[ix] != 0; i++) {
				if (this->data[ix] > 0) {
					ret[i] = this->data[ix];
					ix++;
				}
				else if (this->data[ix] < -0x20) {
					ret[i] = utf8DecodeTwo(this->data + ix);
					ix += 2;
				}
				else if (this->data[ix] < -0x10) {
					ret[i] = utf8DecodeThree(this->data[ix]);
					ix += 3;
				}
				else {
					ret[i] = utf8DecodeFour(this->data[ix]);
					ix += 4;
				}
			}
		}

		wchar_t* toWString() const {
			if constexpr (sizeof(wchar_t) == 2) {
				return as<wchar_t*>(toUTF16());
			}
			else if constexpr (sizeof(wchar_t) == 4) {
				return as<wchar_t*>(toUTF32());
			}
		}

		String_T<Ty>() noexcept
			:Container_T<char, Ty>(0, 0, nullptr) {}

		String_T<Ty>(Ty size) noexcept
			:Container_T<char, Ty>(size, calcConSize(size + 1), nullptr) {
			this->data = new char[this->conSize];
			this->data[this->size] = 0;
		}

		String_T<Ty>(const char* string) noexcept
			:Container_T<char, Ty>(length(string), 0, nullptr) {
			this->conSize = calcConSize(this->size + 1);
			this->data = new char[this->conSize];

			for (Ty i = 0; i < this->size; i++) {
				this->data[i] = string[i];
			}
			this->data[this->size] = 0;
		}

		String_T<Ty>(const u16* string) {
			Ty len = 0;
			for (Ty i = 0; string[i] != 0;) {
				if (utf16IsSingle(string[i])) {
					len += 1 + (string[i] > 0x7F) + (string[i] > 0x7FF);
					i++;
				}
				else {
					len += 4;
					i += 2;
				}
			}
			char* str = new char[len];

			for (Ty i = 0, ix = 0; string[i] != 0;) {
				if (utf16IsSingle(string[i])) {
					ix += utf8EncodeCodePoint<Ty>(str + ix, string[i]);
					i++;
				}
				else {
					ix += utf8EncodeCodePoint<Ty>(str + ix, utf16DecodeSurrogates(string + i));
					i += 2;
				}
			}
			append(str, len);
			delete[] str;
		}

		String_T<Ty>(const u32* string) {
			Ty len = 0;
			for (Ty i = 0; string[i] != 0; i++) {
				len += 1 + (string[i] > 0x7F) + (string[i] > 0x7FF);
			}

			char* str = new char[len];

			for (Ty i = 0, ix = 0; string[i] != 0; i++) {
				ix += utf8EncodeCodePoint(str + ix, string[i]);
			}
			append(str, len);
			delete[] str;
		}

		String_T<Ty>(const wchar_t* string) {
			if constexpr (sizeof(wchar_t) == 2) {
				*this = mov(String_T<Ty>(as<const u16*>(string)));
			}
			else if constexpr (sizeof(wchar_t) == 4) {
				*this = mov(String_T<Ty>(as<const u32*>(string)));
			}
		}

		String_T<Ty>(const String_T<Ty>& other) noexcept
			:Container_T<char, Ty>(other) {
			if (this->data)
				this->data[this->size] = 0;
		}

		String_T<Ty>(String_T<Ty>&& other) noexcept
			:Container_T<char, Ty>(mov(other)) {}

		String_T<Ty>& operator=(const String_T<Ty>& other) noexcept {
			Container_T<char, Ty>::operator=(other);
			if (this->data)
				this->data[this->size] = 0;
			return *this;
		}

		String_T<Ty>& operator=(String_T<Ty>&& other) noexcept {
			if (&other != this) {
				Container_T<char, Ty>::operator=(mov(other));
			}
			return *this;
		}

		operator char* () const
		{
			return this->data;
		}

		char& operator[](Ty index) const {
			return this->data[index];
		}

		bool operator<(const String_T<Ty>& other) const {

			if (other.size < this->size) {
				for (Ty i = 0; i < other.size; i++) {
					if (this->data[i] != other[i])
						return this->data[i] < other[i];
				}
				return false;
			}
			else {
				for (Ty i = 0; i < this->size; i++) {
					if (this->data[i] != other[i])
						return this->data[i] < other[i];
				}
				return this->size != other.size;
			}
		}

		bool operator==(const String_T<Ty>& other) const {
			if (other.size == this->size) {
				for (Ty i = 0; i < this->size; i++) {
					if (this->data[i] != other[i])
						return false;
				}
				return true;
			}
			return false;
		}

		inline void clear() {
			if (this->data)
				delete[] this->data;
			this->size = 0;
			this->conSize = 0;
		}

		void resize(const Ty count, const char defaultElem) noexcept {
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

			this->conSize = calcConSize(count + 1);
			char* newData = new char[this->conSize];

			if (count > this->size) {
				for (Ty i = 0; i < this->size; i++) {
					newData[i] = this->data[i];
				}
				for (Ty i = this->size; i < count; i++) {
					newData[i] = defaultElem;
				}
			}
			else {
				for (Ty i = 0; i < count; i++) {
					newData[i] = this->data[i];
				}
			}

			newData[count] = 0;
			if (this->data)
				delete[] this->data;
			this->data = newData;
			this->size = count;
		}

		void resize(const Ty count) noexcept {
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

			this->conSize = calcConSize(count + 1);
			char* newData = new char[this->conSize];

			if (count > this->size) {
				for (Ty i = 0; i < this->size; i++) {
					newData[i] = this->data[i];
				}
			}
			else {
				for (Ty i = 0; i < count; i++) {
					newData[i] = this->data[i];
				}
			}

			newData[count] = 0;
			if (this->data)
				delete[] this->data;
			this->data = newData;
			this->size = count;
		}

		void sub(Ty start, Ty end, String_T<Ty>& dst) const {
			if (end > this->size)
				end = this->size;
			dst.resize(end - start);
			for (Ty i = start; i < end; i++) {
				dst.data[i - start] = this->data[i];
			}
		}

		bool beginsWith(const char* string) const {
			Ty len = length(string);
			if (len <= this->size) {
				for (Ty i = 0; i < len; i++) {
					if (this->data[i] != string[i])
						return false;
				}
				return true;
			}
			return false;
		}

		bool endsWith(const char* string) const {
			Ty len = length(string);
			if (len <= this->size && len > 0) {
				Ty off = this->size - len;
				for (Ty i = off; i < this->size; i++) {
					if (this->data[i] != string[i - off])
						return false;
				}
				return true;
			}
			return false;
		}

		Ty find(const char c, const Ty offset = 0) const {
			for (Ty i = offset; i < this->size; i++) {
				if (this->data[i] == c)
					return i;
			}
			return this->size;
		}

		Ty find(const char* string, Ty offset = 0) const {
			Ty len = length(string);
			Ty count = 0;
			for (Ty i = offset; i < this->size; i++) {
				if (string[count] == this->data[i]) {
					count++;
					if (count == len)
						return i - count + 1;
				}
				else {
					count = 0;
				}
			}
			return this->size;
		}

		Ty findLast(const char c) const {
			for (Ty i = this->size - 1; i > 0; i--) {
				if (this->data[i] == c) {
					return i;
				}
			}

			return this->size;
		}

		bool contains(const char* string) const {
			Ty len = length(string);
			Ty count = 0;
			for (Ty i = 0; i < this->size; i++) {
				if (string[count] == this->data[i]) {
					count++;
					if (count == len)
						return true;
				}
				else {
					count = 0;
				}
			}
			return false;
		}

		void split(const char sep, Array_T<String_T<Ty>, Ty>& dst) const {
			dst.clear();

			Ty first = 0;
			while (this->data[first] == sep)
				first++;

			for (Ty i = 0; i < this->size; i++) {
				if (this->data[i] == sep) {
					String_T<Ty> string;
					sub(first, i, string);
					dst.addM(mov(string));
					while (this->data[i] == sep)
						i++;
					first = i;
				}
			}

			Ty last = this->size;
			if (first != last) {
				String_T<Ty> string;
				sub(first, last, string);
				dst.addM(mov(string));
			}
		}
		// The index you specify, is going to be the first of the right string.
		void split(const Ty index, String_T<Ty>& left, String_T<Ty>& right) const {
			left.clear();
			right.clear();

			left.resize(index);
			right.resize(this->size - index);

			for (Ty i = 0; i < index; i++) {
				left.data[i] = this->data[i];
			}
			for (Ty i = 0; i < right.size; i++) {
				right.data[i] = (&this->data[index])[i];
			}
		}

		void replace(const char* old, const char* _new) {
			Ty leno = length(old);
			Ty lenn = length(_new);

			String_T<Ty> string;

			Ty first = 0;
			Ty count = 0;
			for (Ty i = 0; i < this->size; i++) {


				if (old[count] == this->data[i]) {
					if (count < 1 && first != i) {
						String_T<Ty> substr;
						sub(first, i, substr);
						string.append(substr.data, substr.size);
					}

					count++;
					if (count == leno) {
						string.append(_new);
						count = 0;
						first = i + 1;
					}
				}
				else if (count > 0) {
					count = 0;
				}
			}
			*this = mov(string);
		}
	};

	using String16 = String_T<u16>;
	using String = String_T<u32>;
	using String64 = String_T<szt>;
}