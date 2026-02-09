#pragma once
#include <stdint.h>


namespace sge {

	// UTF-8
	// 01111111                             7-bit ASCII characters
	// 110xxxxx 10xxxxxx                    2-byte sequence 11-bit characters 
	// 1110xxxx 10xxxxxx 10xxxxxx           3-byte sequence 16-bit characters
	// 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx  4-byte sequence 21-bit characters
	// 
	// ASCII: 0 - 127
	// 
	// Start: 192u - 253u		||	-64 - -3
	// Following: 128u - 191u	||	-128 - -65
	// 
	// Start 2: 192u - 223u		||	-64 - -33
	// Start 3: 224u - 239u		||	-32 - -17
	// Start 4: 240u - 247u		||	-16 - -9
	// Start 5: 248u - 251u		||	-8 - -5
	// Start 6: 252u - 253u		||	-4 - -3
	// 
	// 

	template<class T>
	T utf8Count(char* str) {
		T count = 0;
		for (T i = 0; str[i] != 0; count++) {
			i += 1 + (as<unsigned char*>(str)[i] > 0xBF) + (as<unsigned char*>(str)[i] > 0xDF) + (as<unsigned char*>(str)[i] > 0xEF);
		}
		return count;
	}

	template<class T>
	T utf8EncodeCodePoint(char* str, u32 codePoint) {
		if (codePoint < 0x80) {
			str[0] = codePoint;
			return 1;
		}
		else if (codePoint < 0x800) {
			str[0] = 0xC0 | static_cast<char>(codePoint >> 6);
			str[1] = 0x80 | static_cast<char>(codePoint & 0x3F);
			return 2;
		}
		else if (codePoint < 0x10000) {
			str[0] = 0xE0 | static_cast<char>(codePoint >> 12);
			str[1] = 0x80 | static_cast<char>(codePoint >> 6 & 0x3F);
			str[2] = 0x80 | static_cast<char>(codePoint & 0x3F);
			return 3;
		}
		else {
			str[0] = 0xF0 | static_cast<char>(codePoint >> 18);
			str[1] = 0x80 | static_cast<char>(codePoint >> 12) & 0x3F;
			str[2] = 0x80 | static_cast<char>(codePoint >> 6) & 0x3F;
			str[3] = 0x80 | static_cast<char>(codePoint & 0x3F);
			return 4;
		}
	}

	inline u32 utf8DecodeOne(const char c) {
		return c;
	}

	inline u32 utf8DecodeTwo(const char* str) {
		return ((0x1F & str[0]) << 6) | (0x1F & str[1]);
	}

	inline u32 utf8DecodeThree(const char* str) {
		return ((0xF & str[0]) << 12) | ((0x1F & str[1]) << 6) | (0x1F & str[2]);
	}

	inline u32 utf8DecodeFour(const char* str) {
		return ((0x7 & str[0]) << 18) | ((0x1F & str[1]) << 12) | ((0x1F & str[2]) << 6) | (0x1F & str[3]);
	}


	// UTF-16 Little Endian & Big Endian (standard x86 is little endian, the byte order is mirrored on big endian)
	// xxxxxxxx xxxxxxxx                    16-bit Unicode characters
	// 110110xx xxxxxxxx                    High
	// 110111xx xxxxxxxx                    Low
	// 110110xx xxxxxxxx 110110xx xxxxxxxx  4-byte sequence 20-bit charcaters
	// Everything between 11011000 00000000 - 11011011 11111111 (0xD800 - 0xDBFF) and 11011100 00000000 - 11011111 11111111 (0xDC00 - 0xDFFF)
	// is reserved space in unicode so you can identify UTF-16 "Surrogate Pairs"
	// 
	// 

	template<class T>
	inline T utf16EncodeCodePoint(u16* str, u32 codePoint) {
		if (codePoint < 0x10000) {
			str[0] = static_cast<u16>(codePoint);
			return 1;
		}
		else {
			codePoint -= 0x10000;
			str[0] = 0xD800 | ((codePoint >> 10) & 0x3FF);
			str[1] = 0xDC00 | (codePoint & 0x3FF);
			return 2;
		}
	}

	inline bool utf16IsSingle(const u16 c)
	{
		return c < 0xD800 || c > 0xDFFF;
	}
	inline u32 utf16DecodeSurrogates(const u16* sur) {
		return ((static_cast<u32>(sur[0]) & 1023) << 10) | (static_cast<u32>(sur[1]) & 1023) + 65536;
	}


	// UTF-32
	// 
	// 
}