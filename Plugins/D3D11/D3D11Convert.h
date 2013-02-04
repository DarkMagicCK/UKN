
#ifndef Project_Unknown_D3D11Convert_h
#define Project_Unknown_D3D11Convert_h


#include "mist/Platform.h"
#include "mist/MathUtil.h"

#include "UKN/Vertex.h"
#include "UKN/GraphicSettings.h"

#include <D3DX11.h>

    struct D3DXMATRIX;
   
namespace ukn {

    void D3DXMatrixToMatrix4(const D3DXMATRIX& d3dmat, Matrix4& mat);
    void Matrix4ToD3DMatrix(const Matrix4& mat, D3DXMATRIX& d3dmat);
	DXGI_FORMAT ElementFormatToDxGIFormat(ukn::ElementFormat format);
    const char* VertexUsageToSemanticName(ukn::VertexUsage usage);
    D3D11_PRIMITIVE_TOPOLOGY RenderModeToPrimitiveTopology(ukn::RenderMode mode);


} // namespace ukn

#endif