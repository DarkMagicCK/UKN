#include "D3D11FrameBuffer.h"
#include "D3D11GraphicDevice.h"
#include "D3D11RenderView.h"

#include "ukn/Context.h"

#include "D3D11GraphicDevice.h"

namespace ukn {

    D3D11FrameBuffer::D3D11FrameBuffer(bool offscreen,  D3D11GraphicDevice* device):
        mOffscreen(offscreen),
        mGraphicDevice(device) {

    }

    D3D11FrameBuffer::~D3D11FrameBuffer() {

    }

    bool D3D11FrameBuffer::requiresFlipping() const {
        return false;
    }

    void D3D11FrameBuffer::swapBuffers() {
        HRESULT result;

        if(Context::Instance().getCfg().render_cfg.vsync) {
            result = mGraphicDevice->getSwapChain()->Present(1, 0);
        } else {
            result = mGraphicDevice->getSwapChain()->Present(0, 0);
        }
        if(FAILED(result)) {
            MIST_THROW_EXCEPTION(L"unable to present framebuffer");
        }
    }

    SharedPtr<uint8> D3D11FrameBuffer::readFrameBufferData(int32 x, int32 y, uint32 width, uint32 height, ElementFormat format) {
        return SharedPtr<uint8>();
    }

    void D3D11FrameBuffer::clear(uint32 flags, const class Color& clr, float depth, int32 stencil) {
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

    void D3D11FrameBuffer::onBind() {
        ID3D11RenderTargetView* rtView = 0;
        ID3D11DepthStencilView* dsView = 0;

        if(this->attached(ATT_Color0)) { 
            rtView = ((D3D11RenderTargetView*)this->attached(ATT_Color0).get())->getD3D11RenderTargetView();
        }
        if(this->attached(ATT_DepthStencil)) {
            dsView = ((D3D11DepthStencilRenderView*)this->attached(ATT_DepthStencil).get())->getD3D11DepthStencilView();
        }
        mGraphicDevice->getD3DDeviceContext()->OMSetRenderTargets(1, 
            &rtView, 
            dsView);
    }

    void D3D11FrameBuffer::onUnbind() {

    }


} // namespace ukn