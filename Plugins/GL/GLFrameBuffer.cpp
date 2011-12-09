//
//  GLFrameBuffer.cpp
//  Project Unknown
//
//  Created by Ruiwei Bu on 12/2/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#include "GLFrameBuffer.h"
#include "GLPreq.h"

namespace ukn {
    
    GLFrameBuffer::GLFrameBuffer(bool offscreen):
    mFBO(0),
    mOffscreen(offscreen) {
        if(mOffscreen) {
			glGenBuffers(1, &mFBO);
        }
    }
    
    GLFrameBuffer::~GLFrameBuffer() {
        
    }
    
    bool GLFrameBuffer::requiresFlipping() const {
        return false;
    }
    
    void GLFrameBuffer::swapBuffers() {
        glfwSwapBuffers();
    }
    
    void GLFrameBuffer::clear(uint32 flags, const class Color& clr, float depth, int32 stencil) {
        GLbitfield glflags = 0;
        
        if(flags & CM_Color) {
            glClearColor(clr.r, clr.g, clr.b, clr.a);
            
            glflags |= GL_COLOR_BUFFER_BIT;
        }
        if(flags & CM_Depth) {
            glClearDepth(depth);
            
            glflags |= GL_DEPTH_BUFFER_BIT;
        }
        if(flags & CM_Stencil) {
            glClearStencil(stencil);
            
            glflags |= GL_STENCIL_BUFFER_BIT;
        }
        
        glClear(glflags);
    }
    
    GLuint GLFrameBuffer::getGLFBO() const {
        return mFBO;
    }
    
} // namespace ukn