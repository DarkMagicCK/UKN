#include "D3D10GraphicFactory.h"

#include "D3D10GraphicDevice.h"
#include "D3D10FrameBuffer.h"
#include "D3D10RenderBuffer.h"
#include "D3D10GraphicBuffer.h"
#include "D3D10Shader.h"
#include "D3D10Texture.h"
#include "D3D10RenderView.h"

#include "UKN/RenderView.h"
#include "UKN/SpriteBatch.h"
#include "UKN/RenderBuffer.h"

namespace ukn {

	class D3D10GraphicFactory: public GraphicFactory {
    public:
        D3D10GraphicFactory();
        virtual ~D3D10GraphicFactory();
        
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
        
    private:
        GraphicDevicePtr mGraphicDevice;
    };
    
    extern "C" {
        UKN_D3D10_API void CreateGraphicFactory(GraphicFactoryPtr& ptr) {
            static GraphicFactoryPtr static_ptr = MakeSharedPtr<D3D10GraphicFactory>();
            ptr = static_ptr;
        }
    }
    
    
    D3D10GraphicFactory::D3D10GraphicFactory() {
		try {
			mGraphicDevice = new D3D10GraphicDevice();
		} catch(Exception& exp) {
			MessageBoxW(0, exp.what(), L"Fatal Error", MB_OK | MB_ICONERROR);
		}
    }
    
    D3D10GraphicFactory::~D3D10GraphicFactory() {
        
    }
    
    GraphicDevice& D3D10GraphicFactory::getGraphicDevice() const {
        return *mGraphicDevice;
    }
    
    SpriteBatchPtr D3D10GraphicFactory::createSpriteBatch() const {
        //return SpriteBatchPtr(new SpriteBatch());
		return SpriteBatchPtr();
    }
    
    GraphicBufferPtr D3D10GraphicFactory::createVertexBuffer(GraphicBuffer::Access access, 
                                                             GraphicBuffer::Usage usage,
                                                             uint32 count, 
                                                             const void* initialData, 
                                                             const VertexFormat& format) const {
        return MakeSharedPtr<D3D10VertexBuffer>(access, usage, count, initialData, format, (D3D10GraphicDevice*)mGraphicDevice.get());
    }
    
    GraphicBufferPtr D3D10GraphicFactory::createIndexBuffer(GraphicBuffer::Access access, 
															GraphicBuffer::Usage usage, 
															uint32 count, 
															const void* initialData) const {
        return MakeSharedPtr<D3D10IndexBuffer>(access, usage, count, initialData, (D3D10GraphicDevice*)mGraphicDevice.get());
    }
    
    
    RenderBufferPtr D3D10GraphicFactory::createRenderBuffer() const {
        return MakeSharedPtr<D3D10RenderBuffer>((D3D10GraphicDevice*)mGraphicDevice.get());
    }
    
    RenderViewPtr D3D10GraphicFactory::create2DRenderView(TexturePtr texture) const {
       // return new GLTexture2DRenderView(*texture.get(), 0, 0);
		return RenderView::NullObject();
    }
    
    RenderViewPtr D3D10GraphicFactory::create2DDepthStencilView(TexturePtr texture) const {
        return RenderView::NullObject();
    }
    
    FrameBufferPtr D3D10GraphicFactory::createFrameBuffer() const {
       return FrameBufferPtr(new D3D10FrameBuffer(true, (D3D10GraphicDevice*)mGraphicDevice.get()));
    }
    
    TexturePtr D3D10GraphicFactory::create2DTexture(uint32 width, uint32 height, uint32 numMipmaps, ElementFormat format, const uint8* initialData) const {
        SharedPtr<D3D10Texture2D> texture = MakeSharedPtr<D3D10Texture2D>((D3D10GraphicDevice*)mGraphicDevice.get());
		if(texture &&
			texture->create(width, height, numMipmaps, format, initialData)) {
			return texture;
		}
		return SharedPtr<D3D10Texture2D>();
    }
    
    TexturePtr D3D10GraphicFactory::load2DTexture(const ResourcePtr& rsrc, bool generateMipmaps) const {
        SharedPtr<D3D10Texture2D> texture = MakeSharedPtr<D3D10Texture2D>((D3D10GraphicDevice*)mGraphicDevice.get());
		if(texture &&
			texture->load(rsrc, generateMipmaps)) {
			return texture;
		}
		return SharedPtr<D3D10Texture2D>();
    }

} // namespace ukn