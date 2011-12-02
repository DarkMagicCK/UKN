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

namespace ukn {
    
    class GLGraphicFactory: public GraphicFactory {
    public:
        GLGraphicFactory();
        ~GLGraphicFactory();
        
        GraphicDevice& getGraphicDevice() const;
        
        GraphicBufferPtr createVertexBuffer(GraphicBuffer::Usage, 
                                            GraphicBuffer::Access, 
                                            uint32 count, 
                                            const void* initialData, 
                                            const VertexFormat& format);
        
        GraphicBufferPtr createIndexBuffer(GraphicBuffer::Usage, 
                                           GraphicBuffer::Access, 
                                           uint32 count, 
                                           const void* initialData);
        
        
        RenderBufferPtr createRenderBuffer();
        
        RenderViewPtr create2DRenderView(TexturePtr texture);
        RenderViewPtr create2DDepthStencilView(TexturePtr texture);
        
        FrameBufferPtr createFrameBuffer();
        
        TexturePtr create2DTexture(uint32 width, uint32 height, uint32 numMipmaps, ElementFormat format, const uint8* initialData);
        
        TexturePtr load2DTexture(const ukn_wstring& name, bool generateMipmaps=false);
        
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
    
    GraphicBufferPtr GLGraphicFactory::createVertexBuffer(GraphicBuffer::Usage, 
                                                          GraphicBuffer::Access, 
                                                          uint32 count, 
                                                          const void* initialData, 
                                                          const VertexFormat& format) {
        return GraphicBuffer::NullObject();
    }
    
    GraphicBufferPtr GLGraphicFactory::createIndexBuffer(GraphicBuffer::Usage, 
                                                         GraphicBuffer::Access, 
                                                         uint32 count, 
                                                         const void* initialData) {
        return GraphicBuffer::NullObject();
    }
    
    
    RenderBufferPtr GLGraphicFactory::createRenderBuffer() {
        return RenderBuffer::NullObject();
    }
    
    RenderViewPtr GLGraphicFactory::create2DRenderView(TexturePtr texture) {
        return RenderView::NullObject();
    }
    
    RenderViewPtr GLGraphicFactory::create2DDepthStencilView(TexturePtr texture) {
        return RenderView::NullObject();
    }
    
    FrameBufferPtr GLGraphicFactory::createFrameBuffer() {
        return FrameBuffer::NullObject();
    }
    
    TexturePtr GLGraphicFactory::create2DTexture(uint32 width, uint32 height, uint32 numMipmaps, ElementFormat format, const uint8* initialData) {
        GLTexture2D* texture = new GLTexture2D();
        if(texture->create(width, height, numMipmaps, format, initialData)) {
            return texture;
        }
        return TexturePtr();
    }
    
    TexturePtr GLGraphicFactory::load2DTexture(const ukn_wstring& name, bool generateMipmaps) {
        GLTexture2D* texture = new GLTexture2D();
        if(texture->load(name, generateMipmaps)) {
            return texture;
        }
        return TexturePtr();
    }
    
} // namespace ukn
