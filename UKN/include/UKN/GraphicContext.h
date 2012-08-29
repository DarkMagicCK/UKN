//
//  RenderHelper.h
//  Project Unknown
//
//  Created by Ruiwei Bu on 1/20/12.
//  Copyright (c) 2012 heizi. All rights reserved.
//

#ifndef Project_Unknown_RenderHelper_h
#define Project_Unknown_RenderHelper_h

#include "UKN/Platform.h"
#include "UKN/PreDeclare.h"
#include "UKN/Renderable.h"

namespace ukn {
    
    class GraphicContext: public virtual IRenderable, public virtual Object {
    public:
        GraphicContext();
        virtual ~GraphicContext();
        
        const String& getName() const;
        
        Box getBound() const;
        RenderBufferPtr getRenderBuffer() const;
        
        void onRenderBegin();
        void onRenderEnd();
        
        void render();
        
        void moveTo(float x, float y);
        void lineTo(float x, float y);
        
    private:
        GraphicBufferPtr mVertexBuffer;
        RenderBufferPtr mRenderBuffer;
    };
    
} // namespace ukn

#endif
