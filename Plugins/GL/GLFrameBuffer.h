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
    
    class GLWindow;
    
    class GLFrameBuffer: public FrameBuffer {
    public:
        GLFrameBuffer(bool offscreen);
        virtual ~GLFrameBuffer();
        
        bool requiresFlipping() const;
        
        void onBind();
        void onUnbind();
        void clear(uint32 flags, const class Color& clr, float depth, int32 stencil);
        
        GLuint getGLFBO() const;
        void swapBuffers();
        
        SharedPtr<uint8> readFrameBufferData(int32 x, int32 y, uint32 width, uint32 height, ElementFormat format);
        
    private:
        GLuint mFBO;
        bool mOffscreen;
        
        friend class GLWindow;
    };
    
    typedef SharedPtr<GLFrameBuffer> GLFrameBufferPtr;
    
    
} // namespace ukn

#endif