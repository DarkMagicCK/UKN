#include "D3D11RenderView.h"
#include "D3D11Preq.h"
#include "D3D11GraphicDevice.h"
#include "D3D11Texture.h"

#include "UKN/Context.h"
#include "ukn/GraphicFactory.h"

namespace ukn {

    D3D11RenderView::D3D11RenderView(D3D11GraphicDevice* device):
        mGraphicDevice(device) {

    }

    D3D11RenderView::~D3D11RenderView() {

    }

    void D3D11RenderView::clearDepth(float depth) {

    }

    void D3D11RenderView::clearStencil(int32 stencil) {

    }

    void D3D11RenderView::clearDepthStencil(float depth, int32 stencil) {

    }

    D3D11RenderTargetView::D3D11RenderTargetView(D3D11GraphicDevice* device):
        D3D11RenderView(device) {

    }

    ID3D11RenderTargetView* D3D11RenderTargetView::getD3D11RenderTargetView() const {
        return mRenderTargetView.get();
    }

    D3D11ScreenColorRenderView::D3D11ScreenColorRenderView(uint32 width, uint32 height, ElementFormat ef, D3D11GraphicDevice* device):
        D3D11RenderTargetView(device) {
            mWidth = width;
            mHeight = height;
            mElementFormat = ef;

            ID3D11Device* idevice = device->getD3DDevice();
            IDXGISwapChain* swapChain = device->getSwapChain();

            ID3D11Texture2D* backBuffer = 0;
            HRESULT result = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
            if(FAILED(result))
                MIST_THROW_EXCEPTION(L"Error getting backbuffer");

            ID3D11RenderTargetView* renderTargetView;
            result = idevice->CreateRenderTargetView(backBuffer, NULL, &renderTargetView);
            if(FAILED(result))
                MIST_THROW_EXCEPTION(L"Error creating render target view");

            mRenderTargetView = MakeCOMPtr(renderTargetView);
            backBuffer->Release();
    }

    D3D11ScreenColorRenderView::~D3D11ScreenColorRenderView() {
    }

    void D3D11ScreenColorRenderView::clearColor(const Color& clr) {
        mGraphicDevice->getD3DDeviceContext()->ClearRenderTargetView(mRenderTargetView.get(), clr.c.value); 
    }

    void D3D11ScreenColorRenderView::clearDepth(float depth) {

    }

    void D3D11ScreenColorRenderView::clearStencil(int32 stencil) {

    }

    void D3D11ScreenColorRenderView::clearDepthStencil(float depth, int32 stencil) {

    }

    void D3D11ScreenColorRenderView::onAttached(FrameBuffer& fb, uint32 att) {

    }

    void D3D11ScreenColorRenderView::onDetached(FrameBuffer& fb, uint32 att) {

    }

    D3D11DepthStencilRenderView::D3D11DepthStencilRenderView(uint32 width, uint32 height, ElementFormat ef, uint32 sampleCount, uint32 sampleQuality, D3D11GraphicDevice* device):
        D3D11RenderView(device) {
            mWidth = width;
            mHeight = height;
            mElementFormat = ef;

            ID3D11Device* idevice = device->getD3DDevice();

            D3D11_TEXTURE2D_DESC depthBufferDesc;
            ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));
            depthBufferDesc.Width = width;
            depthBufferDesc.Height = height;
            depthBufferDesc.MipLevels = 1;
            depthBufferDesc.ArraySize = 1;
            depthBufferDesc.Format = ElementFormatToDxGIFormat(ef);
            depthBufferDesc.SampleDesc.Count = sampleCount;
            depthBufferDesc.SampleDesc.Quality = sampleQuality;
            depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
            depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
            depthBufferDesc.CPUAccessFlags = 0;
            depthBufferDesc.MiscFlags = 0;

            ID3D11Texture2D* depthBufferTex;
            HRESULT result = idevice->CreateTexture2D(&depthBufferDesc, NULL, &depthBufferTex);
            if(FAILED(result))
                log_error(L"D3D1DepthStencilRenderView: Error create depth stencil texture 2d");
            else {
                mDepthStencilBuffer = MakeCOMPtr(depthBufferTex);
                this->createDSView(TB_DepthStencil, ef, sampleCount);
            }
    }

    D3D11DepthStencilRenderView::D3D11DepthStencilRenderView(const TexturePtr& texture, D3D11GraphicDevice* device):
    D3D11RenderView(device) {
        if(texture->type() != TT_Texture2D) {
            log_error("D3D11DepthStencilRenderView: invalid texture format");
        } else {
            mTexture = texture;
            mDepthStencilBuffer = (ID3D11Texture2D*)mTexture->getTextureId();
            // temp
            this->createDSView(((D3D11Texture*)texture.get())->getTextureId(), 
                                texture->format(), 
                                1);
        }
    }

    D3D11DepthStencilRenderView::~D3D11DepthStencilRenderView() {
    }

    void D3D11DepthStencilRenderView::createDSView(uint32 bindFlag, ElementFormat ef, int32 sampleCount) {
        ID3D11Device* idevice = mGraphicDevice->getD3DDevice();

        D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
        ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

        depthStencilViewDesc.Format = ElementFormatToDxGIFormat(ef);
        depthStencilViewDesc.ViewDimension = sampleCount > 1 ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;
        depthStencilViewDesc.Texture2D.MipSlice = 0;
        
        ID3D11DepthStencilView* view;
        HRESULT result = idevice->CreateDepthStencilView(mDepthStencilBuffer.get(), 
                                                         &depthStencilViewDesc, 
                                                         &view);
        if(FAILED(result))
            log_error(L"D3D1DepthStencilRenderView: Error create depth stencil view");
        else
            mDepthStencilView = MakeCOMPtr(view);

    }

    ID3D11DepthStencilView* D3D11DepthStencilRenderView::getD3D11DepthStencilView() const {
        return mDepthStencilView.get();
    }

    void D3D11DepthStencilRenderView::clearColor(const Color& clr) {

    }

    void D3D11DepthStencilRenderView::clearDepth(float depth) {
        if(mDepthStencilView)
            mGraphicDevice->getD3DDeviceContext()->ClearDepthStencilView(mDepthStencilView.get(), 
                                                                         D3D11_CLEAR_DEPTH, 
                                                                         depth, 
                                                                         0);
    }

    void D3D11DepthStencilRenderView::clearStencil(int32 stencil) {
        if(mDepthStencilView)
            mGraphicDevice->getD3DDeviceContext()->ClearDepthStencilView(mDepthStencilView.get(), 
                                                                         D3D11_CLEAR_STENCIL, 
                                                                         0, 
                                                                         stencil);
    }

    void D3D11DepthStencilRenderView::clearDepthStencil(float depth, int32 stencil) {
        if(mDepthStencilView)
            mGraphicDevice->getD3DDeviceContext()->ClearDepthStencilView(mDepthStencilView.get(), 
                                                                         D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 
                                                                         depth, 
                                                                         stencil);
    }

    void D3D11DepthStencilRenderView::onAttached(FrameBuffer& fb, uint32 att) {
    
    }

    void D3D11DepthStencilRenderView::onDetached(FrameBuffer& fb, uint32 att) {

    }

    D3D11Texture2DRenderView::D3D11Texture2DRenderView(const TexturePtr& texture, D3D11GraphicDevice* device):
        D3D11RenderTargetView(device),
        mTexture(texture)
    {
        D3D11_RENDER_TARGET_VIEW_DESC renderTargetDesc;

        D3D11Texture2D* d3d11Texture = checked_cast<D3D11Texture2D*>(texture.get());
        if(texture) {
            D3D11_TEXTURE2D_DESC textureDesc;
            d3d11Texture->getTexture()->GetDesc(&textureDesc);

            renderTargetDesc.Format = textureDesc.Format;
            renderTargetDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
            renderTargetDesc.Texture2D.MipSlice = 0;

            ID3D11RenderTargetView* renderTargetView;
            if(!D3D11Debug::CHECK_RESULT(device->getD3DDevice()->CreateRenderTargetView(
                d3d11Texture->getTexture(),
                &renderTargetDesc,
                &renderTargetView))) {
                    MIST_THROW_EXCEPTION(L"Error creating render target view");
            }
            mRenderTargetView = MakeCOMPtr(renderTargetView);

            mWidth = d3d11Texture->width(0);
            mHeight = d3d11Texture->height(0);
            mElementFormat = d3d11Texture->format();
        }
    }

    D3D11Texture2DRenderView::~D3D11Texture2DRenderView() {

    }

    void D3D11Texture2DRenderView::clearColor(const Color& clr) {
        float color[4];
        color[0] = clr.r(); color[1] = clr.g(); color[2] = clr.b(); color[3] = clr.a();

        mGraphicDevice->getD3DDeviceContext()->ClearRenderTargetView(mRenderTargetView.get(), color);
    }

    void D3D11Texture2DRenderView::onAttached(FrameBuffer& fb, uint32 att) {
        // OMSetRenderTargets in FrameBuffer
    }

    void D3D11Texture2DRenderView::onDetached(FrameBuffer& fb, uint32 att) {
        // OMSetRenderTargets in FrameBuffer
    }

}