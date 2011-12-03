//
//  GLPreq.h
//  Project Unknown
//
//  Created by Ruiwei Bu on 12/2/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifdef UKN_OS_WINDOWS
#include <GL/GL.h>
#include <GL/GLU.h>

#pragma comment(lib, "glfw.lib")

#elif defined(UKN_OS_OSX)
#include <OpenGL/OpenGL.h>
#include <OpenGL/glext.h>

#include <Availability.h>
#if MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_7
#include <OpenGL/gl3.h>
#define APPLE_OSX_LION
#endif

#elif defined(UKN_OS_LINUX)
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>
#endif

#include "glfw/glfw3.h"

