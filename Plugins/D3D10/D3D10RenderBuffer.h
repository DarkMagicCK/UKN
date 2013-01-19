#include "UKN/RenderBuffer.h"

namespace ukn {

	class D3D10Effect;
	class D3D10GraphicDevice;

	class D3D10RenderBuffer: public RenderBuffer {
	public:
		D3D10RenderBuffer(D3D10GraphicDevice* device);
		virtual ~D3D10RenderBuffer();
		
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
		D3D10GraphicDevice* mDevice;
	};

}