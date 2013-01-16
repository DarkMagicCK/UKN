//
//  RenderBuffer.h
//  Project Unknown
//
//  Created by Robert Bu on 11/22/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_RenderBuffer_h
#define Project_Unknown_RenderBuffer_h

#include "mist/Platform.h"
#include "UKN/PreDeclare.h"
#include "UKN/GraphicSettings.h"
#include "UKN/Vertex.h"

namespace ukn {
    
    /**
     * RenderBuffer Object
     * Represents a vertex stream/list and a index list to render
     **/
    
    class UKN_API RenderBuffer {
    public:
        RenderBuffer();
        // force base class
        virtual ~RenderBuffer() = 0;
        
        static RenderBufferPtr NullObject();
        
        RenderMode          getRenderMode() const;
        void                setRenderMode(RenderMode mode);
        
        GraphicBufferPtr    getVertexStream() const;
        void                bindVertexStream(GraphicBufferPtr vertexStream, const VertexFormat& format);
        
        const VertexFormat& getVertexFormat() const;
        void                setVertexFormat(const VertexFormat& format);
        
        uint32              getVertexCount() const;
        void                setVertexCount(uint32 count);
        
        uint32              getVertexStartIndex() const;
        void                setVertexStartIndex(uint32 index);
        
        void                useIndexStream(bool flag);
        bool                isUseIndexStream() const;
        
        GraphicBufferPtr    getIndexStream() const;
        void                bindIndexStream(GraphicBufferPtr indexStream);
        
        uint32              getIndexCount() const;
        void                setIndexCount(uint32 count);
        
        uint32              getIndexStartIndex() const;
        void                setIndexStartIndex(uint32 index);

	protected:
		virtual void onBindVertexStream(GraphicBufferPtr vertexStream, const VertexFormat& format);
		virtual void onSetVertexFormat(const VertexFormat& format);
		virtual void onBindIndexStream(GraphicBufferPtr indexStream);
		virtual void onSetIndexCount(uint32 count);
		virtual void onSetVertexCount(uint32 count);
		virtual void onSetIndexStartIndex(uint32 index);
		virtual void onUseIndexStream(bool flag);
        
    private:
        RenderMode mRenderMode;
        
        GraphicBufferPtr mVertexStream;
        uint32 mVertexStartIndex;
        uint32 mVertexCount;
        VertexFormat mVertexFormat;
        
        bool mUseIndexStream;
        GraphicBufferPtr mIndexStream;
        uint32 mIndexStartIndex;
        uint32 mIndexCount;
    };

} // namespace ukn



#endif
