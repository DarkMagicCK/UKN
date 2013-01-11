#include "D3D10FrameBuffer.h"
#include "D3D10GraphicDevice.h"
#include "D3D10RenderView.h"

#include "ukn/Context.h"
#include "ukn/GraphicFactory.h"

namespace ukn {

	D3D10FrameBuffer::D3D10FrameBuffer(bool offscreen):
    mOffscreen(offscreen) {

    }
    
    D3D10FrameBuffer::~D3D10FrameBuffer() {
        
    }
    
    bool D3D10FrameBuffer::requiresFlipping() const {
        return false;
    }
    
    void D3D10FrameBuffer::swapBuffers() {
		D3D10GraphicDevice& gd = *checked_cast<D3D10GraphicDevice*>(&Context::Instance().getGraphicFactory().getGraphicDevice());
		HRESULT result;

		if(Context::Instance().getCfg().render_cfg.vsync) {
			result = gd.getSwapChain()->Present(1, 0);
		} else {
			result = gd.getSwapChain()->Present(0, 0);
		}
		if(FAILED(result)) {
			MIST_THROW_EXCEPTION("unable to present framebuffer");
		}
	}
    
    SharedPtr<uint8> D3D10FrameBuffer::readFrameBufferData(int32 x, int32 y, uint32 width, uint32 height, ElementFormat format) {
        return SharedPtr<uint8>();
    }
    
    void D3D10FrameBuffer::clear(uint32 flags, const class Color& clr, float depth, int32 stencil) {
        if(flags & CM_Color) {
			if(this->attached(ATT_Color0)) {
				this->attached(ATT_Color0)->clearColor(clr);
			}
		}
		RenderViewPtr depthStencilView = this->attached(ATT_DepthStencil);
		if((bool)depthStencilView) {
			if(flags & CM_Depth)
				depthStencilView->clearDepth(depth);
			if(flags & CM_Stencil)
				depthStencilView->clearStencil(stencil);
		}
    }
    
    void D3D10FrameBuffer::onBind() {
        D3D10GraphicDevice& gd = *checked_cast<D3D10GraphicDevice*>(&Context::Instance().getGraphicFactory().getGraphicDevice());
        
    }
    
    void D3D10FrameBuffer::onUnbind() {
        D3D10GraphicDevice& gd = *checked_cast<D3D10GraphicDevice*>(&Context::Instance().getGraphicFactory().getGraphicDevice());
        
    }
    

} // namespace ukn