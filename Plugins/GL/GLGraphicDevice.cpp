//
//  GLRenderDevice.cpp
//  Project Unknown
//
//  Created by Robert Bu on 11/24/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#include "GLGraphicDevice.h"

#include "glfw/glfw3.h"

#ifdef UKN_OS_WINDOWS
#include <GL/GL.h>
#include <GL/GLU.h>

#pragma comment(lib, "glfw.lib")

#elif defined(UKN_OS_OSX)
#include <OpenGL/OpenGL.h>
#include <OpenGL/glext.h>

#elif defined(UKN_OS_LINUX)
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>
#endif

#include "GLWindow.h"
#include "UKN/Common.h"

namespace ukn {
    
    GLGraphicDevice::GLGraphicDevice() {
        
    }
    
    GLGraphicDevice::~GLGraphicDevice() {
        
    }
    
    ukn_string GLGraphicDevice::description() const {
        static ukn_string des = format_string("OpenGL Graphic Device\nOpenGL Version: %s Vender: %s GLSL Version: %s", 
                                              (char*)glGetString(GL_VERSION),
                                              (char*)glGetString(GL_VENDOR),
                                              (char*)glGetString(GL_SHADING_LANGUAGE_VERSION));;
        return des;
    }
    
    WindowPtr GLGraphicDevice::createRenderWindow(const ukn_string& name, const RenderSettings& settings) {
        WindowPtr window;
        try {
            window = MakeSharedPtr<GLWindow>(name, settings);
        } catch(Exception& e) {
            return WindowPtr();
        }
        return window;
    }
    
    void GLGraphicDevice::onRenderBuffer(const RenderBufferPtr& buffer) {
        
    }
    
}