
#ifndef Project_Unknown_D3D10RenderView_h
#define Project_Unknown_D3D10RenderView_h

#include "mist/Platform.h"
#include "UKN/RenderView.h"

#include "D3D10Preq.h"

namespace ukn {

	class D3D10GraphicDevice;
	class D3D10Texture2D;

	class D3D10RenderView: public RenderView {
	public:
		D3D10RenderView(D3D10GraphicDevice* device);
		virtual ~D3D10RenderView();

		virtual void clearDepth(float depth);
		virtual void clearStencil(int32 stencil);
		virtual void clearDepthStencil(float depth, int32 stencil);

	protected:
		D3D10GraphicDevice* mGraphicDevice;
	};

	typedef SharedPtr<D3D10RenderView> D3D10RenderViewPtr;

	class D3D10ScreenColorRenderView: Uncopyable, public D3D10RenderView {
	public:
		D3D10ScreenColorRenderView(uint32 width, uint32 height, ElementFormat ef, D3D10GraphicDevice* device);
		virtual ~D3D10ScreenColorRenderView();

		void clearColor(const Color& clr);
		void clearDepth(float depth);
		void clearStencil(int32 stencil);
		void clearDepthStencil(float depth, int32 stencil);

		void onAttached(FrameBuffer& fb, uint32 att);
		void onDetached(FrameBuffer& fb, uint32 att);

		ID3D10RenderTargetView* getD3D10RenderTargetView() const;

	private:
		ID3D10RenderTargetView* mRenderView;
	};

	typedef SharedPtr<D3D10ScreenColorRenderView> D3D10ScreenColorRenderViewPtr;

	class D3D10DepthStencilRenderView: Uncopyable, public D3D10RenderView {
	public:
		D3D10DepthStencilRenderView(uint32 width, uint32 height, ElementFormat ef, D3D10GraphicDevice* device);
		virtual ~D3D10DepthStencilRenderView();

		void clearColor(const Color& clr);
		void clearDepth(float depth);
		void clearStencil(int32 stencil);
		void clearDepthStencil(float depth, int32 stencil);

		void onAttached(FrameBuffer& fb, uint32 att);
		void onDetached(FrameBuffer& fb, uint32 att);

		ID3D10DepthStencilView* getD3D10DepthStencilView() const;

	private:
		ID3D10Texture2D* mDepthStencilBuffer;
		ID3D10DepthStencilView* mDepthStencilView;
		ID3D10DepthStencilState* mDepthStencilState;
	};

	typedef SharedPtr<D3D10DepthStencilRenderView> D3D10DepthStencilRenderViewPtr;

	
	class D3D10Texture2DRenderView: Uncopyable, public D3D10RenderView {
	public:
		D3D10Texture2DRenderView(Texture& texture, int32 index, int32 level);
		virtual ~D3D10Texture2DRenderView();

		void clearColor(const Color& clr);

		void onAttached(FrameBuffer& fb, uint32 att);
		void onDetached(FrameBuffer& fb, uint32 att);

	private:
		D3D10Texture2D& mTexture;
		int32 mIndex;
		int32 mLevel;
	};

	typedef SharedPtr<D3D10Texture2DRenderView> D3D10Texture2DRenderViewPtr;

} // namespace ukn

#endif