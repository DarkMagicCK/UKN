
#ifndef Project_Unknown_D3D11GraphicDevice_h
#define Project_Unknown_D3D11GraphicDevice_h


#include "mist/Platform.h"
#include "mist/Ptr.h"

#include "UKN/GraphicDevice.h"
#include "UKN/Texture.h"

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
        
        void renderBuffer(const RenderBufferPtr& buffer) override;
        void onBindFrameBuffer(const FrameBufferPtr& frameBuffer) override;
        
        void setViewMatrix(const Matrix4& mat) override;
        void setProjectionMatrix(const Matrix4& mat) override;
        void setWorldMatrix(const Matrix4& mat) override;

        void getViewMatrix(Matrix4& mat) const override;
        void getProjectionMatrix(Matrix4& mat) const override;
        void getWorldMatrix(Matrix4& mat) const override;

        void bindTexture(const TexturePtr& texture) override;
     	void bindEffect(const EffectPtr& effect) override;

        void fillGraphicCaps(GraphicDeviceCaps& caps) override;
        
        void setRenderState(RenderStateType type, uint32 func) override;

		ID3D11Device* getD3DDevice() const;
        ID3D11DeviceContext* getD3DDeviceContext() const;
        IDXGISwapChain* getSwapChain() const;

    private:
		bool initD3DDevice(const RenderSettings& settings, HWND hWnd);
		void onWindowResize();

        WindowPtr mWindow;
        TexturePtr mCurrTexture;
        EffectPtr mEffect;

		ID3D11Device* mDevice;
        ID3D11DeviceContext* mDeviceContext;
		IDXGISwapChain* mSwapChain;
		ID3D11RasterizerState* mRasterState;
		ID3D11BlendState* mBlendState;

		Matrix4 mWorldMatrix;
		Matrix4 mProjectionMatrix;
		Matrix4 mViewMatrix;

		ScopedPtr<D3D11Debug> mDebug;
	};

} // namespace ukn

#endif