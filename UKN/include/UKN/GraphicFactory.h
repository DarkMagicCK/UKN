//
//  GraphicFactory.h
//  Project Unknown
//
//  Created by Robert Bu on 11/23/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_GraphicFactory_h
#define Project_Unknown_GraphicFactory_h

#include "mist/Platform.h"
#include "UKN/PreDeclare.h"
#include "UKN/GraphicBuffer.h"
#include "UKN/GraphicSettings.h"
#include "UKN/Vertex.h"

namespace ukn {
    
    class UKN_API GraphicFactory {
    public:
        static GraphicFactoryPtr NullObject();
        
        GraphicFactory();
        virtual ~GraphicFactory();
        
        virtual GraphicDevice& getGraphicDevice() const = 0;
        
        virtual SpriteBatchPtr createSpriteBatch() const;
        
        
        virtual GraphicBufferPtr createVertexBuffer(GraphicBuffer::Access, 
                                                    GraphicBuffer::Usage, 
                                                    uint32 count, 
                                                    const void* initialData, 
                                                    const vertex_elements_type& format) const = 0;
        
        virtual GraphicBufferPtr createIndexBuffer(GraphicBuffer::Access, 
                                                   GraphicBuffer::Usage, 
                                                   uint32 count, 
                                                   const void* initialData) const = 0;
        
        
        virtual RenderBufferPtr createRenderBuffer() const = 0;
        
        // FBOs
        virtual RenderViewPtr createRenderView(const TexturePtr& texture) const = 0;
        virtual RenderViewPtr createDepthStencilView(const TexturePtr& texture) const = 0;
        virtual RenderViewPtr createDepthStencilView(uint32 width,
                                                     uint32 height,
                                                     ElementFormat format,
                                                     uint32 sampleCount,
                                                     uint32 sampleQuality) const = 0;
        
        // FrameBuffer
        virtual FrameBufferPtr createFrameBuffer() const = 0;
        
        // textures
        virtual TexturePtr create2DTexture(uint32 width, 
                                           uint32 height, 
                                           uint32 numMipmaps, 
                                           ElementFormat format, 
                                           const uint8* initialData = 0,
                                           uint32 bindFlag = TB_Texture /* used by d3d */) const = 0;
        virtual TexturePtr load2DTexture(const ResourcePtr& rsrc, bool generateMipmaps=false) const = 0;
        
        // shaders & effects
        virtual EffectPtr createEffect() const = 0;

        virtual BlendStatePtr createBlendStateObject(const BlendStateDesc& desc) const = 0;
        virtual SamplerStatePtr createSamplerStateObject(const SamplerStateDesc& desc) const = 0;
        virtual RasterizerStatePtr createRasterizerStateObject(const RasterizerStateDesc& desc) const = 0;
        virtual DepthStencilStatePtr createDepthStencilStateObject(const DepthStencilStateDesc& desc) const = 0;
    };
    
} // namespace ukn


#endif
