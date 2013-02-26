#include "UKN/RenderBuffer.h"

class ID3D11InputLayout;

namespace ukn {

	class D3D11Effect;
	class D3D11GraphicDevice;

	class D3D11RenderBuffer: public RenderBuffer {
	public:
		D3D11RenderBuffer(D3D11GraphicDevice* device);
		virtual ~D3D11RenderBuffer();

	private:
		D3D11GraphicDevice* mDevice;
	};

}