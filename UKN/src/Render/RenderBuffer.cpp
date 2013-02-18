//
//  RenderBuffer.cpp
//  Project Unknown
//
//  Created by Robert Bu on 11/23/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#include "UKN/RenderBuffer.h"
#include "UKN/GraphicBuffer.h"
#include "UKN/Shader.h"

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
    mUseIndexStream(false),
    mIndexStream(GraphicBufferPtr()),
    mIndexStartIndex(0),
    mIndexCount(0),
    mEffect(EffectPtr()) {
        
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
    
    void RenderBuffer::bindVertexStream(GraphicBufferPtr vertexStream, const vertex_elements_type& format) {
        mVertexStream = vertexStream;
        mVertexFormat = format;

		this->onBindVertexStream(vertexStream, format);
    }
    
    const vertex_elements_type& RenderBuffer::getVertexFormat() const {
        return mVertexFormat;
    }
    
    void RenderBuffer::setVertexFormat(const vertex_elements_type& format) {
        mVertexFormat = format;

		this->onSetVertexFormat(format);
    }
    
    uint32 RenderBuffer::getVertexCount() const {
        if(mVertexCount == 0 && mVertexStream) {
            return mVertexStream->count();
        }
        return mVertexCount;
    }
    
    void RenderBuffer::setVertexCount(uint32 count) {
        mVertexCount = count;

		this->onSetVertexCount(count);
    }
    
    uint32 RenderBuffer::getVertexStartIndex() const {
        return mVertexStartIndex;
    }
    
    void RenderBuffer::setVertexStartIndex(uint32 index) {
        mVertexStartIndex = index;

		this->setVertexStartIndex(index);
    }
    
    GraphicBufferPtr RenderBuffer::getIndexStream() const {
        return mIndexStream;
    }
    
    void RenderBuffer::bindIndexStream(GraphicBufferPtr indexStream) {
        mIndexStream = indexStream;

		this->onBindIndexStream(indexStream);
    }
    
    uint32 RenderBuffer::getIndexCount() const {
        if(mIndexCount == 0 && mIndexStream) {
            return mIndexStream->count();
        }
        return mIndexCount;
    }
    
    void RenderBuffer::setIndexCount(uint32 count) {
        mIndexCount = count;

		this->onSetIndexCount(count);
    }
    
    uint32 RenderBuffer::getIndexStartIndex() const {
        return mIndexStartIndex;
    }
    
    void RenderBuffer::setIndexStartIndex(uint32 index) {
        mIndexStartIndex = index;

		this->onSetIndexStartIndex(index);
    }
    
    void RenderBuffer::useIndexStream(bool flag) {
        mUseIndexStream = flag;

		this->onUseIndexStream(flag);
    }
    
    bool RenderBuffer::isUseIndexStream() const {
        return mUseIndexStream;
    }
    
	void RenderBuffer::onBindVertexStream(GraphicBufferPtr vertexStream, const vertex_elements_type& format) {

	}

	void RenderBuffer::onSetVertexFormat(const vertex_elements_type& format) {

	}

	void RenderBuffer::onBindIndexStream(GraphicBufferPtr indexStream) {

	}

	void RenderBuffer::onSetIndexCount(uint32 count) {

	}

	void RenderBuffer::onSetVertexCount(uint32 count) {

	}

	void RenderBuffer::onSetIndexStartIndex(uint32 index) {

	}

	void RenderBuffer::onUseIndexStream(bool flag) {

	}

    void RenderBuffer::onSetEffect(const EffectPtr& effect) {

    }

    void RenderBuffer::setEffect(const EffectPtr& effect) {
        mEffect = effect;
        this->onSetEffect(mEffect);
    }

    EffectPtr RenderBuffer::getEffect() const {
        return mEffect;
    }

} // namespace ukn
