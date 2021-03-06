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
#include "UKN/GraphicSettings.h"
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
#include "GLBlendStateObject.h"
#include "GLSamplerStateObject.h"
#include "GLRasterizerStateObject.h"
#include "GLDepthStencilStateObject.h"

#include "CgShader.h"

namespace ukn {
    
    class GLGraphicFactory: public GraphicFactory {
    public:
        GLGraphicFactory();
        virtual ~GLGraphicFactory();
        
        GraphicDevice& getGraphicDevice() const override;
        
        SpriteBatchPtr createSpriteBatch() const override;
        
        GraphicBufferPtr createVertexBuffer(GraphicBuffer::Access, 
                                            GraphicBuffer::Usage, 
                                            uint32 count, 
                                            const void* initialData, 
                                            const vertex_elements_type& format) const override;
        
        GraphicBufferPtr createIndexBuffer(GraphicBuffer::Access, 
                                           GraphicBuffer::Usage, 
                                           uint32 count, 
                                           const void* initialData) const;
        
        
        RenderBufferPtr createRenderBuffer() const override;
        
        RenderViewPtr createRenderView(const TexturePtr& texture) const override;
        RenderViewPtr createDepthStencilView(const TexturePtr& texture) const override;
        
        RenderViewPtr createDepthStencilView(uint32 width,
                                             uint32 height,
                                             ElementFormat format,
                                             uint32 sampleCount,
                                             uint32 sampleQuality) const override;
        
        FrameBufferPtr createFrameBuffer() const override;
        
        TexturePtr create2DTexture(uint32 width, 
                                   uint32 height, 
                                   uint32 numMipmaps, 
                                   ElementFormat format, 
                                   const uint8* initialData,
                                   uint32 bindFlag) const override;

        TexturePtr load2DTexture(const ResourcePtr& rsrc, bool generateMipmaps=false) const override;
        EffectPtr createEffect() const override;
        
        BlendStatePtr createBlendStateObject(const BlendStateDesc& desc) const override;
        SamplerStatePtr createSamplerStateObject(const SamplerStateDesc& desc) const override;
        RasterizerStatePtr createRasterizerStateObject(const RasterizerStateDesc& desc) const override;
        DepthStencilStatePtr createDepthStencilStateObject(const DepthStencilStateDesc& desc) const override;

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
                                                          const vertex_elements_type& format) const {
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
    
    RenderViewPtr GLGraphicFactory::createRenderView(const TexturePtr& texture) const {
        if(texture->type() == TT_Texture2D)
            return MakeSharedPtr<GLTexture2DRenderView>(texture, 0, 0);
        return RenderViewPtr();
    }
    
    RenderViewPtr GLGraphicFactory::createDepthStencilView(const TexturePtr& texture) const {
        if(texture->type() == TT_Texture2D)
            return MakeSharedPtr<GLDepthStencilView>(texture);
        return RenderViewPtr();
    }

    RenderViewPtr GLGraphicFactory::createDepthStencilView(uint32 width,
                                                           uint32 height,
                                                           ElementFormat format,
                                                           uint32 sampleCount,
                                                           uint32 sampleQuality) const {
        return MakeSharedPtr<GLDepthStencilView>(width, height, format, sampleCount, sampleQuality);
    }
    
    FrameBufferPtr GLGraphicFactory::createFrameBuffer() const {
        return FrameBufferPtr(new GLFrameBuffer(true));
    }
    
    TexturePtr GLGraphicFactory::create2DTexture(uint32 width, 
                                                 uint32 height, 
                                                 uint32 numMipmaps, 
                                                 ElementFormat format, 
                                                 const uint8* initialData,
                                                 uint32 flag) const {
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
        return MakeSharedPtr<CgGLEffect>((GLGraphicDevice*)mGraphicDevice.get());
    }
    
    BlendStatePtr GLGraphicFactory::createBlendStateObject(const BlendStateDesc& desc) const {
        return MakeSharedPtr<GLBlendStateObject>(desc);
    }
    
    SamplerStatePtr GLGraphicFactory::createSamplerStateObject(const SamplerStateDesc& desc) const {
        return MakeSharedPtr<GLSamplerStateObject>(desc);
    }

    RasterizerStatePtr GLGraphicFactory::createRasterizerStateObject(const RasterizerStateDesc& desc) const {
        return MakeSharedPtr<GLRasterizerStateObject>(desc);
    }

    DepthStencilStatePtr GLGraphicFactory::createDepthStencilStateObject(const DepthStencilStateDesc& desc) const {
        return MakeSharedPtr<GLDepthStencilStateObject>(desc);
    }
    
} // namespace ukn
