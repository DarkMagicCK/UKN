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
#include "GLBlendStateObject.h"
#include "GLSamplerStateObject.h"


#include "CgShader.h"

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
                                            const vertex_elements_type& format) const;
        
        GraphicBufferPtr createIndexBuffer(GraphicBuffer::Access, 
                                           GraphicBuffer::Usage, 
                                           uint32 count, 
                                           const void* initialData) const;
        
        
        RenderBufferPtr createRenderBuffer() const;
        
        RenderViewPtr createRenderView(const TexturePtr& texture) const;
        RenderViewPtr createDepthStencilView(const TexturePtr& texture) const;
        
        FrameBufferPtr createFrameBuffer() const;
        
        TexturePtr create2DTexture(uint32 width, uint32 height, uint32 numMipmaps, ElementFormat format, const uint8* initialData) const;
        
        TexturePtr load2DTexture(const ResourcePtr& rsrc, bool generateMipmaps=false) const;
        EffectPtr createEffect() const;
        
        BlendStatePtr createBlendStateObject(const BlendStateDesc& desc) const;
        SamplerStatePtr createSamplerStateObject(const SamplerStateDesc& desc) const;

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
        if(texture->getType() == TT_Texture2D)
            return new GLTexture2DRenderView(texture, 0, 0);
        return RenderViewPtr();
    }
    
    RenderViewPtr GLGraphicFactory::createDepthStencilView(const TexturePtr& texture) const {
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
        return new CgGLEffect((GLGraphicDevice*)mGraphicDevice.get());
    }
    
    BlendStatePtr GLGraphicFactory::createBlendStateObject(const BlendStateDesc& desc) const {
        return MakeSharedPtr<GLBlendStateObject>(desc);
    }
    
    SamplerStatePtr GLGraphicFactory::createSamplerStateObject(const SamplerStateDesc& desc) const {
        return MakeSharedPtr<GLSamplerStateObject>(desc);
    }
    
} // namespace ukn
