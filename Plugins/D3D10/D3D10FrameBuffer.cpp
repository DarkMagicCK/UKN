#include "D3D10FrameBuffer.h"
#include "D3D10GraphicDevice.h"
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
		
		if(Context::Instance().getCfg().render_cfg.vsync) {
			gd.getSwapChain()->Present(1, 0);
		} else {
			gd.getSwapChain()->Present(0, 0);
		}
	}
    
    SharedPtr<uint8> D3D10FrameBuffer::readFrameBufferData(int32 x, int32 y, uint32 width, uint32 height, ElementFormat format) {
        return SharedPtr<uint8>();
    }
    
    void D3D10FrameBuffer::clear(uint32 flags, const class Color& clr, float depth, int32 stencil) {
        
    }
    
    void D3D10FrameBuffer::onBind() {
        D3D10GraphicDevice& gd = *checked_cast<D3D10GraphicDevice*>(&Context::Instance().getGraphicFactory().getGraphicDevice());
        
    }
    
    void D3D10FrameBuffer::onUnbind() {
        D3D10GraphicDevice& gd = *checked_cast<D3D10GraphicDevice*>(&Context::Instance().getGraphicFactory().getGraphicDevice());
        
    }
    

} // namespace ukn