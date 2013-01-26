//
//  GLGraphicFactory.cpp
//  Project Unknown
//
//  Created by Robert Bu on 11/24/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#include "GLGraphicFactory.h"
#include "GLGraphicDevice.h"

#include "UKN/GraphicFactory.h"
#include "UKN/RenderBuffer.h"
#include "UKN/RenderView.h"
#include "UKN/FrameBuffer.h"
#include "UKN/Texture.h"
#include "UKN/Shader.h"

#include "GLTexture.h"
#include "GLRenderBuffer.h"
#include "GLFrameBuffer.h"
#include "GLGraphicBuffer.h"
#include "GLSpriteBatch.h"
#include "GLRenderView.h"

namespace ukn {
    
    class GLGraphicFactory: public GraphicFactory {
    public:
        GLGraphicFactory();
        virtual ~GLGraphicFactory();
        
        GraphicDevice& getGraphicDevice() const;
        
        SpriteBatchPtr createSpriteBatch() const;
        
        GraphicBufferPtr createVertexBuffer(GraphicBuffer::Access, 
                                            GraphicBuffer::Usage, 
                                            uint32 count, 
                                            const void* initialData, 
                                            const VertexFormat& format) const;
        
        GraphicBufferPtr createIndexBuffer(GraphicBuffer::Access, 
                                           GraphicBuffer::Usage, 
                                           uint32 count, 
                                           const void* initialData) const;
        
        
        RenderBufferPtr createRenderBuffer() const;
        
        RenderViewPtr create2DRenderView(TexturePtr texture) const;
        RenderViewPtr create2DDepthStencilView(TexturePtr texture) const;
        
        FrameBufferPtr createFrameBuffer() const;
        
        TexturePtr create2DTexture(uint32 width, uint32 height, uint32 numMipmaps, ElementFormat format, const uint8* initialData) const;
        
        TexturePtr load2DTexture(const ResourcePtr& rsrc, bool generateMipmaps=false) const;
        EffectPtr createEffect() const;

    private:
        GraphicDevicePtr mGraphicDevice;
    };
    
    extern "C" {
        UKN_GL_API void CreateGraphicFactory(GraphicFactoryPtr& ptr) {
            static GraphicFactoryPtr static_ptr = MakeSharedPtr<GLGraphicFactory>();
            ptr = static_ptr;
        }
    }
    
    
    GLGraphicFactory::GLGraphicFactory() {
        mGraphicDevice = MakeSharedPtr<GLGraphicDevice>();
    }
    
    GLGraphicFactory::~GLGraphicFactory() {
        
    }
    
    GraphicDevice& GLGraphicFactory::getGraphicDevice() const {
        return *mGraphicDevice;
    }
    
    SpriteBatchPtr GLGraphicFactory::createSpriteBatch() const {
        return SpriteBatchPtr(new GLSpriteBatch());
    }
    
    GraphicBufferPtr GLGraphicFactory::createVertexBuffer(GraphicBuffer::Access access, 
                                                          GraphicBuffer::Usage usage,
                                                          uint32 count, 
                                                          const void* initialData, 
                                                          const VertexFormat& format) const {
        return GraphicBufferPtr(new GLVertexBuffer(access,
                                                   usage,
                                                   count,
                                                   initialData,
                                                   format));
    }
    
    GraphicBufferPtr GLGraphicFactory::createIndexBuffer(GraphicBuffer::Access access, 
                                                         GraphicBuffer::Usage usage, 
                                                         uint32 count, 
                                                         const void* initialData) const {
        return GraphicBufferPtr(new GLIndexBuffer(access,
                                                  usage,
                                                  count,
                                                  initialData));
    }
    
    
    RenderBufferPtr GLGraphicFactory::createRenderBuffer() const {
        return RenderBufferPtr(new GLRenderBuffer());
    }
    
    RenderViewPtr GLGraphicFactory::create2DRenderView(TexturePtr texture) const {
        return new GLTexture2DRenderView(*texture.get(), 0, 0);
    }
    
    RenderViewPtr GLGraphicFactory::create2DDepthStencilView(TexturePtr texture) const {
        return RenderView::NullObject();
    }
    
    FrameBufferPtr GLGraphicFactory::createFrameBuffer() const {
        return FrameBufferPtr(new GLFrameBuffer(true));
    }
    
    TexturePtr GLGraphicFactory::create2DTexture(uint32 width, uint32 height, uint32 numMipmaps, ElementFormat format, const uint8* initialData) const {
        GLTexture2D* texture = new GLTexture2D();
        if(texture->create(width, height, numMipmaps, format, initialData)) {
            return TexturePtr(texture);
        }
		delete texture;
        return TexturePtr();
    }
    
    TexturePtr GLGraphicFactory::load2DTexture(const ResourcePtr& rsrc, bool generateMipmaps) const {
        GLTexture2D* texture = new GLTexture2D();
        if(texture->load(rsrc, generateMipmaps)) {
            return TexturePtr(texture);
        }
		delete texture;
        return TexturePtr();
    }

    EffectPtr GLGraphicFactory::createEffect() const {
        return EffectPtr();
    }
    
} // namespace ukn
