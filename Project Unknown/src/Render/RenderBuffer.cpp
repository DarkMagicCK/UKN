//
//  RenderBuffer.cpp
//  Project Unknown
//
//  Created by Ruiwei Bu on 11/23/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#include "UKN/RenderBuffer.h"
#include "UKN/GraphicBuffer.h"

namespace ukn {
    
    class NullRenderBuffer: public RenderBuffer {
    public:
        NullRenderBuffer() {
            
        }
        
        virtual ~NullRenderBuffer() {
            
        }
    };
    
    RenderBufferPtr RenderBuffer::NullObject() {
        static RenderBufferPtr static_ptr = MakeSharedPtr<NullRenderBuffer>();
        return static_ptr;
    }
    
    RenderBuffer::RenderBuffer():
    mRenderMode(RM_Triangle),
    mVertexStream(GraphicBufferPtr()),
    mVertexStartIndex(0),
    mVertexCount(0),
    mVertexFormat(VertexFormat()),
    mUseIndexStream(false),
    mIndexStream(GraphicBufferPtr()),
    mIndexStartIndex(0),
    mIndexCount(0) {
        
    }
    
    RenderBuffer::~RenderBuffer() {
        
    }
    
    RenderMode RenderBuffer::getRenderMode() const {
        return mRenderMode;
    }
    
    void RenderBuffer::setRenderMode(RenderMode mode) {
        mRenderMode = mode;
    }

    GraphicBufferPtr RenderBuffer::getVertexStream() const {
        return mVertexStream;
    }
    
    void RenderBuffer::bindVertexStream(GraphicBufferPtr vertexStream, const VertexFormat& format) {
        mVertexStream = vertexStream;
        mVertexFormat = format;
    }
    
    VertexFormat RenderBuffer::getVertexFormat() const {
        return mVertexFormat;
    }
    
    void RenderBuffer::setVertexFormat(const VertexFormat& format) {
        mVertexFormat = format;
    }
    
    uint32 RenderBuffer::getVertexCount() const {
        return mVertexCount;
    }
    
    void RenderBuffer::setVertexCount(uint32 count) {
        mVertexCount = count;
    }
    
    uint32 RenderBuffer::getVertexStartIndex() const {
        return mVertexStartIndex;
    }
    
    void RenderBuffer::setVertexStartIndex(uint32 index) {
        mVertexStartIndex = index;
    }
    
    GraphicBufferPtr RenderBuffer::getIndexStream() const {
        return mIndexStream;
    }
    
    void RenderBuffer::bindIndexStream(GraphicBufferPtr indexStream) {
        mIndexStream = indexStream;
    }
    
    uint32 RenderBuffer::getIndexCount() const {
        return mIndexCount;
    }
    
    void RenderBuffer::setIndexCount(uint32 count) {
        mIndexCount = count;
    }
    
    uint32 RenderBuffer::getIndexStartIndex() const {
        return mIndexStartIndex;
    }
    
    void RenderBuffer::setIndexStartIndex(uint32 index) {
        mIndexStartIndex = index;
    }
    
    void RenderBuffer::useIndexStream(bool flag) {
        mUseIndexStream = flag;
    }
    
    bool RenderBuffer::isUseIndexStream() const {
        return mUseIndexStream;
    }
    
} // namespace ukn
