//
//  GLError.h
//  Project Unknown
//
//  Created by Robert Bu on 4/13/13.
//  Copyright (c) 2013 heizi. All rights reserved.
//

#ifndef __Project_Unknown__GLError__
#define __Project_Unknown__GLError__

namespace ukn {
    
#ifdef MIST_DEBUG
#define CHECK_GL_CALL(X) \
    X; \
    check_gl_error(#X);
    
#define CHECK_GL_CALL_APPLY(f, ...) \
    f(__VA_ARGS__); \
    check_gl_error(#f);
    
#else
#define CHECK_GL_CALL(X) \
    X;
    
#define CHECK_GL_CALL_APPLY(f, ...) \
    f(__VA_ARGS__); 
    
#endif

    
    
    extern bool check_gl_error(const char* f);
    extern bool check_framebuffer_status();
    
}

#endif /* defined(__Project_Unknown__GLError__) */
