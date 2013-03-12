#include "D3D11GraphicDevice.h"
#include "D3D11FrameBuffer.h"
#include "D3D11RenderView.h"
#include "D3D11GraphicFactory.h"
#include "D3D11BlendStateObject.h"
#include "D3D11SamplerStateObject.h"
#include "D3D11RasterizerStateObject.h"
#include "D3D11DepthStencilStateObject.h"
#include "D3D11GraphicBuffer.h"
#include "CgShader.h"

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
        mDebug(0) {
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
        return UknString(L"D3D11 Device, ") + (mFeatureLevel == D3D_FEATURE_LEVEL_10_1 ? L"Feature Level 10.1" : L"Feature Level 11");
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

        ID3D11Device* device;
        IDXGISwapChain* swapChain;
        ID3D11DeviceContext* deviceContext;
        for(int i=0; i < sizeof(driverTypes) / sizeof(D3D_DRIVER_TYPE); ++i) {
            result = D3D11CreateDeviceAndSwapChain(NULL,
                driverTypes[i],
                NULL,
                creationFlags,
                &mFeatureLevel,
                1,
                D3D11_SDK_VERSION,
                &swapChainDesc,
                &swapChain,
                &device,
                NULL,
                &deviceContext);
            if(D3D11Debug::CHECK_RESULT(result, L"Create Device with D3D11_DRIVE_TYPE_HARDWARE")) 
                break;
        }
        mDevice = MakeCOMPtr(device);
        mSwapChain = MakeCOMPtr(swapChain);
        mDeviceContext = MakeCOMPtr(deviceContext);

#if defined(MIST_DEBUG)
        mDebug.reset(new D3D11Debug(mDevice.get()));
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
            settings.sample_count,
            settings.sample_quality,
            this));

        ((WindowsWindow*)mWindow.get())->setFrameBuffer(mScreenFrameBuffer);
        ((WindowsWindow*)mWindow.get())->updateWindowProperties(0, 0, settings.width, settings.height);
        this->bindFrameBuffer(mScreenFrameBuffer);

        return true;
    }

    ID3D11Device* D3D11GraphicDevice::getD3DDevice() const {
        return mDevice.get();
    }

    IDXGISwapChain* D3D11GraphicDevice::getSwapChain() const {
        return mSwapChain.get();
    }

    ID3D11DeviceContext* D3D11GraphicDevice::getD3DDeviceContext() const {
        return mDeviceContext.get();
    }

    void D3D11GraphicDevice::beginFrame() {
        mWindow->onFrameStart().raise(mWindow.get(), _NullEventArgs);
    }

    void D3D11GraphicDevice::endFrame() {
        mWindow->onFrameEnd().raise(mWindow.get(), _NullEventArgs);
    }

    void D3D11GraphicDevice::setViewport(const Viewport& vp) {
        D3D11_VIEWPORT viewport;
        viewport.Width = (FLOAT)vp.width;
        viewport.Height = (FLOAT)vp.height;
        viewport.TopLeftX = (FLOAT)vp.left;
        viewport.TopLeftY = (FLOAT)vp.top;
        viewport.MaxDepth = 1.0f;
        viewport.MinDepth = 0.0f;
        mDeviceContext->RSSetViewports(1, &viewport);
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

                    
                    
                    fb.getViewport().camera->update();

                    this->setViewport(fb.getViewport());
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

    void D3D11GraphicDevice::renderBuffer(const EffectTechniquePtr& technique, const RenderBufferPtr& buffer) {
        mist_assert(buffer.isValid());

        const GraphicBufferPtr& vertexBuffer = buffer->getVertexStream();
        if(!vertexBuffer.isValid()) {
            log_error("ukn::GLGraphicDevice::onRenderBuffer: invalid vertex buffer stream");
            return;
        }

        const GraphicBufferPtr& indexBuffer = buffer->getIndexStream();

        D3D11RenderBuffer* d3dBuffer = checked_cast<D3D11RenderBuffer*>(buffer.get());
        if(d3dBuffer) {
            const RenderBuffer::VertexStreamVec& vertex_streams = d3dBuffer->getVertexStreams();
            std::vector<ID3D11Buffer*> vertex_buffer_arr;
            std::vector<UINT> strides;
            std::vector<UINT> offsets;

            for(const RenderBuffer::StreamObject& so: vertex_streams) {
                D3D11VertexBuffer* vb = checked_cast<D3D11VertexBuffer*>(so.stream.get());
                if(vb) {
                    vertex_buffer_arr.push_back(vb->getD3DBuffer());
                    offsets.push_back(0);
                    strides.push_back(so.vertex_size);
                }
            }
            if(d3dBuffer->hasInstanceStream()) {
                D3D11VertexBuffer* vb = checked_cast<D3D11VertexBuffer*>(d3dBuffer->getInstanceStream().get());
                if(vb) {
                    vertex_buffer_arr.push_back(vb->getD3DBuffer());
                    offsets.push_back(0);
                    strides.push_back(d3dBuffer->getInstanceFormatSize());
                }
            }

            if(vertex_buffer_arr.empty()) {
                return;
            }
            mDeviceContext->IASetVertexBuffers(0, 
                                               vertex_buffer_arr.size(),
                                               &vertex_buffer_arr[0],
                                               &strides[0],
                                               &offsets[0]);

            if(indexBuffer.isValid() &&
                buffer->isUseIndexStream()) {
                    indexBuffer->activate();
            } 

            mDeviceContext->IASetPrimitiveTopology(RenderModeToPrimitiveTopology(buffer->getRenderMode()));

            for(int i=0; i < mSamplerStates.size(); ++i) {
                D3D11SamplerStateObject* samplerObj = checked_cast<D3D11SamplerStateObject*>(mSamplerStates[i].get());
                if(samplerObj) {
                    ID3D11SamplerState* d3d11samplerState = samplerObj->getD3DSamplerState();
                    mDeviceContext->PSSetSamplers(i, 1, &d3d11samplerState);
                }
            }
            if(mSamplerStates.size() < mCurrFrameBuffer->colorViewSize()) {
                for(int i=mSamplerStates.size(); i<mCurrFrameBuffer->colorViewSize(); ++i) {
                    ID3D11SamplerState* empty_state = 0;
                    mDeviceContext->PSSetSamplers(i, 1, &empty_state);
                }
            }

            for(uint32 i=0; i<technique->getNumPasses(); ++i) {
                const EffectPassPtr& pass = technique->getPass(i);
                CgDxShader* shader = (CgDxShader*)pass->getVertexShader().get();
                ID3D10Blob* blob = shader->getCompiledProgram();
                mDeviceContext->IASetInputLayout(d3dBuffer->inputLayout(blob->GetBufferPointer(),
                                                                        blob->GetBufferSize()));
                   
                pass->begin();
                if(buffer->hasInstanceStream()) {
                    if(indexBuffer.isValid() &&
                        buffer->isUseIndexStream()) {
                            mDeviceContext->DrawIndexedInstanced(
                                buffer->getIndexCount(),
                                buffer->getInstanceCount(),
                                buffer->getIndexStartIndex(),
                                buffer->getVertexStartIndex(),
                                buffer->getInstanceStartIndex());
                    } else {
                        mDeviceContext->DrawInstanced(
                            buffer->getVertexCount(),
                            buffer->getInstanceCount(),
                            buffer->getVertexStartIndex(),
                            buffer->getInstanceStartIndex());
                    }
                } else {
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
                pass->end();
            }
        }
    }

    void D3D11GraphicDevice::onBindFrameBuffer(const FrameBufferPtr& frameBuffer) {
        Viewport& vp = frameBuffer->getViewport();
        this->setViewport(vp);
    }

    void D3D11GraphicDevice::setViewMatrix(const Matrix4& mat) {
        mViewMatrix = mat;
    }

    void D3D11GraphicDevice::setProjectionMatrix(const Matrix4& mat) {
        mProjectionMatrix = mat;
    }

    void D3D11GraphicDevice::getViewMatrix(Matrix4& mat) const {
        mat = mViewMatrix;
    }

    void D3D11GraphicDevice::getProjectionMatrix(Matrix4& mat) const {
        mat = mProjectionMatrix;
    }

    void D3D11GraphicDevice::bindTexture(const TexturePtr& texture) {
        
    }

    void D3D11GraphicDevice::fillGraphicCaps(GraphicDeviceCaps& caps) {
        caps.max_texture_width = 8192;
        caps.max_texture_height = 8192;
    }

    void D3D11GraphicDevice::setRenderState(RenderStateType type, uint32 func) {

    }

    D3D_FEATURE_LEVEL D3D11GraphicDevice::getDeviceFeatureLevel() const {
        return mFeatureLevel;
    }

    void D3D11GraphicDevice::onSetBlendState(const BlendStatePtr& blendState) {
        if(blendState) {
            D3D11BlendStateObject* blendObj = checked_cast<D3D11BlendStateObject*>(blendState.get());
            if(blendObj) {
                mDeviceContext->OMSetBlendState(
                    blendObj->getD3DBlendState(),
                    blendObj->getDesc().blend_factor.value,
                    0xffffffff);
            }
        }
    }

    void D3D11GraphicDevice::onSetSamplerState(const SamplerStatePtr& samplerState, uint32 index) {
        if(samplerState) {
            D3D11SamplerStateObject* samplerObj = checked_cast<D3D11SamplerStateObject*>(samplerState.get());
            if(samplerObj) {
                ID3D11SamplerState* d3d11samplerState = samplerObj->getD3DSamplerState();
                mDeviceContext->PSSetSamplers(index, 1, &d3d11samplerState);
            }
        } else {
            ID3D11SamplerState* empty_state = 0;
            mDeviceContext->PSSetSamplers(index, 1, &empty_state);
        }
    }

    void D3D11GraphicDevice::onSetRasterizerState(const RasterizerStatePtr& rasterizerState) {
        if(rasterizerState) {
            D3D11RasterizerStateObject* rasterizerObj = checked_cast<D3D11RasterizerStateObject*>(rasterizerState.get());
            if(rasterizerObj) {
                mDeviceContext->RSSetState(rasterizerObj->getD3D11RasterizerState());
            }
        }
    }
     void D3D11GraphicDevice::onSetDepthStencilState(const DepthStencilStatePtr& depthStencilState) {
         if(depthStencilState) {
             D3D11DepthStencilStateObject* depthStencilObject = checked_cast<D3D11DepthStencilStateObject*>(depthStencilState.get());
             if(depthStencilObject) {
                 mDeviceContext->OMSetDepthStencilState(depthStencilObject->getD3DDepthStencilState(), 
                                                        depthStencilObject->getDesc().stencil_ref);
             }
         }
     }
} // namespace ukn