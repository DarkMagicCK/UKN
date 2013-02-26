#include "D3D11GraphicFactory.h"

#include "D3D11GraphicDevice.h"
#include "D3D11FrameBuffer.h"
#include "D3D11RenderBuffer.h"
#include "D3D11GraphicBuffer.h"
#include "D3D11Shader.h"
#include "D3D11Texture.h"
#include "D3D11RenderView.h"
#include "D3D11BlendStateObject.h"
#include "D3D11SamplerStateObject.h"
#include "D3D11RasterizerStateObject.h"
#include "D3D11DepthStencilStateObject.h"

#include "CgShader.h"

#include "UKN/RenderView.h"
#include "UKN/SpriteBatch.h"
#include "UKN/RenderBuffer.h"

namespace ukn {

    class D3D11GraphicFactory: public GraphicFactory {
    public:
        D3D11GraphicFactory();
        virtual ~D3D11GraphicFactory();

        GraphicDevice& getGraphicDevice() const;

   //     SpriteBatchPtr createSpriteBatch() const;

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

        TexturePtr create2DTexture(uint32 width, 
                                   uint32 height, 
                                   uint32 numMipmaps, 
                                   ElementFormat format, 
                                   const uint8* initialData,
                                   uint32 flag) const;
        TexturePtr load2DTexture(const ResourcePtr& rsrc, bool generateMipmaps=false) const;

        EffectPtr createEffect() const;
        
        BlendStatePtr createBlendStateObject(const BlendStateDesc& desc) const;
        SamplerStatePtr createSamplerStateObject(const SamplerStateDesc& desc) const;
        RasterizerStatePtr createRasterizerStateObject(const RasterizerStateDesc& desc) const;
        DepthStencilStatePtr createDepthStencilStateObject(const DepthStencilStateDesc& desc) const;

    private:
        GraphicDevicePtr mGraphicDevice;
    };

    extern "C" {
        UKN_D3D11_API void CreateGraphicFactory(GraphicFactoryPtr& ptr) {
            static GraphicFactoryPtr static_ptr = MakeSharedPtr<D3D11GraphicFactory>();
            ptr = static_ptr;
        }
    }


    D3D11GraphicFactory::D3D11GraphicFactory() {
        try {
            mGraphicDevice = new D3D11GraphicDevice();
        } catch(Exception& exp) {
            MessageBoxW(0, exp.what(), L"Fatal Error", MB_OK | MB_ICONERROR);
        }
    }

    D3D11GraphicFactory::~D3D11GraphicFactory() {

    }

    GraphicDevice& D3D11GraphicFactory::getGraphicDevice() const {
        return *mGraphicDevice;
    }

   // SpriteBatchPtr D3D11GraphicFactory::createSpriteBatch() const {
        //return SpriteBatchPtr(new SpriteBatch());
        //return SpriteBatchPtr();
    //}

    GraphicBufferPtr D3D11GraphicFactory::createVertexBuffer(GraphicBuffer::Access access, 
        GraphicBuffer::Usage usage,
        uint32 count, 
        const void* initialData, 
        const vertex_elements_type& format) const {
            return MakeSharedPtr<D3D11VertexBuffer>(access, usage, count, initialData, format, (D3D11GraphicDevice*)mGraphicDevice.get());
    }

    GraphicBufferPtr D3D11GraphicFactory::createIndexBuffer(GraphicBuffer::Access access, 
        GraphicBuffer::Usage usage, 
        uint32 count, 
        const void* initialData) const {
            return MakeSharedPtr<D3D11IndexBuffer>(access, usage, count, initialData, (D3D11GraphicDevice*)mGraphicDevice.get());
    }


    RenderBufferPtr D3D11GraphicFactory::createRenderBuffer() const {
        return MakeSharedPtr<D3D11RenderBuffer>((D3D11GraphicDevice*)mGraphicDevice.get());
    }

    RenderViewPtr D3D11GraphicFactory::createRenderView(const TexturePtr& texture) const {
        if(texture->type() == TT_Texture2D)
            return MakeSharedPtr<D3D11Texture2DRenderView>(texture, (D3D11GraphicDevice*)mGraphicDevice.get());
        return 0;
    }

    RenderViewPtr D3D11GraphicFactory::createDepthStencilView(const TexturePtr& texture) const {
        return MakeSharedPtr<D3D11DepthStencilRenderView>(texture, (D3D11GraphicDevice*)mGraphicDevice.get());
    }

    FrameBufferPtr D3D11GraphicFactory::createFrameBuffer() const {
        return FrameBufferPtr(new D3D11FrameBuffer(true, (D3D11GraphicDevice*)mGraphicDevice.get()));
    }

    TexturePtr D3D11GraphicFactory::create2DTexture(uint32 width, 
                                                    uint32 height, 
                                                    uint32 numMipmaps, 
                                                    ElementFormat format, 
                                                    const uint8* initialData,
                                                    uint32 flag) const {
        SharedPtr<D3D11Texture2D> texture = MakeSharedPtr<D3D11Texture2D>((D3D11GraphicDevice*)mGraphicDevice.get());
        if(texture &&
            texture->create(width, height, numMipmaps, format, initialData, flag)) {
                return texture;
        }
        return SharedPtr<D3D11Texture2D>();
    }

    TexturePtr D3D11GraphicFactory::load2DTexture(const ResourcePtr& rsrc, bool generateMipmaps) const {
        SharedPtr<D3D11Texture2D> texture = MakeSharedPtr<D3D11Texture2D>((D3D11GraphicDevice*)mGraphicDevice.get());
        if(texture &&
            texture->load(rsrc, generateMipmaps)) {
                return texture;
        }
        return SharedPtr<D3D11Texture2D>();
    }

    EffectPtr D3D11GraphicFactory::createEffect() const {
        CgDxEffect* effect = new CgDxEffect((D3D11GraphicDevice*)mGraphicDevice.get());
        if(effect) {
            return EffectPtr(effect);
        }
        return EffectPtr();
    }

    BlendStatePtr D3D11GraphicFactory::createBlendStateObject(const BlendStateDesc& desc) const {
        return MakeSharedPtr<D3D11BlendStateObject>(desc, ((D3D11GraphicDevice*)mGraphicDevice.get()));
    }

    SamplerStatePtr D3D11GraphicFactory::createSamplerStateObject(const SamplerStateDesc& desc) const {
        return MakeSharedPtr<D3D11SamplerStateObject>(desc, ((D3D11GraphicDevice*)mGraphicDevice.get()));
    }

    RasterizerStatePtr D3D11GraphicFactory::createRasterizerStateObject(const RasterizerStateDesc& desc) const {
        return MakeSharedPtr<D3D11RasterizerStateObject>(desc, ((D3D11GraphicDevice*)mGraphicDevice.get()));
    }

    DepthStencilStatePtr D3D11GraphicFactory::createDepthStencilStateObject(const DepthStencilStateDesc& desc) const {
        return MakeSharedPtr<D3D11DepthStencilStateObject>(desc, ((D3D11GraphicDevice*)mGraphicDevice.get()));
    }

} // namespace ukn