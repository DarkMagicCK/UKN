//
//  GLRenderDevice.cpp
//  Project Unknown
//
//  Created by Ruiwei Bu on 11/24/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#include "GLGraphicDevice.h"


#ifdef UKN_OS_WINDOWS
#include <GL/GL.h>
#include <GL/GLU.h>

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

namespace ukn {
    
    GLGraphicDevice::GLGraphicDevice() {
        
    }
    
    GLGraphicDevice::~GLGraphicDevice() {
        
    }
    
    const ukn_wstring& GLGraphicDevice::description() const {
        static ukn_wstring des(L"OpenGL GraphicDevice");
        return des;
    }
    
    WindowPtr GLGraphicDevice::createRenderWindow(const ukn_string& name, const RenderSettings& settings) {
        return MakeSharedPtr<GLWindow>(name, settings);
    }
    
    void GLGraphicDevice::onRenderBuffer(const RenderBufferPtr& buffer) {
        
    }
    
}