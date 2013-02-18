//
//  GLPreq.h
//  Project Unknown
//
//  Created by Robert Bu on 12/2/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#include "mist/Platform.h"

// use gl3.2 profile means there's no fixed pipeline functions unless use compability context
// and since glew will not work for core profile
// ***** THIS WILL NOT WORK, just a flag to test the code is core compatile or not ******
// #define UKN_OSX_REQUEST_OPENGL_32_CORE_PROFILE

#ifndef UKN_OSX_REQUEST_OPENGL_32_CORE_PROFILE
// static link
#define GLEW_STATIC

#if defined(UKN_OSX_REQUEST_OPENGL_32_CORE_PROFILE) && defined(MIST_OS_OSX)
    // glew includes glu which includes <OpenGL/gl.h> in osx
    // that causes problem
    #define GLEW_NO_GLU
#endif

#include "glew/glew.h"

#endif

#ifndef MIST_OS_OSX

#if defined(GLEW_VERSION_4_2)
#define UKN_OPENGL_VERSION_MAJOR 4
#define UKN_OPENGL_VERSION_MINOR 2

#elif defined(GLEW_VERSION_4_1)
#define UKN_OPENGL_VERSION_MAJOR 4
#define UKN_OPENGL_VERSION_MINOR 1

#elif defined(GLEW_VERSION_4_0)
#define UKN_OPENGL_VERSION_MAJOR 4
#define UKN_OPENGL_VERSION_MINOR 0

#elif defined(GLEW_VERSION_3_3)
#define UKN_OPENGL_VERSION_MAJOR 3
#define UKN_OPENGL_VERSION_MINOR 3

#elif defined(GLEW_VERSION_3_2)
#define UKN_OPENGL_VERSION_MAJOR 3
#define UKN_OPENGL_VERSION_MINOR 2

#elif defined(GLEW_VERSION_2_1)
#define UKN_OPENGL_VERSION_MAJOR 2
#define UKN_OPENGL_VERSION_MINOR 1

#endif

#endif

#ifdef MIST_OS_WINDOWS
// glew for higher opengl profile extensions


#include <GL/GL.h>
#include <GL/GLU.h>

#pragma comment(lib, "glfw3.lib")

#pragma comment(lib, "OpenGL32.lib")
#pragma comment(lib, "glu32.lib")


#elif defined(MIST_OS_OSX)

	#include <Availability.h>

    #if defined(UKN_OSX_REQUEST_OPENGL_32_CORE_PROFILE)
        #if MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_7
            #include <OpenGL/gl3.h>
            #include <OpenGL/gl3ext.h>

            #define UKN_OPENGL_3_2
            #define UKN_OPENGL_VERSION 32

            #define UKN_OPENGL_VERSION_MAJOR 3
            #define UKN_OPENGL_VERSION_MINOR 2

            // opengl 3.2 core profile
            #define GLFW_INCLUDE_GLCOREARB
            
        #else
            #error opengl 3.2 core profile requires osx lion 10.7+
        #endif
	#else
		#define UKN_OPENGL_VERSION 21

        #include <OpenGL/gl.h>
        #include <OpenGL/glext.h>

        #define UKN_OPENGL_VERSION_MAJOR 2
        #define UKN_OPENGL_VERSION_MINOR 1
	#endif

#elif defined(MIST_OS_LINUX) // linux still need workaround
	#include <GL/gl.h>
	#include <GL/glu.h>
	#include <GL/glext.h>
#endif


// check opengl profile availability
#if UKN_OPENGL_VERSION_MAJOR < 2
#error ukn need at least opengl 2.1 profile support to run
#endif



#include "glfw/glfw3.h"

