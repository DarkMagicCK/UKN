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

    enum VertexStreamType {
        VST_Geometry,
        // hardware instancing stream
        VST_Instance, 
    };
    
    class UKN_API RenderBuffer {
    public:
        struct StreamObject {
            StreamObject(): vertex_size(0), type(VST_Geometry) { }

            GraphicBufferPtr stream;
            vertex_elements_type format;
            uint32 vertex_size;
            VertexStreamType type;
        };
        typedef std::vector<StreamObject> VertexStreamVec;

    public:
        RenderBuffer();
        // force base class
        virtual ~RenderBuffer() = 0;
        
        static RenderBufferPtr NullObject();
        
        RenderMode          getRenderMode() const;
        void                setRenderMode(RenderMode mode);
        
        GraphicBufferPtr    getVertexStream(uint32 index = 0) const;
        void                bindVertexStream(const GraphicBufferPtr& vertexStream, const vertex_elements_type& format);
        
        const VertexStreamVec& getVertexStreams() const;

        void                setVertexStream(uint32 index, const GraphicBufferPtr& stream, const vertex_elements_type& format);
        void                removeVertexStream(uint32 index);

        vertex_elements_type getVertexFormat(uint32 index = 0) const;
        void                setVertexFormat(const vertex_elements_type& format, uint32 index = 0);
        
        uint32              getVertexCount() const;
        void                setVertexCount(uint32 count);
        
        uint32              getVertexStartIndex() const;
        void                setVertexStartIndex(uint32 index);
        
        void                useIndexStream(bool flag);
        bool                isUseIndexStream() const;
        
        GraphicBufferPtr    getIndexStream() const;
        void                bindIndexStream(const GraphicBufferPtr& indexStream);
        
        uint32              getIndexCount() const;
        void                setIndexCount(uint32 count);
        
        uint32              getIndexStartIndex() const;
        void                setIndexStartIndex(uint32 index);

        /* instancing */
        void    setInstanceCount(uint32 count);
        uint32  getInstanceCount() const;
        void    bindInstanceStream(const GraphicBufferPtr& instanceStream, const vertex_elements_type& format);

        uint32 getInstanceStartIndex() const;
        void   setInstanceStartIndex(uint32 index);

        bool    hasInstanceStream() const;
        const GraphicBufferPtr& getInstanceStream() const;
        uint32 getInstanceFormatSize() const;

	protected:
		virtual void onBindVertexStream(GraphicBufferPtr vertexStream, const vertex_elements_type& format, VertexStreamType type);
		virtual void onSetVertexFormat(const vertex_elements_type& format, uint32 index);
		virtual void onBindIndexStream(GraphicBufferPtr indexStream);
		virtual void onSetIndexCount(uint32 count);
		virtual void onSetVertexCount(uint32 count);
		virtual void onSetIndexStartIndex(uint32 index);
		virtual void onUseIndexStream(bool flag);

    protected:
        RenderMode mRenderMode;

        VertexStreamVec mVertexStreams;
        StreamObject mInstanceStream;
        
        uint32 mVertexStartIndex;
        uint32 mVertexCount;

        bool mUseIndexStream;
        GraphicBufferPtr mIndexStream;
        uint32 mIndexStartIndex;
        uint32 mIndexCount;

        uint32 mInstanceCount;
        uint32 mInstanceStartIndex;
    };

} // namespace ukn



#endif
