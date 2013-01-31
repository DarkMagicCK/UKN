//
//  RenderView.h
//  Project Unknown
//
//  Created by Robert Bu on 11/23/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_RenderView_h
#define Project_Unknown_RenderView_h

#include "mist/Platform.h"
#include "mist/Color.h"

#include "UKN/PreDeclare.h"
#include "UKN/GraphicSettings.h"

namespace ukn {
    
    class UKN_API RenderView {
    public:
        static RenderViewPtr NullObject();
        
    public:
        virtual ~RenderView() {
            
        }
        
        uint32 width() const {
            return mWidth;
        }
        
        uint32 height() const {
            return mHeight;
        }
        
        uint32 bpp() const {
            // todo elements formats;
            return 32;
        }
        
        ElementFormat format() const {
            return mElementFormat;
        }
        
        virtual void clearColor(const Color& clr) = 0;
        virtual void clearDepth(float depth) = 0;
        virtual void clearStencil(int32 stencil) = 0;
        virtual void clearDepthStencil(float depth, int32 stencil) = 0;
        
        virtual void onAttached(FrameBuffer& fb, uint32 att) = 0;
        virtual void onDetached(FrameBuffer& fb, uint32 att) = 0;
        
        virtual void onBind(FrameBuffer& fb, uint32 att);
        virtual void onUnbind(FrameBuffer& fb, uint32 att);
        
        virtual void enableDepth(bool flag);

    protected:
        uint32 mWidth, mHeight;
        
        ElementFormat mElementFormat;
        // todo element formats
        // uint32 mColorDepth;
    };
    
} // namespace ukn


#endif
