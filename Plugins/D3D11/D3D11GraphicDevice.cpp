#include "D3D11GraphicDevice.h"
#include "D3D11FrameBuffer.h"
#include "D3D11RenderView.h"
#include "D3D11GraphicFactory.h"

#include "WindowsWindow.h"

#include "UKN/Context.h"
#include "UKN/App.h"
#include "UKN/RenderBuffer.h"
#include "UKN/GraphicBuffer.h"
#include "UKN/CgHelper.h"

#include "mist/Stream.h"
#include "mist/Profiler.h"

#include "D3D11Shader.h"
#include "D3D11RenderBuffer.h"
#include "D3D11Debug.h"

#pragma comment(lib, "D3D11.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3dx10.lib")
#pragma comment(lib, "dxgi.lib")

namespace ukn {

    D3D11GraphicDevice::D3D11GraphicDevice():
    mDevice(0),
    mEffect(0),
    mDebug(0),
    mIs2D(false) {
            /*
            LH to RH and fix z
            www.klayge.org/2011/07/15
            1  0   0   0
            0  1   0   0
            0  0   2   0
            0  0   -1  1
            */
    }

    D3D11GraphicDevice::~D3D11GraphicDevice() {

    }

    UknString D3D11GraphicDevice::description() const {
        return L"D3D11 Device";
    }

    WindowPtr D3D11GraphicDevice::doCreateRenderWindow(const UknString& name, const RenderSettings& settings) {
        mWindow = MakeSharedPtr<WindowsWindow>(name, settings);
        if(!mWindow) {
            MIST_THROW_EXCEPTION(L"Error creating window");
        }
        if(!this->initD3DDevice(settings, mWindow->getHWnd())) {
            MIST_THROW_EXCEPTION(L"Error initializing D3D11 device");
        }
        return mWindow;
    }

    bool D3D11GraphicDevice::initD3DDevice(const RenderSettings& settings, HWND hWnd) {
#define CHECK_RESULT_AND_RETURN(result, mssg) if(!D3D11Debug::CHECK_RESULT(result, mssg)) return false;

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
        creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

        D3D_DRIVER_TYPE driverTypes[] = {D3D_DRIVER_TYPE_HARDWARE, D3D_DRIVER_TYPE_WARP};
        mFeatureLevel = D3D_FEATURE_LEVEL_10_1;

        for(int i=0; i < sizeof(driverTypes) / sizeof(D3D_DRIVER_TYPE); ++i) {
            result = D3D11CreateDeviceAndSwapChain(NULL,
                D3D_DRIVER_TYPE_HARDWARE,
                NULL,
                creationFlags,
                &mFeatureLevel,
                1,
                D3D11_SDK_VERSION,
                &swapChainDesc,
                &mSwapChain,
                &mDevice,
                NULL,
                &mDeviceContext);
            if(D3D11Debug::CHECK_RESULT(result, L"Create Device with D3D11_DRIVE_TYPE_HARDWARE")) 
                break;
        }

#if defined(MIST_DEBUG)
        mDebug.reset(new D3D11Debug(mDevice));
#endif

        mScreenFrameBuffer = MakeSharedPtr<D3D11FrameBuffer>(false, this);
        mScreenFrameBuffer->attach(ATT_Color0,
            MakeSharedPtr<D3D11ScreenColorRenderView>(settings.width,
            settings.height,
            settings.color_fmt,
            this));
        mScreenFrameBuffer->attach(ATT_DepthStencil,
            MakeSharedPtr<D3D11DepthStencilRenderView>(settings.width,
            settings.height,
            settings.depth_stencil_fmt,
            this));

        ((WindowsWindow*)mWindow.get())->setFrameBuffer(mScreenFrameBuffer);
        ((WindowsWindow*)mWindow.get())->updateWindowProperties(0, 0, settings.width, settings.height);
        this->bindFrameBuffer(mScreenFrameBuffer);

        try {
            ID3D11RenderTargetView* screenRenderView = ((D3D11ScreenColorRenderView*)mScreenFrameBuffer->attached(ATT_Color0).get())->getD3D11RenderTargetView();
            mDeviceContext->OMSetRenderTargets(1, 
                &screenRenderView, 
                ((D3D11DepthStencilRenderView*)mScreenFrameBuffer->attached(ATT_DepthStencil).get())->getD3D11DepthStencilView());
        } catch(mist::Exception& exp) {
            MessageBoxW(mWindow->getHWnd(),
                L"Error",
                L"Error",
                MB_OK | MB_ICONERROR);
            return false;
        }

        D3D11_RASTERIZER_DESC rasterDesc;
        ZeroMemory(&rasterDesc, sizeof(rasterDesc));
        rasterDesc.AntialiasedLineEnable = false;
        rasterDesc.CullMode = D3D11_CULL_NONE;
        rasterDesc.DepthBias = 0;
        rasterDesc.DepthBiasClamp = 0.f;
        rasterDesc.DepthClipEnable = true;
        rasterDesc.FillMode = D3D11_FILL_SOLID;
        rasterDesc.FrontCounterClockwise = false;
        rasterDesc.MultisampleEnable = settings.sample_quality > 0;
        rasterDesc.ScissorEnable = false;
        rasterDesc.SlopeScaledDepthBias = 0.f;

        result = mDevice->CreateRasterizerState(&rasterDesc, &mRasterState);
        CHECK_RESULT_AND_RETURN(result, L"ID3D11Device->CreateRasterizerState");
        mDeviceContext->RSSetState(mRasterState);

        mWorldMatrix = Matrix4();

        /* default D3D11 Blend State */
        D3D11_BLEND_DESC blendDesc;
        ZeroMemory(&blendDesc, sizeof(blendDesc));

        blendDesc.AlphaToCoverageEnable = FALSE;
        blendDesc.IndependentBlendEnable = FALSE;
        blendDesc.RenderTarget[0].BlendEnable = FALSE;
        blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
        blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
        blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
        blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
        blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
        blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
        CHECK_RESULT_AND_RETURN(mDevice->CreateBlendState(&blendDesc, &mBlendState),
            L"ID3D11Device->CreateBlendState");

        return true;
    }

    ID3D11Device* D3D11GraphicDevice::getD3DDevice() const {
        return mDevice;
    }

    IDXGISwapChain* D3D11GraphicDevice::getSwapChain() const {
        return mSwapChain;
    }

    void D3D11GraphicDevice::beginFrame() {
        mWindow->onFrameStart().raise(mWindow.get(), _NullEventArgs);
    }

    void D3D11GraphicDevice::endFrame() {
        mWindow->onFrameEnd().raise(mWindow.get(), _NullEventArgs);
    }

    void D3D11GraphicDevice::beginRendering() {
        FrameBuffer& fb = *this->getScreenFrameBuffer();

        FrameCounter& counter = FrameCounter::Instance();
        AppLauncher& app = Context::Instance().getApp();

        while(true) {

            if(fb.isActive()) {
                counter.waitToNextFrame();

                {            
                    MIST_PROFILE(L"__MainFrame__");

                    D3D11_VIEWPORT viewport;
                    viewport.Width = fb.getViewport().width;
                    viewport.Height = fb.getViewport().height;
                    viewport.TopLeftX = fb.getViewport().left;
                    viewport.TopLeftY = fb.getViewport().top;
                    viewport.MaxDepth = 1.0f;
                    viewport.MinDepth = 0.0f;

                    mDeviceContext->RSSetViewports(1, &viewport);
                    mDeviceContext->OMSetBlendState(mBlendState, 0, 0xffffffff);

                    fb.getViewport().camera->update();

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

    void D3D11GraphicDevice::renderBuffer(const RenderBufferPtr& buffer) {
        mist_assert(buffer.isValid());

        GraphicBufferPtr vertexBuffer = buffer->getVertexStream();
        if(!vertexBuffer.isValid()) {
            log_error("ukn::GLGraphicDevice::onRenderBuffer: invalid vertex buffer stream");
            return;
        }

        GraphicBufferPtr indexBuffer = buffer->getIndexStream();

        D3D11RenderBuffer* D3D11buffer = checked_cast<D3D11RenderBuffer*>(buffer.get());
        if(D3D11buffer) {
            EffectPtr effect = mIs2D ? m2DEffect: buffer->getEffect();
            if(effect) {
                /* temporary */
                if(effect->getVertexShader()) {
                    if(!effect->getVertexShader()->setMatrixVariable("worldMatrix", mWorldMatrix))
                        log_error("error setting world matrix in effect");
                    if(!effect->getVertexShader()->setMatrixVariable("projectionMatrix", mProjectionMatrix))
                        log_error("error setting projection matrix in effect");
                    if(!effect->getVertexShader()->setMatrixVariable("viewMatrix", mViewMatrix))
                        log_error("error setting view matrix in effect");
                }
                if(effect->getFragmentShader()) {
                    if(mCurrTexture) {
                        if(!effect->getFragmentShader()->setTextureVariable("tex", mCurrTexture))
                            log_error("error setting texture in effect");
                    }
                }

                vertexBuffer->activate();
                if(indexBuffer.isValid() &&
                    buffer->isUseIndexStream()) {
                        indexBuffer->activate();
                } 

                mDeviceContext->IASetPrimitiveTopology(RenderModeToPrimitiveTopology(buffer->getRenderMode()));

                for(uint32 i=0; i<effect->getPasses(); ++i) {
                    effect->bind(i);

                    if(indexBuffer.isValid() &&
                        buffer->isUseIndexStream()) {
                            mDeviceContext->DrawIndexed(buffer->getIndexCount(),
                                buffer->getIndexStartIndex(),
                                buffer->getVertexStartIndex());
                    } else {
                        mDeviceContext->Draw(buffer->getVertexCount(),
                            buffer->getVertexStartIndex());
                    }
                }
            }
        }
    }

    void D3D11GraphicDevice::onBindFrameBuffer(const FrameBufferPtr& frameBuffer) {

    }

    void D3D11GraphicDevice::setViewMatrix(const Matrix4& mat) {
        mViewMatrix = mat;
    }

    void D3D11GraphicDevice::setProjectionMatrix(const Matrix4& mat) {
        mProjectionMatrix = mat;
    }

    void D3D11GraphicDevice::setWorldMatrix(const Matrix4& mat) {
        mWorldMatrix = mat;
    }

    void D3D11GraphicDevice::getViewMatrix(Matrix4& mat) const {
        mat = mViewMatrix;
    }

    void D3D11GraphicDevice::getProjectionMatrix(Matrix4& mat) const {
        mat = mProjectionMatrix;
    }

    void D3D11GraphicDevice::getWorldMatrix(Matrix4& mat) const {
        mat = mWorldMatrix;
    }

    void D3D11GraphicDevice::bindTexture(const TexturePtr& texture) {
        mCurrTexture = texture;
    }

    void D3D11GraphicDevice::fillGraphicCaps(GraphicDeviceCaps& caps) {
        caps.max_texture_width = 8192;
        caps.max_texture_height = 8192;
    }

    void D3D11GraphicDevice::setRenderState(RenderStateType type, uint32 func) {

    }

    void D3D11GraphicDevice::bindEffect(const EffectPtr& effect) {
        mEffect = effect;
    }

    ID3D11DeviceContext* D3D11GraphicDevice::getD3DDeviceContext() const {
        return mDeviceContext;
    }

    void D3D11GraphicDevice::begin2DRendering(const OrthogonalParams& params) {
        mIs2D = true;

        if(!m2DEffect) {
            m2DEffect = ukn::CreateCgEffet2D();
            if(m2DEffect) 
                this->bindEffect(m2DEffect);
            else 
                log_error("Error creating effect for 2d rendering");
        }
        
        float width = params.width;
        if(width == 0.f) width = mWindow->width();
        float height = params.height;
        if(height == 0.f) height = mWindow->height();

        this->setProjectionMatrix(
            mist::Matrix4::OrthoOffCenterMatLH(
                params.x,
                params.x + width,
                params.y + height,
                params.y,
                0.0,
                1.0f));

        Matrix4 viewMat;
        viewMat.translate(params.x + params.dx, params.y + params.dy, 0.f);
        viewMat.rotate(0.f, params.rotation, 0.f);
        viewMat.scale(params.scalex, params.scaley, 1.f);
        viewMat.translate(-params.dx, -params.dy, 0.f);
        this->setViewMatrix(viewMat);

        if(mCurrFrameBuffer) {
            RenderViewPtr dsView = mCurrFrameBuffer->attached(ATT_DepthStencil);
            if(dsView) dsView->enableDepth(false);
        }
    }

    void D3D11GraphicDevice::end2DRendering() {
        mIs2D = false;
        if(mCurrFrameBuffer) {
            CameraPtr cam = mCurrFrameBuffer->getViewport().camera;
            this->setProjectionMatrix(cam->getProjMatrix());
            this->setViewMatrix(cam->getViewMatrix());

            RenderViewPtr dsView = mCurrFrameBuffer->attached(ATT_DepthStencil);
            if(dsView) dsView->enableDepth(true);
        }
    }

    D3D_FEATURE_LEVEL D3D11GraphicDevice::getDeviceFeatureLevel() const {
        return mFeatureLevel;
    }

} // namespace ukn