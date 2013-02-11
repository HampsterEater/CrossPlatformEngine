///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#pragma once

// Defines:
//
//  Architecture:
//      ARCH_BIT_32      :   Compiling for a 32bit platform.
//      ARCH_BIT_64      :   Compiling for a 64bit platform.
//      ARCH_X86         :   Compiling for x86 cpu.
//      ARCH_X64         :   Compiling for x64 cpu.
//      ARCH_AMD64       :   Compiling for amd64 cpu.
//		ARCH_PPC		 :	Compiling for a ppc cpu.
//
//  Operating System:
//      OS_WIN      :   Compiling for windows.
//      OS_LINUX    :   Compiling for linux.
//      OS_MAC      :   Compiling for macos.
//      OS_IOS      :   Compiling for iOS.
//      OS_XBOX360  :   Compiling for xbox360.
//
//  Endianness:
//      ENDIAN_LITTLE   :   Compiling for LSB first architecture.
//      ENDIAN_BIG      :   Compiling for MSB first architecture.
//
//  Compile Mode:
//      DEBUG           : Debug compilation!
//      RELEASE         : Release compilation!
//
//  Macros:
//      __CURRENT_FUNCTION__    :   Compiler-independent name of the current function.
//

// Get compile mode.
#if defined(_DEBUG) || defined(__DEBUG) || defined(__DEBUG__) || defined(_DEBUG_) || defined(DEBUG)
    #ifdef DEBUG
        #undef DEBUG
    #endif
    #define DEBUG 1
#else
    #ifdef RELEASE
        #undef RELEASE
    #endif
    #define RELEASE 1
#endif

// Architecture macros.
#if defined(__i386__) || defined(_M_IX86) || defined(_X86_)
    #define ARCH_BIT_32  1
    #define ARCH_X86     1

#elif defined(__x86_64__) || defined(_M_X64) || defined(__amd64__)
    #define ARCH_BIT_64  1
    #define ARCH_X64     1
    #define ARCH_AMD64   1

#elif defined(__amd64__) || defined(_M_IA64)
    #define ARCH_BIT_64		1
    #define ARCH_ITANIUM64   1

#elif defined(_M_PPC)
	#define ARCH_BIT_32		1
	#define ARCH_PPC		1

#else
    #error(Attempt to compile for unknown architecture!)

#endif

// Create some generic macros defining what OS we are running on.
#if defined(__APPLE__)

    #include "TargetConditionals.h"

    #if defined(TARGET_OS_IPHONE) || defined(TARGET_IPHONE_SIMULATOR)
        #define OS_IOS  1

    #elif defined(TARGET_OS_MAC)
        #define OS_MAC  1

    #else
        #error(Attempt to compile for unknown platform!)

    #endif

#elif defined(__LINUX__) || defined(__linux)
    #define OS_LINUX    1

#elif defined(_XBOX_VER)
	#define OS_XBOX360	1

#elif defined(_WIN32) || defined(_WIN64) || defined(__MINGW32__) || defined(__BORLANDC__)
    #define OS_WIN  1

#else
    #error(Attempt to compile for unknown platform!)

#endif

// Endianness defines
#if defined(_M_PPCBE)
    #define ENDIAN_BIG      1

#elif defined(_M_PPC)
    #define ENDIAN_LITTLE    1

#elif defined(__LITTLE_ENDIAN__) || defined(_LITTLE_ENDIAN)
    #define ENDIAN_LITTLE   1

#elif defined(__BIG_ENDIAN__) || defined(_BIG_ENDIAN)
    #define ENDIAN_BIG      1

#elif defined(__BYTE_ORDER) && __BYTE_ORDER == _LITTLE_ENDIAN
    #define ENDIAN_LITTLE   1

#elif defined(__BYTE_ORDER) && __BYTE_ORDER == _BIG_ENDIAN
    #define ENDIAN_BIG      1

#elif defined(__sparc) || defined(__sparc__) \
    || defined(_POWER) || defined(__powerpc__) \
    || defined(__ppc__) || defined(__hpux) \
    || defined(_MIPSEB) || defined(_POWER) \
    || defined(__s390__)
    #define ENDIAN_BIG      1

#elif defined(__i386__) || defined(__alpha__) \
    || defined(__ia64) || defined(__ia64__) \
    || defined(_M_IX86) || defined(_M_IA64) \
    || defined(_M_ALPHA) || defined(__amd64) \
    || defined(__amd64__) || defined(_M_AMD64) \
    || defined(__x86_64) || defined(__x86_64__) \
    || defined(_M_X64) || defined(__bfin__)
    #define ENDIAN_LITTLE   1

#else
    #error(Attempt to compile for unknown endianness!)

#endif

// General macros.
#if __GNUC__ >= 2
    #define __CURRENT_FUNCTION__ __FUNCTION__
#elif defined(_MSC_VER)
    #define __CURRENT_FUNCTION__ __FUNCTION__
#else
    #define __CURRENT_FUNCTION__ "<unknown function>"
#endif

// Define datatypes.
// These should probably be replaced with typedef's
#define u8  char
#define u16 unsigned short
#define u32 unsigned int
#define u64 unsigned long long int

#define s8  signed char
#define s16 signed short
#define s32 signed int
#define s64 signed long long int

#define f32 float
#define f64 double

// Nulllllllerrrrooo
#ifndef NULL
	#define NULL 0
#endif
#ifndef null
	#define null 0
#endif

// Compiler specific things.
#if __GNUC__ >= 2
    #define FORCE_INLINE __attribute__((always_inline))
#elif defined(_MSC_VER)
    #define FORCE_INLINE __forceinline 
#else
    #define FORCE_INLINE
#endif

// Seriously MS, go fuck yourselves.
// I will not use your shitty unportable CRT functions.
#if defined(_MSC_VER)
	#define _CRT_SECURE_NO_WARNINGS // Microsoft in there wisdom decided that this should only work sparadically, hence below lines.
	#pragma warning(disable : 4996)	// C4996 warning, you can fuck right off.

	// Seriously? Removing stanardized C++ and adding underscores? >_> No
	#define snprintf _snprintf 
#endif




