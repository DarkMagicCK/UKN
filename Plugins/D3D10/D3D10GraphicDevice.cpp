#include "D3D10GraphicDevice.h"
#include "D3D10FrameBuffer.h"
#include "D3D10RenderView.h"

#include "WindowsWindow.h"

#include "UKN/Context.h"
#include "UKN/App.h"
#include "UKN/RenderBuffer.h"
#include "UKN/GraphicBuffer.h"

#include "mist/Profiler.h"

#include "D3D10Shader.h"
#include "D3D10RenderBuffer.h"
#include "D3D10Debug.h"

#pragma comment(lib, "d3d10.lib")
#pragma comment(lib, "d3dx10.lib")
#pragma comment(lib, "dxgi.lib")

namespace ukn {

	D3D10GraphicDevice::D3D10GraphicDevice():
		mDevice(0),
		mEffect(0) {

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
		return mWindow;
	}

	bool D3D10GraphicDevice::initD3DDevice(const RenderSettings& settings, HWND hWnd) {
		#define CHECK_RESULT_AND_RETURN(result, mssg) if(!D3D10Debug::CHECK_RESULT(result, mssg)) return false;

		IDXGIFactory* factory;
		HRESULT result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
		CHECK_RESULT_AND_RETURN(result, L"CreateDXGIFactory");

		IDXGIAdapter* adapter;
		result = factory->EnumAdapters(0, &adapter);
		CHECK_RESULT_AND_RETURN(result, L"EnumAdapters");

		IDXGIOutput* adapterOutput;
		result = adapter->EnumOutputs(0, &adapterOutput);
		CHECK_RESULT_AND_RETURN(result, L"IDXGIAdapter->EnumOutputs");

		DXGI_FORMAT colorFormat = ElementFormatToDxGIFormat(settings.color_fmt);

		unsigned int numModes;
		result = adapterOutput->GetDisplayModeList(colorFormat,
			DXGI_ENUM_MODES_INTERLACED,
			&numModes,
			NULL);
		CHECK_RESULT_AND_RETURN(result, L"IDXGIAdapter->GetDisplayModeList");

		DXGI_MODE_DESC* displayModeList = new DXGI_MODE_DESC[numModes];
		result = adapterOutput->GetDisplayModeList(colorFormat,
			DXGI_ENUM_MODES_INTERLACED,
			&numModes,
			displayModeList);
		CHECK_RESULT_AND_RETURN(result, L"IDXGIAdapter->GetDisplayModeList");

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
		CHECK_RESULT_AND_RETURN(result, L"IDXGIAdapter->GetDesc");

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

		swapChainDesc.SampleDesc.Count = settings.sample_count;
		swapChainDesc.SampleDesc.Quality = settings.sample_quality;

		swapChainDesc.Windowed = !settings.full_screen;

		swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		swapChainDesc.Flags = 0;

		DWORD creationFlags = 0;
#if defined(MIST_DEBUG)
		creationFlags |= D3D10_CREATE_DEVICE_DEBUG;
#endif

		result = D3D10CreateDeviceAndSwapChain(NULL,
			D3D10_DRIVER_TYPE_HARDWARE,
			NULL,
			creationFlags,
			D3D10_SDK_VERSION,
			&swapChainDesc,
			&mSwapChain,
			&mDevice);
		if(!D3D10Debug::CHECK_RESULT(result, L"Create Device with D3D10_DRIVE_TYPE_HARDWARE")) {
			/* if hardware creation failed, try software */
			result = D3D10CreateDeviceAndSwapChain(NULL,
				D3D10_DRIVER_TYPE_WARP /* software rasterizer */,
				NULL,
				creationFlags,
				D3D10_SDK_VERSION,
				&swapChainDesc,
				&mSwapChain,
				&mDevice);
			CHECK_RESULT_AND_RETURN(result, L"D3D10CreateDeviceAndSwapChain");
		}

#if defined(MIST_DEBUG)
		mDebug = new D3D10Debug(mDevice);
#endif

		mScreenFrameBuffer = MakeSharedPtr<D3D10FrameBuffer>(false, this);
		mScreenFrameBuffer->attach(ATT_Color0,
			MakeSharedPtr<D3D10ScreenColorRenderView>(settings.width,
			settings.height,
			settings.color_fmt,
			this));
		mScreenFrameBuffer->attach(ATT_DepthStencil,
			MakeSharedPtr<D3D10DepthStencilRenderView>(settings.width,
			settings.height,
			settings.depth_stencil_fmt,
			this));

		((WindowsWindow*)mWindow.get())->setFrameBuffer(mScreenFrameBuffer);
		((WindowsWindow*)mWindow.get())->updateWindowProperties(0, 0, settings.width, settings.height);
		this->bindFrameBuffer(mScreenFrameBuffer);

		try {
			ID3D10RenderTargetView* screenRenderView = ((D3D10ScreenColorRenderView*)mScreenFrameBuffer->attached(ATT_Color0).get())->getD3D10RenderTargetView();
			mDevice->OMSetRenderTargets(1, 
				&screenRenderView, 
				((D3D10DepthStencilRenderView*)mScreenFrameBuffer->attached(ATT_DepthStencil).get())->getD3D10DepthStencilView());
		} catch(mist::Exception& exp) {
			MessageBoxW(mWindow->getHWnd(),
						L"Error",
						L"Error",
						MB_OK | MB_ICONERROR);
			return false;
		}

		D3D10_RASTERIZER_DESC rasterDesc;
		ZeroMemory(&rasterDesc, sizeof(rasterDesc));
		rasterDesc.AntialiasedLineEnable = false;
		rasterDesc.CullMode = D3D10_CULL_BACK;
		rasterDesc.DepthBias = 0;
		rasterDesc.DepthBiasClamp = 0.f;
		rasterDesc.DepthClipEnable = true;
		rasterDesc.FillMode = D3D10_FILL_SOLID;
		rasterDesc.FrontCounterClockwise = false;
		rasterDesc.MultisampleEnable = settings.sample_quality > 0;
		rasterDesc.ScissorEnable = false;
		rasterDesc.SlopeScaledDepthBias = 0.f;

		result = mDevice->CreateRasterizerState(&rasterDesc, &mRasterState);
		CHECK_RESULT_AND_RETURN(result, L"ID3D10Device->CreateRasterizerState");

		D3DXMatrixIdentity(&mWorldMatrix);

		/* default D3D10 Blend State */
		D3D10_BLEND_DESC blendDesc;
		ZeroMemory(&blendDesc, sizeof(blendDesc));
		blendDesc.BlendEnable[0] = FALSE;
		blendDesc.RenderTargetWriteMask[0] = D3D10_COLOR_WRITE_ENABLE_ALL;
		blendDesc.BlendOp = D3D10_BLEND_OP_ADD;
		blendDesc.SrcBlend = D3D10_BLEND_ONE;
		blendDesc.DestBlend = D3D10_BLEND_ZERO;
		blendDesc.SrcBlendAlpha = D3D10_BLEND_ONE;
		blendDesc.DestBlendAlpha = D3D10_BLEND_ZERO;
		blendDesc.BlendOpAlpha = D3D10_BLEND_OP_ADD;
		CHECK_RESULT_AND_RETURN(mDevice->CreateBlendState(&blendDesc, &mBlendState),
								L"ID3D10Device->CreateBlendState");
		
		return true;
	}

	ID3D10Device* D3D10GraphicDevice::getD3DDevice() const {
		return mDevice;
	}

	IDXGISwapChain* D3D10GraphicDevice::getSwapChain() const {
		return mSwapChain;
	}

	void D3D10GraphicDevice::beginFrame() {
		mWindow->onFrameStart().raise(mWindow.get(), _NullEventArgs);
	}

	void D3D10GraphicDevice::endFrame() {
		mWindow->onFrameEnd().raise(mWindow.get(), _NullEventArgs);
	}

	void D3D10GraphicDevice::beginRendering() {
		FrameBuffer& fb = *this->getScreenFrameBuffer();

		FrameCounter& counter = FrameCounter::Instance();
		AppLauncher& app = Context::Instance().getApp();

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
					viewport.MaxDepth = 1.0f;
					viewport.MinDepth = 0.0f;

					mDevice->RSSetViewports(1, &viewport);
					mDevice->OMSetBlendState(mBlendState, 0, 0xffffffff);

					this->setViewMatrix(fb.getViewport().camera->getViewMatrix());
					this->setProjectionMatrix(fb.getViewport().camera->getProjMatrix());

					app.update();
					app.render();
					
					fb.swapBuffers();

					if(mWindow->pullEvents())
						break;

#if defined(MIST_DEBUG)
					if(mDebug) mDebug->logAvailableMessages();
#endif
				}

			}
		}
	}

	void D3D10GraphicDevice::onRenderBuffer(const RenderBufferPtr& buffer) {
		mist_assert(buffer.isValid());

		GraphicBufferPtr vertexBuffer = buffer->getVertexStream();
		if(!vertexBuffer.isValid()) {
			log_error("ukn::GLGraphicDevice::onRenderBuffer: invalid vertex buffer stream");
			return;
		}

		GraphicBufferPtr indexBuffer = buffer->getIndexStream();

		D3D10RenderBuffer* d3d10buffer = checked_cast<D3D10RenderBuffer*>(buffer.get());
		if(d3d10buffer) {
			SharedPtr<D3D10Effect> effect = d3d10buffer->getEffect();

			if(effect.isValid()) {
				if(!effect->setMatrixVariable("worldMatrix", mWorldMatrix))
					log_error("error setting world matrix in effect");
				if(!effect->setMatrixVariable("projectionMatrix", mProjectionMatrix))
					log_error("error setting projection matrix in effect");
				if(!effect->setMatrixVariable("viewMatrix", mViewMatrix))
					log_error("error setting view matrix in effect");
				if(mCurrTexture) {
					if(!effect->setShaderResourceVariable("tex", 
						(ID3D10ShaderResourceView*)mCurrTexture->getTextureId()))
						log_error("error setting texture in effect");
				}

				vertexBuffer->activate();
				if(indexBuffer.isValid()) {
					indexBuffer->activate();
				} 

				mDevice->IASetPrimitiveTopology(RenderModeToPrimitiveTopology(buffer->getRenderMode()));
				
				effect->bind();
				for(uint32 i=0; i<effect->getPasses(); ++i) {
					effect->applyPass(i);

					if(indexBuffer.isValid()) {
						mDevice->DrawIndexed(buffer->getIndexCount(),
							buffer->getIndexStartIndex(),
							buffer->getVertexStartIndex());
					} else {
						mDevice->Draw(buffer->getVertexCount(),
							buffer->getVertexStartIndex());
					}
				}
			}
		}
	}

	void D3D10GraphicDevice::onBindFrameBuffer(const FrameBufferPtr& frameBuffer) {

	}

	void D3D10GraphicDevice::setViewMatrix(const Matrix4& mat) {
		memcpy((float*)&mViewMatrix, mat.x, sizeof(float) * 16);
	}

	void D3D10GraphicDevice::setProjectionMatrix(const Matrix4& mat) {
		memcpy((float*)&mProjectionMatrix, mat.x, sizeof(float) * 16);
	}

	void D3D10GraphicDevice::getViewMatrix(Matrix4& mat) {
		memcpy(mat.x, (float*)&mViewMatrix, sizeof(float) * 16);
	}

	void D3D10GraphicDevice::getProjectionMatrix(Matrix4& mat) {
		memcpy(mat.x, (float*)&mProjectionMatrix, sizeof(float) * 16);
	}

	void D3D10GraphicDevice::bindTexture(const TexturePtr& texture) {
		mCurrTexture = texture;
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
		caps.max_texture_width = 8192;
		caps.max_texture_height = 8192;
	}

	void D3D10GraphicDevice::setRenderState(RenderStateType type, uint32 func) {

	}

	void D3D10GraphicDevice::bindEffect(D3D10Effect* effect) {
		mEffect = effect;
	}


} // namespace ukn