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

namespace ukn {
    
    class GLFrameBuffer: public FrameBuffer {
    public:
        const ukn_string& description() const;
        bool requiresFlipping() const;
        
        void clear(uint32 flags, const class Color& clr, float depth, int32 stencil);
        void onBind();
        void onUnbind();
    };
    
    
} // namespace ukn

#endif
