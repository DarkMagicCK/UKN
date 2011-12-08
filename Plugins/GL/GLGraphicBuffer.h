//
//  GLGraphicBuffer.h
//  Project Unknown
//
//  Created by Ruiwei Bu on 12/7/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_GLGraphicBuffer_h
#define Project_Unknown_GLGraphicBuffer_h

#include "UKN/Platform.h"
#include "UKN/GraphicBuffer.h"

namespace ukn {
    
    class GLVertexBuffer: public GraphicBuffer {
    public:
        GLVertexBuffer(GraphicBuffer::Access access, 
                       GraphicBuffer::Usage usage, 
                       uint32 desired_count,
                       const void* initData,
                       const VertexFormat& format);
        
        virtual ~GLVertexBuffer();
        
        virtual void* map();
        virtual void unmap();
        
        virtual void activate();
        
        VertexFormat& format();
        const VertexFormat& format() const;
        
        uint32 count() const;
        void resize(uint32 desired_count);
        
    private:
        GLuint mId;
        
        uint32 mCount;
        VertexFormat mFormat;
    };
    
    class GLIndexBuffer: public GraphicBuffer {
    public:
        GLIndexBuffer(GraphicBuffer::Access acess, 
                      GraphicBuffer::Usage usage, 
                      uint32 desired_count,
                      const void* initData);
        
        virtual ~GLIndexBuffer();
        
        virtual void* map();
        virtual void unmap();
        
        virtual void activate();
        
        uint32 count() const;
        void resize(uint32 desired_count);
        
    private:
        GLuint mId;
        
        uint32 mCount;
    };
    
    
} // namespace ukn

#endif
