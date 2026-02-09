#pragma once


enum D_COMPILER {
	MICROSOFT = 1, GCC, CLANG
};


enum D_ARCHITECTURE {
	X86 = 1, X64, ARM32, ARM64
};

enum D_SYSTEM {
	WINDOWS = 1, LINUX, ANDROID, MACOS
};



/*
* Detect compiler
*/

#if defined _MSC_VER

#define D_COMPILER_MICROSOFT 1
#define D_CONST_COMPILER D_COMPILER::MICROSOFT
#define D_EXPORT __declspec(dllexport)
#define D_IMPORT __declspec(dllimport)
#define D_DONT_EXPORT
#define D_FORCE_INLINE __forceinline


#elif defined __GNUC__ || __MINGW32__ || __MINGW64__

#define D_COMPILER_GCC 1
#define D_CONST_COMPILER D_COMPILER::GCC
#define D_SHARED_EXPORT __attribute__((visibility("default")))
#define D_SHARED_IMPORT
#define D_DONT_EXPORT __attribute__((visibility("hidden")))
#define D_FORCE_INLINE __attribute__((always_inline))


#elif defined __clang__

#define D_COMPILER_CLANG 1
#define D_CONST_COMPILER D_COMPILER::CLANG
#define D_SHARED_EXPORT __attribute__((visibility("default")))
#define D_SHARED_IMPORT
#define D_DONT_EXPORT __attribute__((visibility("hidden")))
#define D_FORCE_INLINE __attribute__((always_inline))


#else

#error detection could not find compiler, please implement it

#endif



/*
* Detect architecture
*/

#if defined _M_AMD64 || _M_X64 || __x86_64__ || __amd64__

#define D_ARCHITECTURE_X64 1
#define D_CONST_ARCHITECTURE D_ARCHITECTURE::X64


#elif defined _M_IX86 || __i386__

#define D_ARCHITECTURE_X86 1
#define D_CONST_ARCHITECTURE D_ARCHITECTURE::X86


#elif defined _M_ARM64 || __aarch64__

#define D_ARCHITECTURE_ARM64 1
#define D_CONST_ARCHITECTURE D_ARCHITECTURE::ARM64


#elif defined _M_ARM || __arm__

#define D_ARCHITECTURE_ARM32 1
#define D_CONST_ARCHITECTURE D_ARCHITECTURE::ARM32


#else

#error detection could not find architecture, please implement it

#endif



/*
* Detect system
*/

#if defined _WIN64 || _WIN32 || WIN32

#define D_SYSTEM_WINDOWS 1
#define D_CONST_SYSTEM D_SYSTEM::WINDOWS


#elif defined __ANDROID__ || ANDROID

#define D_SYSTEM_ANDROID 1
#define D_CONST_SYSTEM D_SYSTEM::ANDROID


#elif defined __linux__

#define D_SYSTEM_LINUX 1
#define D_CONST_SYSTEM D_SYSTEM::LINUX


#elif defined __APPLE__

#define D_SYSTEM_MACOS 1
#define D_CONST_SYSTEM D_SYSTEM::MACOS


#else
#error detection could not find system, please implement it
#endif