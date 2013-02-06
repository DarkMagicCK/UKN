//
//  GLBlendStateObject.h
//  Project Unknown
//
//  Created by Robert Bu on 2/5/13.
//  Copyright (c) 2013 heizi. All rights reserved.
//

#ifndef __Project_Unknown__GLBlendStateObject__
#define __Project_Unknown__GLBlendStateObject__

#include "GLPreq.h"
#include "UKN/BlendStateObject.h"

namespace ukn {
    
    //
    
    class GLBlendStateObject: public BlendStateObject {
    public:
        GLBlendStateObject(const BlendStateDesc& desc):
        BlendStateObject(desc) {
            
        }
        virtual ~GLBlendStateObject() { }
        
    };
    
}

#endif /* defined(__Project_Unknown__GLBlendStateObject__) */
