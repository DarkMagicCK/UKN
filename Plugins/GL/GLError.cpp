//
//  GLError.cpp
//  Project Unknown
//
//  Created by Robert Bu on 4/13/13.
//  Copyright (c) 2013 heizi. All rights reserved.
//

#include "GLError.h"
#include "GLPreq.h"
#include "mist/Convert.h"

namespace ukn {
    
    bool check_gl_error(const char* f) {
        int err = glGetError();
        if(err != GL_NO_ERROR) {
            const wchar_t* msg = L"Unknown Error";
            switch(err) {
                case GL_INVALID_ENUM:
                    msg = L"Invalid Enum";
                    break;
                case GL_INVALID_OPERATION:
                    msg = L"Invalid Operation";
                    break;
                case GL_INVALID_VALUE:
                    msg = L"Invalid Value";
                    break;
                case GL_INVALID_FRAMEBUFFER_OPERATION:
                    msg = L"Invalid FrameBuffer Operation";
                    break;
            }
            if(!f)
                f = "unknown";
            log_error((mist::FormatString(L"GL Error when calling {0}: {1}"), f, msg));
            return false;
        }
        return true;
    }
    
}