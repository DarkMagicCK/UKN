//
//  Viewport.h
//  Project Unknown
//
//  Created by Robert Bu on 11/23/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_Viewport_h
#define Project_Unknown_Viewport_h

#include "mist/Platform.h"
#include "UKN/Camera.h"

namespace ukn {

    struct UKN_API Viewport {
        int left;
        int top;
        int width;
        int height;
        
        CameraPtr camera;
        
        Viewport() {
            
        }
        
        Viewport(int l, int t, int w, int h):
        left(l),
        top(t),
        width(w),
        height(h) {
            
        }
    };
    
} // namespace ukn


#endif
