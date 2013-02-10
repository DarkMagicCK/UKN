//
//  GLFrameBuffer.cpp
//  Project Unknown
//
//  Created by Robert Bu on 12/2/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#include "GLFrameBuffer.h"
#include "GLPreq.h"
#include "GLConvert.h"
#include "GLGraphicDevice.h"
#include "GLGraphicFactory.h"
#include "ukn/Context.h"

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
    
    SharedPtr<uint8> GLFrameBuffer::readFrameBufferData(int32 x, int32 y, uint32 width, uint32 height, ElementFormat format) {
        GLint prevBuffer;
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevBuffer);
        
        glBindBuffer(GL_FRAMEBUFFER_EXT, mFBO);
        
        if(width == 0)
            width = this->getViewport().width;
        if(height == 0)
            height = this->getViewport().height;
        
        uint8* texData = new uint8[width * height * GetElementSize(format)];
        glReadPixels(x,
                     y,
                     width,
                     height,
                     element_format_to_gl_format(format),
                     element_format_to_gl_element_type(format),
                     texData);
        
        if(glGetError() != GL_NO_ERROR) {
            log_error("GLGraphicDevice: error when read pixels");
        }
        
        glBindBuffer(GL_FRAMEBUFFER_EXT, prevBuffer);
        
        return SharedPtr<uint8>(texData);
    }
    
    void GLFrameBuffer::clear(uint32 flags, const class Color& clr, float depth, int32 stencil) {
        GLbitfield glflags = 0;
        
        if(flags & CM_Color) {
            glClearColor(clr.r(), clr.g(), clr.b(), clr.a());
            
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
    
    void GLFrameBuffer::onBind() {
        GLGraphicDevice& gd = *checked_cast<GLGraphicDevice*>(&Context::Instance().getGraphicFactory().getGraphicDevice());
        
        gd.bindGLFrameBuffer(mFBO);

    }
    
    void GLFrameBuffer::onUnbind() {
        GLGraphicDevice& gd = *checked_cast<GLGraphicDevice*>(&Context::Instance().getGraphicFactory().getGraphicDevice());
        
        gd.bindGLFrameBuffer(0);
    }
    
    GLuint GLFrameBuffer::getGLFBO() const {
        return mFBO;
    }
    
} // namespace ukn