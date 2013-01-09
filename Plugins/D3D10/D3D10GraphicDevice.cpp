#include "D3D10GraphicDevice.h"
#include "WindowsWindow.h"
#include "D3D10FrameBuffer.h"
#include "D3D10RenderView.h"

#include "UKN/Context.h"
#include "UKN/App.h"
#include "mist/Profiler.h"

#pragma comment(lib, "d3d10.lib")
#pragma comment(lib, "d3dx10.lib")
#pragma comment(lib, "dxgi.lib")

namespace ukn {

	D3D10GraphicDevice::D3D10GraphicDevice():
		mDevice(0) {

	}

	D3D10GraphicDevice::~D3D10GraphicDevice() {

	}

	UknString D3D10GraphicDevice::description() const {
		return L"D3D10 Device";
	}

	WindowPtr D3D10GraphicDevice::doCreateRenderWindow(const UknString& name, const RenderSettings& settings) {
		mWindow = MakeSharedPtr<WindowsWindow>(name, settings);
		if(!mWindow) {
			MIST_THROW_EXCEPTION("Error creating window");
		}
		if(!this->initD3DDevice(settings, mWindow->getHWnd())) {
			MIST_THROW_EXCEPTION("Error initializing d3d10 device");
		}
	}

	bool D3D10GraphicDevice::initD3DDevice(const RenderSettings& settings, HWND hWnd) {
#define CHECK_RESULT(result) if(FAILED(result)) return false;
		IDXGIFactory* factory;
		HRESULT result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
		CHECK_RESULT(result);

		IDXGIAdapter* adapter;
		result = factory->EnumAdapters(0, &adapter);
		CHECK_RESULT(result);

		IDXGIOutput* adapterOutput;
		result = adapter->EnumOutputs(0, &adapterOutput);
		CHECK_RESULT(result);

		DXGI_FORMAT colorFormat = ColorFormatToDxGIFormat(settings.color_fmt);

		unsigned int numModes;
		result = adapterOutput->GetDisplayModeList(colorFormat,
			DXGI_ENUM_MODES_INTERLACED,
			&numModes,
			NULL);
		CHECK_RESULT(result);

		DXGI_MODE_DESC* displayModeList = new DXGI_MODE_DESC[numModes];
		result = adapterOutput->GetDisplayModeList(colorFormat,
			DXGI_ENUM_MODES_INTERLACED,
			&numModes,
			displayModeList);
		CHECK_RESULT(result);

		unsigned int numerator = 0, denominator = 1;
		for(int i=0; i<numModes; ++i) {
			if(displayModeList[i].Width == settings.width &&
				displayModeList[i].Height == settings.height) {
					numerator = displayModeList[i].RefreshRate.Numerator;
					denominator = displayModeList[i].RefreshRate.Denominator;
			}
		}

		DXGI_ADAPTER_DESC adapterDesc;
		result = adapter->GetDesc(&adapterDesc);
		CHECK_RESULT(result);

		delete[] displayModeList;
		adapterOutput->Release();
		adapter->Release();
		factory->Release();

		DXGI_SWAP_CHAIN_DESC swapChainDesc;
		ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
		swapChainDesc.BufferCount = 1;
		swapChainDesc.BufferDesc.Width = settings.width;
		swapChainDesc.BufferDesc.Height = settings.height;
		swapChainDesc.BufferDesc.Format = colorFormat;

		if(settings.vsync) {
			swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
			swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
		} else {
			swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
			swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
		}
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.OutputWindow = hWnd;

		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = settings.fsaa_samples;

		swapChainDesc.Windowed = !settings.full_screen;

		swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		swapChainDesc.Flags = 0;

		result = D3D10CreateDeviceAndSwapChain(NULL,
			D3D10_DRIVER_TYPE_HARDWARE,
			NULL,
			0,
			D3D10_SDK_VERSION,
			&swapChainDesc,
			&mSwapChain,
			&mDevice);
		if(FAILED(result)) {
			/* if hardware creation failed, try software */
			result = D3D10CreateDeviceAndSwapChain(NULL,
				D3D10_DRIVER_TYPE_WARP /* software rasterizer */,
				NULL,
				0,
				D3D10_SDK_VERSION,
				&swapChainDesc,
				&mSwapChain,
				&mDevice);
			CHECK_RESULT(result);
		}

		mScreenBuffer = MakeSharedPtr<D3D10FrameBuffer>(false);
		mScreenBuffer->attach(ATT_Color0,
							  MakeSharedPtr<D3D10ScreenColorRenderView>(settings.width,
																		settings.height,
																		settings.color_fmt));
		mScreenBuffer->attach(ATT_DepthStencil,
							  MakeSharedPtr<D3D10DepthStencilRenderView>(settings.width,
																		 settings.height,
																		 settings.depth_stencil_fmt));
		((WindowsWindow*)mWindow.get())->setFrameBuffer(mScreenBuffer);

		ID3D10RenderTargetView* screenRenderView = ((D3D10ScreenColorRenderView*)mScreenBuffer->attached(ATT_Color0).get())->getD3D10RenderTargetView();
		mDevice->OMSetRenderTargets(1, 
									&screenRenderView, 
									((D3D10DepthStencilRenderView*)mScreenBuffer->attached(ATT_DepthStencil).get())->getD3D10DepthStencilView());

		D3D10_RASTERIZER_DESC rasterDesc;
		ZeroMemory(&rasterDesc, sizeof(rasterDesc));
		rasterDesc.AntialiasedLineEnable = false;
		rasterDesc.CullMode = D3D10_CULL_BACK;
		rasterDesc.DepthBias = 0;
		rasterDesc.DepthBiasClamp = 0.f;
		rasterDesc.DepthClipEnable = true;
		rasterDesc.FillMode = D3D10_FILL_SOLID;
		rasterDesc.FrontCounterClockwise = false;
		rasterDesc.MultisampleEnable = settings.fsaa_samples > 0;
		rasterDesc.ScissorEnable = false;
		rasterDesc.SlopeScaledDepthBias = 0.f;

		result = mDevice->CreateRasterizerState(&rasterDesc, &mRasterState);
		CHECK_RESULT(result);

		
		//float fieldOfView = (float)D3DX_PI / 4.0f;
		//float screenAspect = (float)settings.width / (float)settings.height;

	}

	void D3D10GraphicDevice::beginFrame() {
		mWindow->onFrameStart().raise(mWindow.get(), _NullEventArgs);
	}

	ID3D10Device* D3D10GraphicDevice::getD3DDevice() const {
		return mDevice;
	}

	IDXGISwapChain* D3D10GraphicDevice::getSwapChain() const {
		return mSwapChain;
	}

	void D3D10GraphicDevice::endFrame() {
		mWindow->onFrameEnd().raise(mWindow.get(), _NullEventArgs);
	}

	void D3D10GraphicDevice::beginRendering() {
		 FrameBuffer& fb = *this->getScreenFrameBuffer();
        
        FrameCounter& counter = FrameCounter::Instance();
        AppInstance& app = Context::Instance().getApp();
        
        while(true) {

            if(fb.isActive()) {
                counter.waitToNextFrame();
                
                {            
                    MIST_PROFILE(L"__MainFrame__");

					D3D10_VIEWPORT viewport;
					viewport.Width = fb.getViewport().width;
					viewport.Height = fb.getViewport().height;
					viewport.TopLeftX = fb.getViewport().left;
					viewport.TopLeftY = fb.getViewport().top;

					mDevice->RSSetViewports(1, &viewport);
                   
                    setViewMatrix(fb.getViewport().camera->getViewMatrix());
                    setProjectionMatrix(fb.getViewport().camera->getProjMatrix());
                    
                    app.update();
                    app.render();
                    
                    if(mWindow->pullEvents())
                        break;
                    
                    fb.swapBuffers();
                }
                
            }
        }
	}

	void D3D10GraphicDevice::onRenderBuffer(const RenderBufferPtr& buffer) {

	}

	void D3D10GraphicDevice::onBindFrameBuffer(const FrameBufferPtr& frameBuffer) {

	}

	void D3D10GraphicDevice::setViewMatrix(const Matrix4& mat) {
		for(int row = 0; row < 4; ++row)
			for(int col = 0; col < 4; ++col)
				mViewMatrix(row, col) = mat.c[col][row];
	}

	void D3D10GraphicDevice::setProjectionMatrix(const Matrix4& mat) {
		for(int row = 0; row < 4; ++row)
			for(int col = 0; col < 4; ++col)
				mProjectionMatrix(row, col) = mat.c[col][row];
	}

	void D3D10GraphicDevice::getViewMatrix(Matrix4& mat) {
		for(int row = 0; row < 4; ++row)
			for(int col = 0; col < 4; ++col)
				mat.c[col][row] = mViewMatrix(row, col);
	}

	void D3D10GraphicDevice::getProjectionMatrix(Matrix4& mat) {
		for(int row = 0; row < 4; ++row)
			for(int col = 0; col < 4; ++col)
				mat.c[col][row] = mProjectionMatrix(row, col);
	}

	void D3D10GraphicDevice::bindTexture(const TexturePtr& texture) {

	}

	SharedPtr<uint8> D3D10GraphicDevice::readFrameBufferData(const FrameBufferPtr& buffer, int32 x, int32 y, uint32 width, uint32 height, ElementFormat format) {
		return SharedPtr<uint8>();
	}

	SharedPtr<uint8> D3D10GraphicDevice::readTextureData(const TexturePtr& texture, uint8 level) {
		return SharedPtr<uint8>();
	}

	void D3D10GraphicDevice::updateTextureData(const TexturePtr& texture, void* data, int32 x, int32 y, uint32 width, uint32 height, uint8 level) {

	}

	void D3D10GraphicDevice::fillGraphicCaps(GraphicDeviceCaps& caps) {

	}

	void D3D10GraphicDevice::setRenderState(RenderStateType type, uint32 func) {

	}


} // namespace ukn