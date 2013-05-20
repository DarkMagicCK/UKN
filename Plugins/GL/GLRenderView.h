//
//  GLRenderView.h
//  Project Unknown
//
//  Created by Robert Bu on 12/2/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_GLRenderView_h
#define Project_Unknown_GLRenderView_h

#include "mist/Uncopyable.h"

#include "UKN/RenderView.h"
#include "UKN/GraphicSettings.h"

#include "GLPreq.h"
#include "GLTexture.h"

namespace ukn {
    
    class GLRenderView: public RenderView {
    public:
        GLRenderView();
        virtual ~GLRenderView();
        
        void clearColor(const Color& clr);
        void clearDepth(float depth);
        void clearStencil(int32 stencil);
        void clearDepthStencil(float depth, int32 stencil);
        
        GLuint getGLIndex() const;
        GLuint getGLFBO() const;
        GLuint getGLTex() const;
        
    protected:
        void doClear(uint32 flags, const class Color& clr, float depth, int32 stencil);
        
    protected:
        GLuint mTex;
        GLuint mFBO;
        GLuint mIndex;
    };
    
    typedef SharedPtr<GLRenderView> GLRenderViewPtr;
    
    class GLScreenColorRenderView: Uncopyable, public GLRenderView {
    public:
        GLScreenColorRenderView(uint32 width, uint32 height, ElementFormat ef);
        
        void clearColor(const Color& clr);
        void clearDepth(float depth);
        void clearStencil(int32 stencil);
        void clearDepthStencil(float depth, int32 stencil);
        
        void onAttached(FrameBuffer& fb, uint32 att);
        void onDetached(FrameBuffer& fb, uint32 att);
    };
    
    typedef SharedPtr<GLScreenColorRenderView> GLScreenColorRenderViewPtr;
    
    class GLScreenDepthStencilRenderView: Uncopyable, public GLRenderView {
    public:
        GLScreenDepthStencilRenderView(uint32 width, uint32 height, ElementFormat ef);
        
        void clearColor(const Color& clr);
        
        void onAttached(FrameBuffer& fb, uint32 att);
        void onDetached(FrameBuffer& fb, uint32 att);
    };
    
    typedef SharedPtr<GLScreenDepthStencilRenderView> GLScreenDepthStencilRenderViewPtr;
    
    class GLTexture2DRenderView: Uncopyable, public GLRenderView {
    public:
        GLTexture2DRenderView(const TexturePtr& texture, int32 index, int32 level);
        
        void clearColor(const Color& clr);
        
        void onAttached(FrameBuffer& fb, uint32 att);
        void onDetached(FrameBuffer& fb, uint32 att);
        
    private:
        TexturePtr mTexture;
        int32 mLevel;
    };
    
    typedef SharedPtr<GLTexture2DRenderView> GLTexture2DRenderViewPtr;
    
    class GLDepthStencilView: Uncopyable, public GLRenderView {
    public:
        GLDepthStencilView(uint32 width, uint32 height, ElementFormat ef, uint32 sampleCount, uint32 sampleQuality);
        GLDepthStencilView(const TexturePtr& texture, int32 level = 0);
        
        void clearDepth(float depth);
        void clearStencil(int32 stencil);
        void clearDepthStencil(float depth, int32 stencil);
        
        void onAttached(FrameBuffer& fb, uint32 att);
        void onDetached(FrameBuffer& fb, uint32 att);
        
    private:
        TexturePtr mTexture;
        int32 mLevel;
        uint32 mSampleCount;
        uint32 mSampleQuaility;
        GLuint mRBO;
    };
    
    typedef SharedPtr<GLDepthStencilView> GLDepthStencilViewPtr;
    
} // namespace ukn

#endif
