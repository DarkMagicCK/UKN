//
//  Viewport.h
//  Project Unknown
//
//  Created by Ruiwei Bu on 11/23/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_Viewport_h
#define Project_Unknown_Viewport_h

#include "Platform.h"
#include "Camera.h"

namespace ukn {

    struct UKN_API Viewport {
        int left;
        int top;
        int width;
        int height;
        
        CameraPtr camera;
        
        Viewport():
        camera(MakeSharedPtr<Camera2D>()) {
            
        }
        
        Viewport(int l, int t, int w, int h):
        left(l),
        top(t),
        width(w),
        height(h),
        camera(MakeSharedPtr<Camera2D>()) {
            
        }
    };
    
} // namespace ukn


#endif
