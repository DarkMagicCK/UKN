//
//  GLSamplerStateObject.h
//  Project Unknown
//
//  Created by Robert Bu on 2/5/13.
//  Copyright (c) 2013 heizi. All rights reserved.
//

#ifndef __Project_Unknown__GLSamplerStateObject__
#define __Project_Unknown__GLSamplerStateObject__

#include "GLPreq.h"
#include "UKN/SamplerStateObject.h"

namespace ukn {
    
    // opengl introduced sampler object in 3.3
    // but for backward capability
    // just simulate here for framebufers
    
    class GLSamplerStateObject: public SamplerStateObject {
    public:
        GLSamplerStateObject(const SamplerStateDesc& desc):
        SamplerStateObject(desc) {
            
        }
        virtual ~GLSamplerStateObject() { }
        
    };
    
}

#endif /* defined(__Project_Unknown__GLSamplerStateObject__) */
