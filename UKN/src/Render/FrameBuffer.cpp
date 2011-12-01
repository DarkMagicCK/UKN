//
//  FrameBuffer.cpp
//  Project Unknown
//
//  Created by Robert Bu on 11/23/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#include "UKN/FrameBuffer.h"
#include "UKN/RenderView.h"

namespace ukn {
    
    class NullFrameBuffer: public FrameBuffer {
    public:
        const ukn_wstring& description() const {
            static ukn_wstring const desc(L"Null Frame Buffer");
            return desc;
        }
        
        bool requiresFlipping() const {
            return false;
        }
        
        void clear(uint32 flags, const class Color& clr, float depth, int32 stencil) {
            
        }
    };
    
    FrameBuffer::FrameBuffer():
    mLeft(0),
    mTop(0),
    mWidth(0),
    mHeight(0),
    mColorDepth(0),
    mIsDepthBuffered(false),
    mDepthBits(0),
    mStencilBits(0),
    mActive(false) {
        
    }
    
    FrameBuffer::~FrameBuffer() {
        
    }
    
    FrameBufferPtr FrameBuffer::NullObject() {
        static FrameBufferPtr static_ptr = MakeSharedPtr<NullFrameBuffer>();
        return static_ptr;
    }
    
    uint32 FrameBuffer::left() const {
        return mLeft;
    }
 
    uint32 FrameBuffer::top() const {
        return mTop;
    }
    
    uint32 FrameBuffer::width() const {
        return mWidth;
    }
    
    uint32 FrameBuffer::height() const {
        return mHeight;
    }
    
    uint32 FrameBuffer::colorDepth() const {
        return mColorDepth;
    }
    
    uint32 FrameBuffer::depthBits() const {
        return mDepthBits;
    }
    
    uint32 FrameBuffer::stencilBits() const {
        return mStencilBits;
    }
    
    Viewport& FrameBuffer::getViewport() {
        return mViewPort;
    }
    
    const Viewport& FrameBuffer::getViewport() const {
        return mViewPort;
    }
    
    bool FrameBuffer::isActive() const {
        return mActive;
    }
    
    void FrameBuffer::setActive(bool flag) {
        mActive = flag;
    }

    void FrameBuffer::attach(uint32 attachment, const RenderViewPtr& view) {
        switch(attachment) {
            case ATT_DepthStencil:
                if(mRSView) {
                    this->detach(attachment);
                }
                mRSView = view;
                mIsDepthBuffered = true;
                
                // to do element formats
                mDepthBits = 32;
                mStencilBits = 0;
                break;
                
            default:
                ukn_assert(attachment > ATT_Color0);
                
                uint32 clrid = attachment - ATT_Color0;
                if(mClearViews.size() < clrid + 1)
                    mClearViews.resize(clrid + 1);
                
                mClearViews[clrid + 1] = view;
                size_t min_clr_index = clrid;
                for(size_t i = 0; i < clrid; ++i) {
                    if(mClearViews[i])
                        min_clr_index = i;
                }
                if(min_clr_index == clrid) {
                    mWidth = view->getWidth();
                    mHeight = view->getHeight();
                    mColorDepth = view->getBpp();
    
                    mViewPort.left = 0;
                    mViewPort.top = 0;
                    mViewPort.width = mWidth;
                    mViewPort.height = mHeight;
                }
                break;
        }
    
        view->onAttached(*this, attachment);
        
        mActive = true;
        mViewsDirty = true;
    }
    
    void FrameBuffer::detach(uint32 attachment) {
        switch(attachment) {
            case ATT_DepthStencil:
                mRSView.reset();
                
                mIsDepthBuffered = false;
                mDepthBits = 0;
                mStencilBits = 0;
                break;
                
            default: {
                uint32 clrid = attachment - ATT_Color0;
                if(mClearViews.size() < clrid + 1)
                    mClearViews.resize(clrid + 1);
                
                if(mClearViews[clrid]) {
                    mClearViews[clrid]->onDetached(*this, attachment);
                    mClearViews[clrid].reset();
                }
                break;
            }
        }
        
        mViewsDirty = true;
    }
    
    RenderViewPtr FrameBuffer::attached(uint32 att) const {
        switch (att) {
            case ATT_DepthStencil:
                return mRSView;
                break;
                
            default:
                uint32 clrid = att - ATT_Color0;
                if(mClearViews.size() < clrid + 1) {
                    return RenderViewPtr();
                } else {
                    return mClearViews[clrid];
                }
                break;
        }
    }
    
    void FrameBuffer::onBind() {
        for(uint32 i = 0; i < mClearViews.size(); ++i) {
            if(mClearViews[i]) {
                mClearViews[i]->onBind(*this, ATT_Color0+i);
            }
        }
        if(mRSView) {
            mRSView->onBind(*this, ATT_DepthStencil);
        }
        mViewsDirty = false;
    }
    
    void FrameBuffer::onUnbind() {
        for(uint32 i = 0; i < mClearViews.size(); ++i) {
            if(mClearViews[i]) {
                mClearViews[i]->onUnbind(*this, ATT_Color0+i);
            }
        }
        if(mRSView) {
            mRSView->onUnbind(*this, ATT_DepthStencil);
        }
    }
    
    void FrameBuffer::swapbuffers() {
        
    }
    
    bool FrameBuffer::isDirty() const {
        return mViewsDirty;
    }
    
     
} // namespace ukn
