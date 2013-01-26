#include "D3D11Preq.h"
#include "ukn/Vertex.h"

namespace ukn {

    struct D3D11ShaderUtilities {
		static ID3D11InputLayout* CreateLayout(ID3D11Device* device, const void* signature, SIZE_T size, const ukn::VertexFormat& format);
		static void LogError(ID3D10Blob* error);
	};
}