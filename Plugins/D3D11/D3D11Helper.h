#include "D3D11Preq.h"
#include "ukn/Vertex.h"

namespace ukn {

    struct D3D11ShaderUtilities {
		static ID3D11InputLayout* CreateLayout(ID3D11Device* device, 
                                                const void* signature, 
                                                SIZE_T size, 
                                                const ukn::vertex_elements_type& format,
                                                uint32 instance_start,
                                                uint32 instance_end /* instance element start and end index */);
		static void LogError(ID3D10Blob* error);
	};
}