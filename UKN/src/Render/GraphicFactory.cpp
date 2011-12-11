//
//  GraphicFactory.cpp
//  Project Unknown
//
//  Created by Robert Bu on 11/24/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#include "UKN/GraphicFactory.h"
#include "UKN/GraphicBuffer.h"
#include "UKN/Texture.h"
#include "UKN/RenderView.h"
#include "UKN/FrameBuffer.h"
#include "UKN/RenderBuffer.h"
#include "UKN/GraphicDevice.h"
#include "UKN/SpriteBatch.h"

namespace ukn {
    
    class NullGraphicFactory: public GraphicFactory {
    public:
        NullGraphicFactory() {
            
        }
        
        ~NullGraphicFactory() {
            
        }
        
        GraphicDevice& getGraphicDevice() const {
            return *GraphicDevice::NullObject();
        }
        
        GraphicBufferPtr createVertexBuffer(GraphicBuffer::Access, 
                                            GraphicBuffer::Usage, 
                                            uint32 count, 
                                            const void* initialData, 
                                            const VertexFormat& format) {
            return GraphicBuffer::NullObject();
        }
        
        GraphicBufferPtr createIndexBuffer(GraphicBuffer::Access, 
                                           GraphicBuffer::Usage, 
                                           uint32 count, 
                                           const void* initialData) {
            return GraphicBuffer::NullObject();
        }
        
        
        RenderBufferPtr createRenderBuffer() {
            return RenderBuffer::NullObject();
        }
        
        RenderViewPtr create2DRenderView(TexturePtr texture) {
            return RenderView::NullObject();
        }
        
        RenderViewPtr create2DDepthStencilView(TexturePtr texture) {
            return RenderView::NullObject();
        }
        
        FrameBufferPtr createFrameBuffer() {
            return FrameBuffer::NullObject();
        }
        
        TexturePtr create2DTexture(uint32 width, uint32 height, uint32 numMipmaps, ElementFormat format, const uint8* initialData) {
            return Texture::NullObject();
        }
        
        TexturePtr load2DTexture(const ResourcePtr& rsrc, bool generateMipmaps=false) {
            return Texture::NullObject();
        }
        
    };
    
    GraphicFactoryPtr GraphicFactory::NullObject() {
        static GraphicFactoryPtr static_ptr = MakeSharedPtr<NullGraphicFactory>();
        return static_ptr;
    }
    
    GraphicFactory::GraphicFactory() { 
    
    }
    
    GraphicFactory::~GraphicFactory() { 
   
    }
    
    SpriteBatchPtr GraphicFactory::createSpriteBatch() const {
        return SpriteBatchPtr(new SpriteBatch());
    }
     
} // namespace ukn
