//
//  GLGraphicBuffer.h
//  Project Unknown
//
//  Created by Robert Bu on 12/7/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_GLGraphicBuffer_h
#define Project_Unknown_GLGraphicBuffer_h

#include "mist/Platform.h"
#include "UKN/GraphicBuffer.h"
#include "UKN/Vertex.h"
#include "GLPreq.h"

namespace ukn {

    class GLVertexBuffer: public GraphicBuffer {
    public:
        GLVertexBuffer(GraphicBuffer::Access access,
                       GraphicBuffer::Usage usage,
                       uint32 desired_count,
                       const void* initData,
                       const vertex_elements_type& format);

        virtual ~GLVertexBuffer();

        virtual void* map();
        virtual void unmap();

        virtual void activate();
        virtual void deactivate();
        virtual bool isInMemory() const;

        vertex_elements_type& format();
        const vertex_elements_type& format() const;

        uint32 count() const;
        void resize(uint32 desired_count);

    private:
        GLuint mId;

        bool mMaped;
        uint32 mCount;
        vertex_elements_type mFormat;
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
        virtual void deactivate();
        virtual bool isInMemory() const;
        
        uint32 count() const;
        void resize(uint32 desired_count);

    private:
        GLuint mId;
        
        bool mMaped;
        uint32 mCount;
    };


} // namespace ukn

#endif
