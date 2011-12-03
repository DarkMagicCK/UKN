//
//  GLFrameBuffer.h
//  Project Unknown
//
//  Created by Ruiwei Bu on 12/2/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_GLFrameBuffer_h
#define Project_Unknown_GLFrameBuffer_h

#include "UKN/FrameBuffer.h"

#include "glfw/glfw3.h"

namespace ukn {
    
    class GLFrameBuffer: public FrameBuffer {
    public:
        GLFrameBuffer(bool offscreen);
        
        bool requiresFlipping() const;
        
        void clear(uint32 flags, const class Color& clr, float depth, int32 stencil);
        
        GLuint getGLFBO() const;
        
    private:
        GLuint mFBO;
        
        bool mOffscreen;
    };
    
    
} // namespace ukn

#endif
