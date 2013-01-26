//
//  FrameBuffer.h
//  Project Unknown
//
//  Created by Robert Bu on 11/22/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_FrameBuffer_h
#define Project_Unknown_FrameBuffer_h

#include "mist/Platform.h"
#include "mist/Color.h"

#include "UKN/PreDeclare.h"
#include "UKN/Viewport.h"
#include "UKN/GraphicSettings.h"

#include <vector>

namespace ukn {
    
    enum Attachment {
        ATT_DepthStencil,
        ATT_Color0,
        // more to do
    };
    
    enum ClearMask {
        CM_Color   = 1UL << 0,
        CM_Depth   = 1UL << 1,
        CM_Stencil = 1UL << 2
    };
    
    class UKN_API FrameBuffer {
    public:
        FrameBuffer();
        virtual ~FrameBuffer();
        
        static FrameBufferPtr NullObject();
                
        uint32 left() const;
        uint32 top() const;
        uint32 width() const;
        uint32 height() const;
        uint32 colorDepth() const;
        uint32 depthBits() const;
        uint32 stencilBits() const;
        
        Viewport& getViewport();
        const Viewport& getViewport() const;
        
        bool isActive() const;
        void setActive(bool flag);
        
        /**
         * origin point for openGL is lower-left
         * origin point for directx is upper-left
         **/
        virtual bool requiresFlipping() const = 0;
        
        void attach(uint32 attachment, const RenderViewPtr& view);
        void detach(uint32 attachment);
        
        RenderViewPtr attached(uint32 att) const;
        
        virtual void clear(uint32 flags, const class Color& clr, float depth, int32 stencil) = 0;
        virtual void onBind();
        virtual void onUnbind();
        
        virtual void swapBuffers();
        
        bool isDirty() const;

		void updateScreenSize(uint32 left, uint32 top, uint32 w, uint32 h);
    
    protected:
        uint32 mLeft;
        uint32 mTop;
        uint32 mWidth;
        uint32 mHeight;
        uint32 mColorDepth;
        
        bool mIsDepthBuffered;
        uint32 mDepthBits;
        uint32 mStencilBits;
        
        bool mActive;
        
        Viewport mViewPort;
        
        std::vector<RenderViewPtr> mClearViews;
        RenderViewPtr mRSView;
        bool mViewsDirty;
    };
    
} // namespace ukn

#endif
