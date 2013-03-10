#include "UKN/RenderBuffer.h"


struct ID3D11InputLayout;

namespace ukn {

	class D3D11Effect;
	class D3D11GraphicDevice;
    class D3D11VertexLayout;
    typedef SharedPtr<D3D11VertexLayout> D3D11VertexLayoutPtr;

	class D3D11RenderBuffer: public RenderBuffer {
	public:
		D3D11RenderBuffer(D3D11GraphicDevice* device);
		virtual ~D3D11RenderBuffer();

        ID3D11InputLayout* inputLayout(void* code, uint32 signature);

	private:
		D3D11GraphicDevice* mDevice;
        std::vector<D3D11VertexLayoutPtr> mLayouts;
	};

}