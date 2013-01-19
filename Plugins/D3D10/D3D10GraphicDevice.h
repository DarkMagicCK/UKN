
#ifndef Project_Unknown_D3D10GraphicDevice_h
#define Project_Unknown_D3D10GraphicDevice_h


#include "mist/Platform.h"

#include "UKN/GraphicDevice.h"
#include "UKN/Texture.h"

#include "D3D10Preq.h"

namespace ukn {

	class D3D10Effect;
	class D3D10Debug;

	class D3D10GraphicDevice: public GraphicDevice {
	public:
		D3D10GraphicDevice();
		virtual ~D3D10GraphicDevice();

		UknString description() const override;
        
        WindowPtr doCreateRenderWindow(const UknString& name, const RenderSettings& settings);
        
        void beginFrame() override;
        void endFrame() override;
        void beginRendering() override;
        
        void renderBuffer(const RenderBufferPtr& buffer) override;
        void onBindFrameBuffer(const FrameBufferPtr& frameBuffer) override;
        
        void setViewMatrix(const Matrix4& mat) override;
        void setProjectionMatrix(const Matrix4& mat) override;
        
        void getViewMatrix(Matrix4& mat) override;
        void getProjectionMatrix(Matrix4& mat) override;
        
        void bindTexture(const TexturePtr& texture) override;
     	void bindEffect(const EffectPtr& effect) override;

        SharedPtr<uint8> readFrameBufferData(const FrameBufferPtr& buffer, int32 x, int32 y, uint32 width, uint32 height, ElementFormat format) override;
        SharedPtr<uint8> readTextureData(const TexturePtr& texture, uint8 level) override;
        void  updateTextureData(const TexturePtr& texture, void* data, int32 x, int32 y, uint32 width, uint32 height, uint8 level) override;
        
        void fillGraphicCaps(GraphicDeviceCaps& caps) override;
        
        void setRenderState(RenderStateType type, uint32 func) override;

		ID3D10Device* getD3DDevice() const;
        IDXGISwapChain* getSwapChain() const;

    private:
		bool initD3DDevice(const RenderSettings& settings, HWND hWnd);
		void onWindowResize();

        WindowPtr mWindow;
        TexturePtr mCurrTexture;
        EffectPtr mEffect;

		ID3D10Device* mDevice;
		IDXGISwapChain* mSwapChain;
		ID3D10RasterizerState* mRasterState;
		ID3D10BlendState* mBlendState;

		Matrix4 mWorldMatrix;
		Matrix4 mProjectionMatrix;
		Matrix4 mViewMatrix;

		D3D10Debug* mDebug;
	};

} // namespace ukn

#endif