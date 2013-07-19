//
//  Platform.h
//  Mist
//
//  Created by Robert Bu on 11/21/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef MISTPlatform_h
#define MISTPlatform_h

#if !defined(__cplusplus) && !defined(__APPLE__)
    #error C++ compiler required.
#endif

#if defined(DEBUG) || defined(_DEBUG)
    #define MIST_DEBUG
#endif

// cpp standard version check, however, some compilers such as llvm currently define this as 1
// which the check fail
#if __cplusplus > 199711L
    #define MIST_CPP11
#endif

#if defined(__llvm__)
    #if defined(__clang__)
        // clang feature check
        // http://clang.llvm.org/docs/LanguageExtensions.html#has_feature_back_compat
        #if __has_feature(cxx_variadic_templates)
            #define MIST_VARIADIC_TEMPLATE
        #endif
    #endif

    #define MIST_COMPILTER_LLVM

    // the newest version of llvm and clang supports most of the c++11
    #define MIST_CPP11

#endif // LLVM

#if defined(__GNUC__)

    #define MIST_COMPILER_GCC

    #if __GNUC_MINOR__ >= 3
        #ifdef __GXX_EXPERIMENTAL_CXX0X__
            #define UKN_CXX0X_SUPPORT
        #endif // __GXX_EXPERIMENTAL_CXX0X__
    #endif // __GNUC_MINOR__

    // gcc 4.7 supports most of the c++11
    #if __GNUC_MINOR__ >= 7
        #define MIST_CPP11
    #endif

#endif // GCC

#if defined(_MSC_VER)

    #define MIST_COMPILER_MSVC
    #define MIST_HAS_DECLSPEC

    // visual studio 2012, supports most of the cpp11 features
    #if _MSC_VER >= 1700
        #define MIST_CPP11
    #endif

#endif // VC++

// vc++ override specification
#if !defined(MIST_COMPILER_MSVC)
#define override
#endif


// apple os ( osx / ios )
#if defined(__APPLE__) || defined(__APPLE_CC__)

#include "TargetConditionals.h"
#include <Availability.h>

#if defined(TARGET_OS_MAC) && TARGET_OS_MAC == 1 && TARGET_IPHONE_SIMULATOR == 0

#define MIST_OS_OSX

#endif

#if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE == 1 || TARGET_IPHONE_SIMULATOR == 1

    #define MIST_OS_IOS
    #define MIST_OS_MOBILE

#endif

    #if MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_7
        #define MIST_APPLE_OSX_LION
    #endif
    
    #define MIST_OS_FAMILY_APPLE
    #define MIST_OS_FAMILY_UNIX

// android
#elif defined(__ANDROID__)
    #define MIST_OS_ANDROID
    #define MIST_OS_MOBILE

// windows
#elif defined(_WIN32) || defined(_WIN64)
    #define MIST_OS_WINDOWS

    #if defined(_WIN64)
        #define MIST_PLATFORM_64
    #else
        #define MIST_PLATFORM_32
    #endif

    #if defined(_M_IX86)
        #define MIST_PROCESSOR_X86  
    #elif defined(_M_X64)
        #define MIST_PROCESSOR_X64
    #endif

    #if defined(_CPPRTTI)
        #define MIST_HAS_RTTI
    #endif

    // windows 8 C++/CX, Some Components in WinRT is different, such as FileUtil, to do
    // /ZW
    #if defined(__cplusplus_winrt)
        #define MIST_OS_WINRT
    #endif

    #ifndef NOMINMAX
        #define NOMINMAX
    #endif

    #if defined(__MINGW32__)
        #include <_mingw.h>
    #endif

    #if defined(__CYGWIN__)
        #define MIST_PLATFORM_CYGWIN
        #define MIST_HAS_DECLSPEC
    #endif

// linux
#elif defined(linux) || defined(__linux)
    #define MIST_OS_LINUX
    #define MIST_OS_FAMILY_UNIX

#else
    #error Unknown Platform
#endif // UKN_PLATFORM

#include <cassert>

#if defined(MIST_STD_CALL)
    #define MIST_CALL __stdcall
#else
    #define MIST_CALL
#endif

#if defined(MIST_OS_WINDOWS)
    #define strcmpnocase stricmp
#else
    #define strcmpnocase strcasecmp
#endif

// are we building a dll or not
#if defined(MIST_OS_WINDOWS) && defined(MIST_HAS_DECLSPEC)
    #if defined(MIST_EXPORTS)
        #define MIST_API __declspec(dllexport)
        #define MIST_EXTERN extern
    #else
        #define MIST_API __declspec(dllimport)
        #define MIST_EXTERN extern
    #endif
#else
    #define MIST_API
    #define MIST_EXTERN
#endif

#define mist_assert assert

#ifdef MIST_OS_WINDOWS
#define __inline__ inline
#endif

#ifndef __GNUC__
    #define snprintf _snprintf
#endif

// #define UKN_ENABLE_MULTI_THREAD

#include "Type.h"

#endif // MISTPlatform_h
