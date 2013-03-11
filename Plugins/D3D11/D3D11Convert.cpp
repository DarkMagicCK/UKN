#include "D3D11Convert.h"
#include <D3DX10math.h>

#include "mist/Convert.h"
#include "ukn/Vertex.h"

namespace ukn {

    void D3DXMatrixToMatrix4(const D3DXMATRIX& d3dmat, Matrix4& mat) {
        for(int col = 0; col < 4; ++col) {
            for(int row = 0; row < 4; ++row) {
                mat.c[col][row] = d3dmat(row, col);
            }
        }
    }

    void Matrix4ToD3DMatrix(const Matrix4& mat, D3DXMATRIX& d3dmat) {
        for(int col = 0; col < 4; ++col) {
            for(int row = 0; row < 4; ++row) {
                d3dmat(row, col) = mat.c[col][row];
            }
        }
    }

    DXGI_FORMAT ElementFormatToDxGIFormat(ukn::ElementFormat format) {
        switch(format) {
        case ElementFormat::EF_RGBA8:
            return DXGI_FORMAT_R8G8B8A8_UNORM;
        case ElementFormat::EF_RGB565:
            return DXGI_FORMAT_B5G6R5_UNORM;
        case ElementFormat::EF_RGB5A1:
            return DXGI_FORMAT_B5G5R5A1_UNORM;
        case ElementFormat::EF_RGBA4444:
            return DXGI_FORMAT_UNKNOWN; /* rgba4444? */
        case ElementFormat::EF_D16:
            return DXGI_FORMAT_D16_UNORM;
        case ElementFormat::EF_D24S8:
            return DXGI_FORMAT_D24_UNORM_S8_UINT;
        case ElementFormat::EF_D32:
            return DXGI_FORMAT_D32_FLOAT;
        case ElementFormat::EF_Float4:
            return DXGI_FORMAT_R32G32B32A32_FLOAT;
        case ElementFormat::EF_Float3:
            return DXGI_FORMAT_R32G32B32_FLOAT;
        case ElementFormat::EF_Float2:
            return DXGI_FORMAT_R32G32_FLOAT;
        case ElementFormat::EF_Float:
            return DXGI_FORMAT_R32_FLOAT;
        case ElementFormat::EF_UInt32:
            return DXGI_FORMAT_R8G8B8A8_UNORM;
        case ElementFormat::EF_RGBA64:
            return DXGI_FORMAT_R16G16B16A16_UNORM;
        case ElementFormat::EF_RG32:
            return DXGI_FORMAT_R16G16_UNORM;
        case ElementFormat::EF_HalfFloat2:
            return DXGI_FORMAT_R16G16_FLOAT;
        case ElementFormat::EF_HalfFloat4:
            return DXGI_FORMAT_R16G16B16A16_FLOAT;
        }
    }

    ElementFormat DxGIFormatToElementFormat(DXGI_FORMAT format) {
        switch(format) {
        case DXGI_FORMAT_R8G8B8A8_UNORM:
            return ElementFormat::EF_RGBA8;
        case DXGI_FORMAT_B5G6R5_UNORM:
            return ElementFormat::EF_RGB565;
        case DXGI_FORMAT_B5G5R5A1_UNORM:
            return ElementFormat::EF_RGB5A1;
        case DXGI_FORMAT_UNKNOWN:
            return ElementFormat::EF_RGBA4444; /* rgba4444? */
        case DXGI_FORMAT_D16_UNORM:
            return ElementFormat::EF_D16;
        case DXGI_FORMAT_D24_UNORM_S8_UINT:
            return ElementFormat::EF_D24S8;
        case DXGI_FORMAT_D32_FLOAT:
            return ElementFormat::EF_D32;
        case DXGI_FORMAT_R32G32B32A32_FLOAT:
            return ElementFormat::EF_Float4;
        case DXGI_FORMAT_R32G32B32_FLOAT:
            return ElementFormat::EF_Float3;
        case DXGI_FORMAT_R32G32_FLOAT:
            return ElementFormat::EF_Float2;
        case DXGI_FORMAT_R32_FLOAT:
            return ElementFormat::EF_Float;
        case DXGI_FORMAT_R16G16B16A16_UNORM:
            return ElementFormat::EF_RGBA64;
        case DXGI_FORMAT_R16G16_UNORM:
            return ElementFormat::EF_RG32;
        case DXGI_FORMAT_R16G16_FLOAT:
            return ElementFormat::EF_HalfFloat2;
        case DXGI_FORMAT_R16G16B16A16_FLOAT:
            return ElementFormat::EF_HalfFloat4;
        }
    }

    const char* VertexUsageToSemanticName(ukn::VertexUsage usage) {
        switch(usage) {
        case VU_Binormal: return "BINORMAL";
        case VU_Tangent: return "TANGENT";
        case VU_UV: return "TEXCOORD";
        case VU_Position: return "POSITION";
        case VU_Diffuse: return "COLOR";
        case VU_Specular: return "COLOR1";
        case VU_Normal: return "NORMAL";
        case VU_BlendWeight: return "BLENDWEIGHT";
        case VU_BlendIndices: return "BLENDINDICIES";
        case VU_PSize: return "PSIZE";
        }
    }

    D3D11_PRIMITIVE_TOPOLOGY RenderModeToPrimitiveTopology(ukn::RenderMode mode) {
        switch(mode) {
        case ukn::RenderMode::RM_Line: return D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
        case ukn::RenderMode::RM_LineLoop: return D3D11_PRIMITIVE_TOPOLOGY_LINELIST_ADJ;
        case ukn::RenderMode::RM_Point: return D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
        case ukn::RenderMode::RM_Triangle: return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
        case ukn::RenderMode::RM_TriangleFan: return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST; /* to fix */
        case ukn::RenderMode::RM_TriangleStrip: return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
        }
    }

}