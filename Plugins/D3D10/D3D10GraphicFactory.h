
#ifndef Project_Unknown_D3D10GraphicFactory_h
#define Project_Unknown_D3D10GraphicFactory_h

#include "mist/Platform.h"
#include "UKN/GraphicFactory.h"

#ifdef MIST_HAS_DECLSPEC
    #ifdef D3D10PLUGIN_EXPORTS				 // Build dll
        #define UKN_D3D10_API __declspec(dllexport)
    #else								     // Use dll
        #define UKN_D3D10_API __declspec(dllimport)
	#endif
#else
#define UKN_D3D10_API
#endif // UKN_HAS_DECLSPEC

namespace ukn {

	extern "C" { 
        UKN_D3D10_API void CreateGraphicFactory(GraphicFactoryPtr& ptr);
    }

} // namespace ukn

#endif