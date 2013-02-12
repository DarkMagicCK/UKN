//
//  GLPreq.h
//  Project Unknown
//
//  Created by Robert Bu on 12/2/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#include "mist/Platform.h"
// use gl3.2 profile means there's no fixed pipeline functions unless use compability context
// #define UKN_REQUEST_OPENGL_3_2_PROFILE


// static link
#define GLEW_STATIC

#include "glew/glew.h"
	
// check opengl profile availability
	#if !defined(GLEW_VERSION_2_1)
		#error ukn need at least opengl 2.1 profile support to run
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

#pragma comment(lib, "glfw.lib")

#pragma comment(lib, "OpenGL32.lib")
#pragma comment(lib, "glu32.lib")

#elif defined(MIST_OS_OSX)

	#include <Availability.h>

	#if MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_7
        #include <OpenGL/gl3.h>
        #include <OpenGL/OpenGL.h>
        #include <OpenGL/glext.h>

		#define UKN_APPLE_OSX_LION
		#define UKN_OPENGL_3_2
		#define UKN_OPENGL_VERSION 32

        #define UKN_OPENGL_VERSION_MAJOR 3
        #define UKN_OPENGL_VERSION_MINOR 2
	#else
		#define UKN_OPENGL_VERSION 21

        #include <OpenGL/gl.h>
        #include <OpenGL/OpenGL.h>
        #include <OpenGL/glext.h>

        #define UKN_OPENGL_VERSION_MAJOR 2
        #define UKN_OPENGL_VERSION_MINOR 1
	#endif

#elif defined(MIST_OS_LINUX) // linux still need workaround
	#include <GL/gl.h>
	#include <GL/glu.h>
	#include <GL/glext.h>
#endif

#include "glfw/glfw3.h"

