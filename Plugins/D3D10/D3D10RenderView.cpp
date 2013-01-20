#include "D3D10RenderView.h"
#include "D3D10Preq.h"
#include "D3D10GraphicDevice.h"
#include "UKN/Context.h"
#include "ukn/GraphicFactory.h"

namespace ukn {

	D3D10RenderView::D3D10RenderView(D3D10GraphicDevice* device):
	mGraphicDevice(device) {

	}

	D3D10RenderView::~D3D10RenderView() {

	}

	void D3D10RenderView::clearDepth(float depth) {

	}

	void D3D10RenderView::clearStencil(int32 stencil) {

	}

	void D3D10RenderView::clearDepthStencil(float depth, int32 stencil) {

	}

	D3D10ScreenColorRenderView::D3D10ScreenColorRenderView(uint32 width, uint32 height, ElementFormat ef, D3D10GraphicDevice* device):
	D3D10RenderView(device) {
		mWidth = width;
		mHeight = height;
		mElementFormat = ef;

		ID3D10Device* idevice = device->getD3DDevice();
		IDXGISwapChain* swapChain = device->getSwapChain();

		ID3D10Texture2D* backBuffer = 0;
		HRESULT result = swapChain->GetBuffer(0, __uuidof(ID3D10Texture2D), (void**)&backBuffer);
		if(FAILED(result))
			MIST_THROW_EXCEPTION(L"Error getting backbuffer");

		result = idevice->CreateRenderTargetView(backBuffer, NULL, &mRenderView);
		if(FAILED(result))
			MIST_THROW_EXCEPTION(L"Error creating render target view");

		backBuffer->Release();
	}

	D3D10ScreenColorRenderView::~D3D10ScreenColorRenderView() {
		if(mRenderView)
			mRenderView->Release();
	}

	ID3D10RenderTargetView* D3D10ScreenColorRenderView::getD3D10RenderTargetView() const {
		return mRenderView;
	}

	void D3D10ScreenColorRenderView::clearColor(const Color& clr) {
		float color[4];
		color[0] = clr.r; color[1] = clr.g; color[2] = clr.b; color[3] = clr.a;
		mGraphicDevice->getD3DDevice()->ClearRenderTargetView(mRenderView, color); 
	}

	void D3D10ScreenColorRenderView::clearDepth(float depth) {

	}

	void D3D10ScreenColorRenderView::clearStencil(int32 stencil) {

	}

	void D3D10ScreenColorRenderView::clearDepthStencil(float depth, int32 stencil) {

	}

	void D3D10ScreenColorRenderView::onAttached(FrameBuffer& fb, uint32 att) {

	}

	void D3D10ScreenColorRenderView::onDetached(FrameBuffer& fb, uint32 att) {

	}

	D3D10DepthStencilRenderView::D3D10DepthStencilRenderView(uint32 width, uint32 height, ElementFormat ef, D3D10GraphicDevice* device):
	D3D10RenderView(device) {
		mWidth = width;
		mHeight = height;
		mElementFormat = ef;

		ID3D10Device* idevice = device->getD3DDevice();

		D3D10_TEXTURE2D_DESC depthBufferDesc;
		ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));
		depthBufferDesc.Width = width;
		depthBufferDesc.Height = height;
		depthBufferDesc.MipLevels = 1;
		depthBufferDesc.ArraySize = 1;
		depthBufferDesc.Format = ElementFormatToDxGIFormat(ef);
		depthBufferDesc.SampleDesc.Count = 1;
		depthBufferDesc.SampleDesc.Quality = 0;
		depthBufferDesc.Usage = D3D10_USAGE_DEFAULT;
		depthBufferDesc.BindFlags = D3D10_BIND_DEPTH_STENCIL;
		depthBufferDesc.CPUAccessFlags = 0;
		depthBufferDesc.MiscFlags = 0;

		HRESULT result = idevice->CreateTexture2D(&depthBufferDesc, NULL, &mDepthStencilBuffer);
		if(FAILED(result))
			MIST_THROW_EXCEPTION(L"Error create depth stencil texture 2d");

		D3D10_DEPTH_STENCIL_DESC depthStencilDesc;
		ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
		depthStencilDesc.DepthEnable = true;
		depthStencilDesc.DepthWriteMask = D3D10_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D10_COMPARISON_LESS;
		depthStencilDesc.StencilEnable = ef == EF_D24S8;
		depthStencilDesc.StencilReadMask = 0xFF;
		depthStencilDesc.StencilWriteMask = 0xFF;
		depthStencilDesc.FrontFace.StencilFailOp = D3D10_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilDepthFailOp = D3D10_STENCIL_OP_INCR;
		depthStencilDesc.FrontFace.StencilPassOp = D3D10_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilFunc = D3D10_COMPARISON_ALWAYS;
		depthStencilDesc.BackFace.StencilFailOp = D3D10_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilDepthFailOp = D3D10_STENCIL_OP_DECR;
		depthStencilDesc.BackFace.StencilPassOp = D3D10_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilFunc = D3D10_COMPARISON_ALWAYS;

		result = idevice->CreateDepthStencilState(&depthStencilDesc, &mDepthStencilState);
		if(FAILED(result))
			MIST_THROW_EXCEPTION(L"Error create depth stencil state");

		D3D10_DEPTH_STENCIL_DESC depthStencilDisabledDesc;
		memcpy(&depthStencilDisabledDesc, &depthStencilDesc, sizeof(depthStencilDesc));
		depthStencilDisabledDesc.DepthEnable = false;

		result = idevice->CreateDepthStencilState(&depthStencilDisabledDesc, &mDepthStencilDisabledState);
		if(FAILED(result))
			MIST_THROW_EXCEPTION(L"Error create depth stencil disabled state");

		D3D10_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
		ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
		depthStencilViewDesc.Format = ElementFormatToDxGIFormat(ef);
		depthStencilViewDesc.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;

		result = idevice->CreateDepthStencilView(mDepthStencilBuffer, &depthStencilViewDesc, &mDepthStencilView);
		if(FAILED(result))
			MIST_THROW_EXCEPTION(L"Error create depth stencil view");

	}

	D3D10DepthStencilRenderView::~D3D10DepthStencilRenderView() {
		mDepthStencilView->Release();
		mDepthStencilBuffer->Release();
		mDepthStencilState->Release();
	}

	ID3D10DepthStencilView* D3D10DepthStencilRenderView::getD3D10DepthStencilView() const {
		return mDepthStencilView;
	}

	void D3D10DepthStencilRenderView::clearColor(const Color& clr) {

	}

	void D3D10DepthStencilRenderView::clearDepth(float depth) {
		mGraphicDevice->getD3DDevice()->ClearDepthStencilView(mDepthStencilView, D3D10_CLEAR_DEPTH, depth, 0);
	}

	void D3D10DepthStencilRenderView::clearStencil(int32 stencil) {
		mGraphicDevice->getD3DDevice()->ClearDepthStencilView(mDepthStencilView, D3D10_CLEAR_STENCIL, 0, stencil);
	}

	void D3D10DepthStencilRenderView::clearDepthStencil(float depth, int32 stencil) {
		mGraphicDevice->getD3DDevice()->ClearDepthStencilView(mDepthStencilView, D3D10_CLEAR_DEPTH | D3D10_CLEAR_STENCIL, depth, stencil);
	}

	void D3D10DepthStencilRenderView::onAttached(FrameBuffer& fb, uint32 att) {
		mGraphicDevice->getD3DDevice()->OMSetDepthStencilState(mDepthStencilState, 1);
	}

	void D3D10DepthStencilRenderView::onDetached(FrameBuffer& fb, uint32 att) {

	}

	void D3D10DepthStencilRenderView::enableDepth(bool flag) {
		mGraphicDevice->getD3DDevice()->OMSetDepthStencilState(flag ? mDepthStencilState : mDepthStencilDisabledState,
															   1);
	}
}