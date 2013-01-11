#include "D3D10GraphicFactory.h"

#include "D3D10GraphicDevice.h"
#include "D3D10FrameBuffer.h"

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
        UKN_D3D10_API void CreateGraphicFactoryD3D(GraphicFactoryPtr& ptr) {
            static GraphicFactoryPtr static_ptr = MakeSharedPtr<D3D10GraphicFactory>();
            ptr = static_ptr;
        }
    }
    
    
    D3D10GraphicFactory::D3D10GraphicFactory() {
		try {
			mGraphicDevice = new D3D10GraphicDevice();
		} catch(Exception& exp) {
			MessageBoxA(0, exp.what(), "Fatal Error", MB_OK | MB_ICONERROR);
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
        return GraphicBufferPtr();
    }
    
    GraphicBufferPtr D3D10GraphicFactory::createIndexBuffer(GraphicBuffer::Access access, 
                                                         GraphicBuffer::Usage usage, 
                                                         uint32 count, 
                                                         const void* initialData) const {
        return GraphicBufferPtr();
    }
    
    
    RenderBufferPtr D3D10GraphicFactory::createRenderBuffer() const {
        return RenderBufferPtr();
    }
    
    RenderViewPtr D3D10GraphicFactory::create2DRenderView(TexturePtr texture) const {
       // return new GLTexture2DRenderView(*texture.get(), 0, 0);
		return RenderViewPtr();
    }
    
    RenderViewPtr D3D10GraphicFactory::create2DDepthStencilView(TexturePtr texture) const {
        return RenderView::NullObject();
    }
    
    FrameBufferPtr D3D10GraphicFactory::createFrameBuffer() const {
       return FrameBufferPtr(new D3D10FrameBuffer(true));
    }
    
    TexturePtr D3D10GraphicFactory::create2DTexture(uint32 width, uint32 height, uint32 numMipmaps, ElementFormat format, const uint8* initialData) const {
       
        return TexturePtr();
    }
    
    TexturePtr D3D10GraphicFactory::load2DTexture(const ResourcePtr& rsrc, bool generateMipmaps) const {
        
        return TexturePtr();
    }

} // namespace ukn