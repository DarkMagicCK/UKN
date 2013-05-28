
#ifndef Project_Unknown_D3D11Convert_h
#define Project_Unknown_D3D11Convert_h


#include "mist/Platform.h"
#include "mist/MathUtil.h"

#include "UKN/Vertex.h"
#include "UKN/GraphicSettings.h"

#include <D3D11.h>

namespace ukn {

	DXGI_FORMAT ElementFormatToDxGIFormat(ukn::ElementFormat format);
    ElementFormat DxGIFormatToElementFormat(DXGI_FORMAT format);
    
    D3D11_BIND_FLAG TextureBindFlagToD3D11Flag(ukn::TextureBindFlag flag);
    const char* VertexUsageToSemanticName(ukn::VertexUsage usage);
    D3D11_PRIMITIVE_TOPOLOGY RenderModeToPrimitiveTopology(ukn::RenderMode mode);


} // namespace ukn

#endif