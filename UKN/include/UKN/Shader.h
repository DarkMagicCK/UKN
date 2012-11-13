//
//  Shader.h
//  Project Unknown
//
//  Created by Robert Bu on 11/23/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_Shader_h
#define Project_Unknown_Shader_h

#include "ukn/PreDeclare.h"

namespace ukn {
    
    struct ShaderDesc {
        UknString func;
        UknString profile;
    };
    
    class Shader {
    public:
        enum ShaderType {
            ST_FragmentShader,
            ST_VertexShader,
            
            // geometry shaders to do
        };
        
        virtual void bind() = 0;
        virtual void unbind() = 0;
    };
    
} // namespace ukn



#endif
