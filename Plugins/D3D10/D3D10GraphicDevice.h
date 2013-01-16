
#ifndef Project_Unknown_D3D10GraphicDevice_h
#define Project_Unknown_D3D10GraphicDevice_h


#include "mist/Platform.h"

#include "UKN/GraphicDevice.h"
#include "UKN/Texture.h"

#include "D3D10Preq.h"

namespace ukn {

	class D3D10Effect;

	class D3D10GraphicDevice: public GraphicDevice {
	public:
		D3D10GraphicDevice();
		virtual ~D3D10GraphicDevice();

		UknString description() const;
        
        WindowPtr doCreateRenderWindow(const UknString& name, const RenderSettings& settings);
        
        void beginFrame();
        void endFrame();
        void beginRendering();
        
        void onRenderBuffer(const RenderBufferPtr& buffer);
        void onBindFrameBuffer(const FrameBufferPtr& frameBuffer);
        
        void setViewMatrix(const Matrix4& mat);
        void setProjectionMatrix(const Matrix4& mat);
        
        void getViewMatrix(Matrix4& mat);
        void getProjectionMatrix(Matrix4& mat);
        
        void bindTexture(const TexturePtr& texture);
        
        SharedPtr<uint8> readFrameBufferData(const FrameBufferPtr& buffer, int32 x, int32 y, uint32 width, uint32 height, ElementFormat format);
        SharedPtr<uint8> readTextureData(const TexturePtr& texture, uint8 level);
        void  updateTextureData(const TexturePtr& texture, void* data, int32 x, int32 y, uint32 width, uint32 height, uint8 level);
        
        void fillGraphicCaps(GraphicDeviceCaps& caps);
        
        void setRenderState(RenderStateType type, uint32 func);

		ID3D10Device* getD3DDevice() const;
        IDXGISwapChain* getSwapChain() const;

		void bindEffect(D3D10Effect* effect);

    private:
		bool initD3DDevice(const RenderSettings& settings, HWND hWnd);

        WindowPtr mWindow;
        TexturePtr mCurrTexture;

		ID3D10Device* mDevice;
		IDXGISwapChain* mSwapChain;
		ID3D10RasterizerState* mRasterState;

		D3DXMATRIX mWorldMatrix;
		D3DXMATRIX mProjectionMatrix;
		D3DXMATRIX mViewMatrix;

		D3D10Effect* mEffect;
	};

} // namespace ukn

#endif