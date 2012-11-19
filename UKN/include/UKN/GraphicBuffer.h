//
//  GraphicBuffer.h
//  Project Unknown
//
//  Created by Robert Bu on 11/22/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_GraphicBuffer_h
#define Project_Unknown_GraphicBuffer_h

#include "mist/Platform.h"
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
        virtual void    deactivate() = 0;
        
		virtual uint32  count() const = 0;
        
        virtual void resize(uint32 desired_count) = 0;
        virtual bool isInMemory() const = 0;
        
        Usage   usage() const;
        Access  access() const;
        
        // set use count, default = count
        uint32 useCount() const;
        void   useCount(uint32 useCount);
        
    private:
        Access mAccessHint;
        Usage mUsage;
        
        uint32 mUseCount;
    };
    
    template<typename T>
    class UKN_API MemoryGraphicBuffer: public GraphicBuffer {
    public:
        typedef Array<T> container_type;
        
    public:
        MemoryGraphicBuffer(uint32 count, const T* initialData):
        GraphicBuffer(GraphicBuffer::ReadWrite, GraphicBuffer::Dynamic) {
            if(count > 0)
                mContainer.resize(count);
            if(initialData)
                mContainer.assign(initialData, initialData + count);
        }
        virtual ~MemoryGraphicBuffer() { }
        
        virtual void*   map() { return &mContainer[0]; }
        virtual void    unmap() { }
        
        virtual void    activate() { }
        virtual void    deactivate() { }
        
        virtual uint32  count() const { return (uint32)mContainer.size(); }
        
        virtual void resize(uint32 desired_count) { mContainer.resize(desired_count); }
        virtual bool isInMemory() const { return true; }
        
    public:
        MemoryGraphicBuffer& push(const T& t) { mContainer.push_back(t); return *this; }
        
    private:
        container_type mContainer;
    };
    
} // namespace ukn

#endif
