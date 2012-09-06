//
//  GLShader.h
//  Project Unknown
//
//  Created by Robert Bu on 9/5/12.
//  Copyright (c) 2012 heizi. All rights reserved.
//

#ifndef __Project_Unknown__GLShader__
#define __Project_Unknown__GLShader__

#include "UKN/Shader.h"

namespace ukn {
    
    class GLShader: public Shader {
    public:
        GLShader();
        virtual ~GLShader();
        
    public:
        // Shader
        virtual void bind();
        virtual void unbind();
    };
    
}

#endif /* defined(__Project_Unknown__GLShader__) */
