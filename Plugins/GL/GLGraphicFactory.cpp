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

#include "GLTexture.h"
#include "GLRenderBuffer.h"
#include "GLFrameBuffer.h"
#include "GLGraphicBuffer.h"
#include "GLSpriteBatch.h"

namespace ukn {
    
    class GLGraphicFactory: public GraphicFactory {
    public:
        GLGraphicFactory();
        ~GLGraphicFactory();
        
        GraphicDevice& getGraphicDevice() const;
        
        SpriteBatchPtr createSpriteBatch() const;
        
        GraphicBufferPtr createVertexBuffer(GraphicBuffer::Access, 
                                            GraphicBuffer::Usage, 
                                            uint32 count, 
                                            const void* initialData, 
                                            const VertexFormat& format);
        
        GraphicBufferPtr createIndexBuffer(GraphicBuffer::Access, 
                                           GraphicBuffer::Usage, 
                                           uint32 count, 
                                           const void* initialData);
        
        
        RenderBufferPtr createRenderBuffer();
        
        RenderViewPtr create2DRenderView(TexturePtr texture);
        RenderViewPtr create2DDepthStencilView(TexturePtr texture);
        
        FrameBufferPtr createFrameBuffer();
        
        TexturePtr create2DTexture(uint32 width, uint32 height, uint32 numMipmaps, ElementFormat format, const uint8* initialData);
        
        TexturePtr load2DTexture(const ResourcePtr& rsrc, bool generateMipmaps=false);
        
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
                                                          const VertexFormat& format) {
        return GraphicBufferPtr(new GLVertexBuffer(access,
                                                   usage,
                                                   count,
                                                   initialData,
                                                   format));
    }
    
    GraphicBufferPtr GLGraphicFactory::createIndexBuffer(GraphicBuffer::Access access, 
                                                         GraphicBuffer::Usage usage, 
                                                         uint32 count, 
                                                         const void* initialData) {
        return GraphicBufferPtr(new GLIndexBuffer(access,
                                                  usage,
                                                  count,
                                                  initialData));
    }
    
    
    RenderBufferPtr GLGraphicFactory::createRenderBuffer() {
        return RenderBufferPtr(new GLRenderBuffer());
    }
    
    RenderViewPtr GLGraphicFactory::create2DRenderView(TexturePtr texture) {
        return RenderView::NullObject();
    }
    
    RenderViewPtr GLGraphicFactory::create2DDepthStencilView(TexturePtr texture) {
        return RenderView::NullObject();
    }
    
    FrameBufferPtr GLGraphicFactory::createFrameBuffer() {
        return FrameBufferPtr(new GLFrameBuffer(true));
    }
    
    TexturePtr GLGraphicFactory::create2DTexture(uint32 width, uint32 height, uint32 numMipmaps, ElementFormat format, const uint8* initialData) {
        GLTexture2D* texture = new GLTexture2D();
        if(texture->create(width, height, numMipmaps, format, initialData)) {
            return TexturePtr(texture);
        }
		delete texture;
        return TexturePtr();
    }
    
    TexturePtr GLGraphicFactory::load2DTexture(const ResourcePtr& rsrc, bool generateMipmaps) {
        GLTexture2D* texture = new GLTexture2D();
        if(texture->load(rsrc, generateMipmaps)) {
            return TexturePtr(texture);
        }
		delete texture;
        return TexturePtr();
    }
    
} // namespace ukn
