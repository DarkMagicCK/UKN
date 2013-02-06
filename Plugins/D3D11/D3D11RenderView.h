
#ifndef Project_Unknown_D3D11RenderView_h
#define Project_Unknown_D3D11RenderView_h

#include "mist/Platform.h"
#include "UKN/RenderView.h"

#include "D3D11Preq.h"

namespace ukn {

	class D3D11GraphicDevice;
	class D3D11Texture2D;

	class D3D11RenderView: public RenderView {
	public:
		D3D11RenderView(D3D11GraphicDevice* device);
		virtual ~D3D11RenderView();

		virtual void clearDepth(float depth);
		virtual void clearStencil(int32 stencil);
		virtual void clearDepthStencil(float depth, int32 stencil);

	protected:
		D3D11GraphicDevice* mGraphicDevice;
	};

	typedef SharedPtr<D3D11RenderView> D3D11RenderViewPtr;

    class D3D11RenderTargetView: public D3D11RenderView {
    public:
        D3D11RenderTargetView(D3D11GraphicDevice* device);

        ID3D11RenderTargetView* getD3D11RenderTargetView() const;

    protected:
        COM<ID3D11RenderTargetView>::Ptr mRenderTargetView;
    };

	class D3D11ScreenColorRenderView: Uncopyable, public D3D11RenderTargetView {
	public:
		D3D11ScreenColorRenderView(uint32 width, uint32 height, ElementFormat ef, D3D11GraphicDevice* device);
		virtual ~D3D11ScreenColorRenderView();

		void clearColor(const Color& clr);
		void clearDepth(float depth);
		void clearStencil(int32 stencil);
		void clearDepthStencil(float depth, int32 stencil);

		void onAttached(FrameBuffer& fb, uint32 att);
		void onDetached(FrameBuffer& fb, uint32 att);
	};

	typedef SharedPtr<D3D11ScreenColorRenderView> D3D11ScreenColorRenderViewPtr;

	class D3D11DepthStencilRenderView: Uncopyable, public D3D11RenderView {
	public:
        D3D11DepthStencilRenderView(const TexturePtr& texture, D3D11GraphicDevice* device);
		D3D11DepthStencilRenderView(uint32 width, uint32 height, ElementFormat ef, uint32 sampleCount, uint32 sampleQuality, D3D11GraphicDevice* device);
		virtual ~D3D11DepthStencilRenderView();

		void clearColor(const Color& clr);
		void clearDepth(float depth);
		void clearStencil(int32 stencil);
		void clearDepthStencil(float depth, int32 stencil);

		void onAttached(FrameBuffer& fb, uint32 att);
		void onDetached(FrameBuffer& fb, uint32 att);

		void enableDepth(bool flag);

		ID3D11DepthStencilView* getD3D11DepthStencilView() const;

	private:
        void createDSView(ElementFormat ef, int32 sampleCount);

        TexturePtr mTexture;
		ID3D11Texture2D* mDepthStencilBuffer;
		ID3D11DepthStencilView* mDepthStencilView;
		ID3D11DepthStencilState* mDepthStencilState;
		/* for 2d rendering */
		ID3D11DepthStencilState* mDepthStencilDisabledState;
	};

	typedef SharedPtr<D3D11DepthStencilRenderView> D3D11DepthStencilRenderViewPtr;

	
	class D3D11Texture2DRenderView: Uncopyable, public D3D11RenderTargetView {
	public:
		D3D11Texture2DRenderView(const TexturePtr& texture, D3D11GraphicDevice* device);
		virtual ~D3D11Texture2DRenderView();

		void clearColor(const Color& clr);

		void onAttached(FrameBuffer& fb, uint32 att);
		void onDetached(FrameBuffer& fb, uint32 att);

	private:
		TexturePtr mTexture;
	};

	typedef SharedPtr<D3D11Texture2DRenderView> D3D11Texture2DRenderViewPtr;

} // namespace ukn

#endif