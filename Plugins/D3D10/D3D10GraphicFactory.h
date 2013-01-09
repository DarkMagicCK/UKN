
#ifndef Project_Unknown_D3D10GraphicFactory_h
#define Project_Unknown_D3D10GraphicFactory_h

#include "mist/Platform.h"
#include "UKN/GraphicFactory.h"

#ifdef MIST_HAS_DECLSPEC
    #ifdef UKN_GL_AS_SOURCE					 // Build dll
        #define UKN_D3D10_API __declspec(dllexport)
    #elif defined(UKN_GL_AS_LIBRARY)								     // Use dll
        #define UKN_D3D10_API __declspec(dllimport)
    #else
		#define UKN_D3D10_API
	#endif
#else
#define UKN_D3D10_API
#endif // UKN_HAS_DECLSPEC

namespace ukn {

	extern "C" { 
        UKN_D3D10_API void CreateGraphicFactoryD3D(GraphicFactoryPtr& ptr);
    }

} // namespace ukn

#endif