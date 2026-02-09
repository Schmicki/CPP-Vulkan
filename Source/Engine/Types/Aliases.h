#pragma once

#include "Detection.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifndef NULL
	#define NULL 0
#endif

#if defined DEBUG || _DEBUG
	#define IF_DEBUG(code) code
	#define IF_RELEASE(code)
#else
	#define IF_DEBUG(code)
	#define IF_RELEASE(code) code
#endif

#if D_SYSTEM_WINDOWS
	#define SGE_PLATFORM_WINDOWS 1
#elif D_SYSTEM_LINUX
	#define SGE_PLATFORM_LINUX 1
#endif

#define FORCE_INLINE D_FORCE_INLINE

#if defined SGE_USE_DOUBLE
	typedef double float_c;
#else
	typedef float float_c;
#endif

// Short type aliases
typedef long long llong;
typedef unsigned char uchar;
typedef unsigned char ubyte;
typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned long ulong;
typedef unsigned long long ullong;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float f32;
typedef double f64;

typedef size_t szt;

template<class To, class From>
inline To to(const From& from) {
	return static_cast<To>(from);
}

template<class Ptr>
inline Ptr as(void* from) {
	return reinterpret_cast<Ptr>(from);
}

template<class Ptr>
inline Ptr as(const void* from) {
	return reinterpret_cast<Ptr>(from);
}