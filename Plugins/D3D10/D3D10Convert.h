
#ifndef Project_Unknown_D3D10Convert_h
#define Project_Unknown_D3D10Convert_h


#include "mist/Platform.h"
#include "mist/MathUtil.h"
#include "UKN/GraphicSettings.h"

#include <d3d10.h>

namespace ukn {

    inline void D3DXMatrixToMatrix4(const D3DXMATRIX& d3dmat, Matrix4& mat) {
        for(int col = 0; col < 4; ++col) {
            for(int row = 0; row < 4; ++row) {
                mat.c[col][row] = d3dmat(row, col);
            }
        }
    }

    inline void Matrix4ToD3DMatrix(const Matrix4& mat, D3DXMATRIX& d3dmat) {
        for(int col = 0; col < 4; ++col) {
            for(int row = 0; row < 4; ++row) {
                d3dmat(row, col) = mat.c[col][row];
            }
        }
    }

	inline DXGI_FORMAT ElementFormatToDxGIFormat(ukn::ElementFormat format) {
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
		}
	}

	inline D3D10_PRIMITIVE_TOPOLOGY RenderModeToPrimitiveTopology(ukn::RenderMode mode) {
		switch(mode) {
		case ukn::RenderMode::RM_Line: return D3D10_PRIMITIVE_TOPOLOGY_LINELIST;
		case ukn::RenderMode::RM_LineLoop: return D3D10_PRIMITIVE_TOPOLOGY_LINELIST_ADJ;
		case ukn::RenderMode::RM_Point: return D3D10_PRIMITIVE_TOPOLOGY_POINTLIST;
		case ukn::RenderMode::RM_Triangle: return D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		case ukn::RenderMode::RM_TriangleFan: return D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST; /* to fix */
		case ukn::RenderMode::RM_TriangleStrip: return D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
		}
	}


} // namespace ukn

#endif