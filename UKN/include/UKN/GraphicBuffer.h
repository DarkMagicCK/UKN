//
//  GraphicBuffer.h
//  Project Unknown
//
//  Created by Robert Bu on 11/22/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_GraphicBuffer_h
#define Project_Unknown_GraphicBuffer_h

#include "UKN/Platform.h"
#include "UKN/PreDeclare.h"

#include "UKN/Vertex.h"

namespace ukn {
    
    /**
     * Abstract base class of a graphic buffer
     * such as VertexBuffer
     **/
    class UKN_API GraphicBuffer {
    public:        
        enum Access {
            ReadOnly,
            WriteOnly,
            ReadWrite,
        };
        
        enum Usage {
            Static,     // GL_STATIC_DRAW etc
            Dynamic,
        };
        
        static GraphicBufferPtr NullObject();
        
        GraphicBuffer(Access a, Usage u);
        
        virtual ~GraphicBuffer();
        
        virtual void*   map() = 0;
        virtual void    unmap() = 0;
        
        virtual void    activate() = 0;
        
		virtual uint32  count() const = 0;
        
        virtual void resize(uint32 desired_count) = 0;
        
        Usage   usage() const;
        Access  access() const;
        
    private:
        Access mAccessHint;
        Usage mUsage;
    };
    
} // namespace ukn

#endif
