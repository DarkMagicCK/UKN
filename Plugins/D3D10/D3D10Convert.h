
#ifndef Project_Unknown_D3D10Convert_h
#define Project_Unknown_D3D10Convert_h


#include "mist/Platform.h"
#include "UKN/GraphicSettings.h"

#include <d3d10.h>

namespace ukn {

	inline DXGI_FORMAT ColorFormatToDxGIFormat(ukn::ElementFormat format) {
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


} // namespace ukn

#endif