//
//  GLGraphicFactory.h
//  Project Unknown
//
//  Created by Robert Bu on 11/24/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_GLGraphicFactory_h
#define Project_Unknown_GLGraphicFactory_h

#include "mist/Platform.h"
#include "UKN/PreDeclare.h"
#include "UKN/GraphicFactory.h"

#ifdef MIST_HAS_DECLSPEC
    #if defined(UKN_GL_AS_SOURCE) || defined(GLPLUGIN_EXPORTS)				 // Build dll
        #define UKN_GL_API __declspec(dllexport)
    #else							     // Use dll
        #define UKN_GL_API __declspec(dllimport)
	#endif
#else
#define UKN_GL_API
#endif // UKN_HAS_DECLSPEC

namespace ukn {
    
    extern "C" { 
        UKN_GL_API void CreateGraphicFactory(GraphicFactoryPtr& ptr);
    }
}


#endif
