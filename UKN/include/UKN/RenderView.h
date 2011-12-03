//
//  RenderView.h
//  Project Unknown
//
//  Created by Robert Bu on 11/23/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_RenderView_h
#define Project_Unknown_RenderView_h

#include "Platform.h"
#include "PreDeclare.h"
#include "GraphicSettings.h"
#include "Color.h"

namespace ukn {
    
    class UKN_API RenderView {
    public:
        static RenderViewPtr NullObject();
        
    public:
        virtual ~RenderView() {
            
        }
        
        uint32 getWidth() const {
            return mWidth;
        }
        
        uint32 getHeight() const {
            return mHeight;
        }
        
        uint32 getBpp() const {
            // todo elements formats;
            return 32;
        }
        
        virtual void clearColor(const Color& clr) = 0;
        virtual void clearDepth(float depth) = 0;
        virtual void clearStencil(int32 stencil) = 0;
        virtual void clearDepthStencil(float depth, int32 stencil) = 0;
        
        virtual void onAttached(FrameBuffer& fb, uint32 att) = 0;
        virtual void onDetached(FrameBuffer& fb, uint32 att) = 0;
        
        virtual void onBind(FrameBuffer& fb, uint32 att);
        virtual void onUnbind(FrameBuffer& fb, uint32 att);
        
    protected:
        uint32 mWidth, mHeight;
        
        ElementFormat mElementFormat;
        // todo element formats
        // uint32 mColorDepth;
    };
    
} // namespace ukn


#endif
