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
    mOffscreen(offscreen),
    mFBO(0) {
        if(mOffscreen) {
			//glGenBuffers(1, &mFBO); 
        }
    }
    
    bool GLFrameBuffer::requiresFlipping() const {
        return false;
    }
    
    void GLFrameBuffer::clear(uint32 flags, const class Color& clr, float depth, int32 stencil) {
        GLbitfield glflags = 0;
        
        if(flags & CM_Color) {
            glClearColor(clr.r, clr.g, clr.b, clr.a);
            
            glflags |= GL_COLOR_BUFFER_BIT;
        }
        if(flags & CM_Depth) {
            glDepthMask(true);
            glClearDepth(depth);
            
            glflags |= GL_DEPTH_BUFFER_BIT;
        }
        if(flags & CM_Stencil) {
            glClearStencil(stencil);
            
            glflags |= GL_STENCIL_BUFFER_BIT;
        }
        
        glClear(GL_COLOR_BUFFER_BIT);
    }
    
    GLuint GLFrameBuffer::getGLFBO() const {
        return mFBO;
    }
    
} // namespace ukn
