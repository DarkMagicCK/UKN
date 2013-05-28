
#ifndef Project_Unknown_D3D11GraphicDevice_h
#define Project_Unknown_D3D11GraphicDevice_h


#include "mist/Platform.h"
#include "mist/Ptr.h"

#include "UKN/GraphicDevice.h"
#include "UKN/Texture.h"
#include "UKN/Viewport.h"

#include "D3D11Preq.h"

namespace ukn {

    class D3D11Effect;
    class D3D11Debug;

    class D3D11GraphicDevice: public GraphicDevice {
    public:
        D3D11GraphicDevice();
        virtual ~D3D11GraphicDevice();

        UknString description() const override;
        
        WindowPtr doCreateRenderWindow(const UknString& name, const RenderSettings& settings);
        
        void beginFrame() override;
        void endFrame() override;
        void beginRendering() override;
        
        void renderBuffer(const EffectTechniquePtr& technique, const RenderBufferPtr& buffer) override;
        void onBindFrameBuffer(const FrameBufferPtr& frameBuffer) override;
        
        void setViewMatrix(const Matrix4& mat) override;
        void setProjectionMatrix(const Matrix4& mat) override;
        
        void getViewMatrix(Matrix4& mat) const override;
        void getProjectionMatrix(Matrix4& mat) const override;
        
        void bindTexture(const TexturePtr& texture) override;
     	
        void fillGraphicCaps(GraphicDeviceCaps& caps) override;
        
        void setRenderState(RenderStateType type, uint32 func) override;

        void onSetBlendState(const BlendStatePtr& blendState) override;
        void onSetSamplerState(const SamplerStatePtr& samplerState, uint32 index) override;
        void onSetRasterizerState(const RasterizerStatePtr& rasterizerState) override;
        void onSetDepthStencilState(const DepthStencilStatePtr& depthstentialState) override;

        ID3D11Device* getD3DDevice() const;
        ID3D11DeviceContext* getD3DDeviceContext() const;
        IDXGISwapChain* getSwapChain() const;
        ID3D11SamplerState* getD3DSamplerState() const;

        /* get device feature level, this flag is required to determine shader profiles etc*/ 
        D3D_FEATURE_LEVEL getDeviceFeatureLevel() const;

    private:
        void setViewport(const Viewport& vp);
        bool initD3DDevice(const RenderSettings& settings, HWND hWnd);
        void onWindowResize();

        WindowPtr mWindow;

        COM<ID3D11Device>::Ptr mDevice;
        COM<ID3D11DeviceContext>::Ptr mDeviceContext;
        COM<IDXGISwapChain>::Ptr mSwapChain;

        Matrix4 mProjectionMatrix;
        Matrix4 mViewMatrix;

        ScopedPtr<D3D11Debug> mDebug;
        D3D_FEATURE_LEVEL mFeatureLevel;

        ID3D11SamplerState* mSamplerState;
    };

} // namespace ukn

#endif