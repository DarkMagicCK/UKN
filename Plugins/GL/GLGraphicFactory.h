//
//  GLGraphicFactory.h
//  Project Unknown
//
//  Created by Ruiwei Bu on 11/24/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_GLGraphicFactory_h
#define Project_Unknown_GLGraphicFactory_h

#include "UKN/Platform.h"
#include "UKN/PreDeclare.h"

#include "UKN/GraphicFactory.h"

#ifdef UKN_HAS_DECLSPEC
    #ifdef UKN_GL_AS_SOURCE					 // Build dll
        #define UKN_GL_API __declspec(dllexport)
    #elif defined(UKN_GL_AS_LIBRARY)								     // Use dll
        #define UKN_GL_API __declspec(dllimport)
    #else
		#define UKN_GL_API
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
