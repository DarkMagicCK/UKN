#include "UKN/RenderBuffer.h"

class ID3D11InputLayout;

namespace ukn {

	class D3D11Effect;
	class D3D11GraphicDevice;

	class D3D11RenderBuffer: public RenderBuffer {
	public:
		D3D11RenderBuffer(D3D11GraphicDevice* device);
		virtual ~D3D11RenderBuffer();
		
	protected:
		virtual void onBindVertexStream(GraphicBufferPtr vertexStream, const VertexFormat& format);
		virtual void onSetVertexFormat(const VertexFormat& format);
		virtual void onBindIndexStream(GraphicBufferPtr indexStream);
		virtual void onSetIndexCount(uint32 count);
		virtual void onSetVertexCount(uint32 count);
		virtual void onSetIndexStartIndex(uint32 index);
		virtual void onUseIndexStream(bool flag);
        virtual void onSetEffect(const EffectPtr& effect);

	private:
		D3D11GraphicDevice* mDevice;
	};

}