//
//  Renderable.h
//  Project Unknown
//
//  Created by Robert Bu on 11/23/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_Renderable_h
#define Project_Unknown_Renderable_h

#include "UKN/Platform.h"
#include "UKN/PreDeclare.h"
#include "UKN/MathUtil.h"
#include "UKN/StringUtil.h"

namespace ukn {
    
    class UKN_API IRenderable: public virtual Interface {
    public:
        virtual const String& getName() const = 0;
        
        virtual Box getBound() const = 0;
        virtual RenderBufferPtr getRenderBuffer() const = 0;
        
        virtual void onRenderBegin() = 0;
        virtual void onRenderEnd() = 0;
        
        virtual void render() = 0;
    };
    
    
} // namespace ukn

#endif
