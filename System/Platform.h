/*
 *  Platform.h
 *  Base
 *
 *  Created by Brent Wilson on 12/22/09.
 *  Copyright 2009 Brent Wilson. All rights reserved.
 *
 */

#ifndef _PLATFORM_H_
#define _PLATFORM_H_

// Defines our possible states.
#define PLATFORM_APPLE 0
#define PLATFORM_LINUX 1
#define PLATFORM_WIN32 2

#define COMPILER_GNUC 0
#define COMPILER_MSVC 1

// Finds the compiler type and version.
#if defined( _MSC_VER )
#   define SYS_COMPILER OGRE_COMPILER_MSVC
#   define SYS_COMPILER_VER _MSC_VER
#elif defined( __GNUC__ )
#   define SYS_COMPILER OGRE_COMPILER_GNUC
#   define SYS_COMPILER_VER (((__GNUC__) * 100) + (__GNUC_MINOR__ * 10) + __GNUC_PATCHLEVEL__)
#else
#   error Unknown compiler!
#endif

// Finds the current platform.
#if defined(__APPLE__) && defined(__MACH__)
#   define SYS_PLATFORM PLATFORM_APPLE
#elif defined( __WIN32__ ) || defined( _WIN32 )
#   define SYS_PLATFORM PLATFORM_WIN32
#else
#   define SYS_PLATFORM PLATFORM_LINUX
#endif

#endif
