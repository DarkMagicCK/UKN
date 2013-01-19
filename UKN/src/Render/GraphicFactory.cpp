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
#include "UKN/Shader.h"

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
                                            const VertexFormat& format) const {
            return GraphicBuffer::NullObject();
        }
        
        GraphicBufferPtr createIndexBuffer(GraphicBuffer::Access, 
                                           GraphicBuffer::Usage, 
                                           uint32 count, 
                                           const void* initialData) const {
            return GraphicBuffer::NullObject();
        }
        
        
        RenderBufferPtr createRenderBuffer() const {
            return RenderBuffer::NullObject();
        }
        
        RenderViewPtr create2DRenderView(TexturePtr texture) const {
            return RenderView::NullObject();
        }
        
        RenderViewPtr create2DDepthStencilView(TexturePtr texture) const {
            return RenderView::NullObject();
        }
        
        FrameBufferPtr createFrameBuffer() const {
            return FrameBuffer::NullObject();
        }
        
        TexturePtr create2DTexture(uint32 width, uint32 height, uint32 numMipmaps, ElementFormat format, const uint8* initialData) const {
            return Texture::NullObject();
        }
        
        TexturePtr load2DTexture(const ResourcePtr& rsrc, bool generateMipmaps=false) const {
            return Texture::NullObject();
        }

        EffectPtr loadEffect(const ResourcePtr& rsrc) const {
            return Effect::NullObject();
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
