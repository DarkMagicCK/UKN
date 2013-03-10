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
    mVertexStartIndex(0),
    mVertexCount(0),
    mUseIndexStream(false),
    mIndexStream(GraphicBufferPtr()),
    mIndexStartIndex(0),
    mIndexCount(0),
    mInstanceStartIndex(0) {
        
    }
    
    RenderBuffer::~RenderBuffer() {
        
    }
    
    RenderMode RenderBuffer::getRenderMode() const {
        return mRenderMode;
    }
    
    void RenderBuffer::setRenderMode(RenderMode mode) {
        mRenderMode = mode;
    }

    const GraphicBufferPtr& RenderBuffer::getVertexStream(uint32 index) const {
        if(index < mVertexStreams.size())
            return mVertexStreams[index].stream;
        return StaticEmptySharedPtr<GraphicBuffer>();
    }
    
    void RenderBuffer::bindVertexStream(const GraphicBufferPtr& vertexStream, const vertex_elements_type& format) {
        for(StreamObject& so: mVertexStreams) {
            if(so.format == format) {
                so.stream = vertexStream;
                so.vertex_size = GetVertexElementsTotalSize(format);
                so.type = VST_Geometry;

		        return;
            }
        }
        StreamObject so;
        so.stream = vertexStream;
        so.format = format;
        so.vertex_size = GetVertexElementsTotalSize(format);
        so.type = VST_Geometry;
       
        mVertexStreams.push_back(so);

		this->onBindVertexStream(vertexStream, format, VST_Geometry);
    }
    
    vertex_elements_type RenderBuffer::getVertexFormat(uint32 index) const {
        if(index < mVertexStreams.size())
            return mVertexStreams[index].format;
        return vertex_elements_type();
    }
    
    void RenderBuffer::setVertexFormat(const vertex_elements_type& format, uint32 index) {
        if(index < mVertexStreams.size()) {
            mVertexStreams[index].format = format;
            mVertexStreams[index].vertex_size = GetVertexElementsTotalSize(format);
            
    		this->onSetVertexFormat(format, index);
        } else {
            log_error(L"RenderBuffer::setVertexFormat: invalid index");
        }
    }
    
    uint32 RenderBuffer::getVertexCount() const {
        if(mVertexCount == 0 && !mVertexStreams.empty()) {
            return mVertexStreams[0].stream->count();
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
    }
    
    const GraphicBufferPtr& RenderBuffer::getIndexStream() const {
        return mIndexStream;
    }
    
    void RenderBuffer::bindIndexStream(const GraphicBufferPtr& indexStream) {
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

    uint32 RenderBuffer::getInstanceCount() const {
        return mInstanceCount;
    }

    void RenderBuffer::setInstanceCount(uint32 count) {
        mInstanceCount = count;
    }

    void RenderBuffer::bindInstanceStream(const GraphicBufferPtr& instanceStream, const vertex_elements_type& format) {
        mInstanceStream.stream = instanceStream;
        mInstanceStream.format = format;
        mInstanceStream.vertex_size = GetVertexElementsTotalSize(format);
        mInstanceStream.type = VST_Instance;
    }

    uint32 RenderBuffer::getInstanceStartIndex() const {
        return mInstanceStartIndex;
    }

    void RenderBuffer::setInstanceStartIndex(uint32 index) {
        mInstanceStartIndex = index;
    }

    const RenderBuffer::VertexStreamVec& RenderBuffer::getVertexStreams() const {
        return mVertexStreams;
    }

    const GraphicBufferPtr& RenderBuffer::getInstanceStream() const {
        return mInstanceStream.stream;
    }

    uint32 RenderBuffer::getInstanceFormatSize() const {
        return mInstanceStream.vertex_size;
    }

    bool RenderBuffer::hasInstanceStream() const {
        return mInstanceStream.stream && mInstanceCount > 0;
    }

    const vertex_elements_type& RenderBuffer::getInstanceFormat() const {
        return mInstanceStream.format;
    }
    
	void RenderBuffer::onBindVertexStream(const GraphicBufferPtr& vertexStream, const vertex_elements_type& format, VertexStreamType type) {

	}

	void RenderBuffer::onSetVertexFormat(const vertex_elements_type& format, uint32 index) {

	}

	void RenderBuffer::onBindIndexStream(const GraphicBufferPtr& indexStream) {

	}

	void RenderBuffer::onSetIndexCount(uint32 count) {

	}

	void RenderBuffer::onSetVertexCount(uint32 count) {

	}

	void RenderBuffer::onSetIndexStartIndex(uint32 index) {

	}

	void RenderBuffer::onUseIndexStream(bool flag) {

	}

} // namespace ukn
