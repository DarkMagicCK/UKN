//
//  RenderTarget.h
//  Project Unknown
//
//  Created by Griffin me on 1/23/12.
//  Copyright (c) 2012 heizi. All rights reserved.
//

#ifndef Project_Unknown_RenderTarget_h
#define Project_Unknown_RenderTarget_h

#include "UKN/Platform.h"
#include "UKN/PreDeclare.h"

namespace ukn {
    
    /**
     * A render target is a composit of a texture render view and a depth stencil render view(optimal)
     **/
    
    class RenderTarget {
    public:
            
    private:
        RenderViewPtr mTarget;
        RenderViewPtr mDepthStencil;
    };
    
} // namespace ukn


#endif
