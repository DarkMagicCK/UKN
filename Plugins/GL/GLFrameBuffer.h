//
//  GLFrameBuffer.h
//  Project Unknown
//
//  Created by Robert Bu on 12/2/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_GLFrameBuffer_h
#define Project_Unknown_GLFrameBuffer_h

#include "UKN/FrameBuffer.h"
#include "GLPreq.h"

namespace ukn {
    
    class GLFrameBuffer: public FrameBuffer {
    public:
        GLFrameBuffer(bool offscreen);
        virtual ~GLFrameBuffer();
        
        bool requiresFlipping() const;
        
        void clear(uint32 flags, const class Color& clr, float depth, int32 stencil);
        
        GLuint getGLFBO() const;
        void swapBuffers();
        
    private:
        GLuint mFBO;
        
        bool mOffscreen;
        
        friend class GLWindow;
    };
    
    typedef SharedPtr<GLFrameBuffer> GLFrameBufferPtr;
    
    
} // namespace ukn

#endif