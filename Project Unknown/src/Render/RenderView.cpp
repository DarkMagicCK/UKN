//
//  RenderView.cpp
//  Project Unknown
//
//  Created by Robert Bu on 11/23/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#include "UKN/RenderView.h"

namespace ukn {
    
    class NullRenderView: public RenderView {
    public:
        void clearColor(const Color& color) {
            
        }
        
        void clearDepth(float depth) {
            
        }
        
        void clearStencil(int32 stencil) {
            
        }
        
        void clearDepthStencil(float depth, int32 stencil) {
            
        }
        
        void onAttached(FrameBuffer& fb, uint32 att) {
            
        }
        
        void onDetached(FrameBuffer& fb, uint32 att) {
            
        }
        
        void onBind(FrameBuffer& fb, uint32 att) {
            
        }
        
        void onUnbind(FrameBuffer& fb, uint32 att) {
            
        }
    };
    
    RenderViewPtr RenderView::NullObject() {
        static RenderViewPtr static_ptr = MakeSharedPtr<NullRenderView>();
        return static_ptr;
    }
    
    void RenderView::onBind(FrameBuffer& fb, uint32 att) {
        
    }
    
    void RenderView::onUnbind(FrameBuffer& fb, uint32 att) {
        
    }
    
    
    
} // namespace ukn