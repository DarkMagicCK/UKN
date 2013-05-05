//
//  GLRenderView.cpp
//  Project Unknown
//
//  Created by Robert Bu on 12/2/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//


#include "GLRenderView.h"
#include "GLGraphicDevice.h"
#include "GLFrameBuffer.h"
#include "GLError.h"

#include "mist/Common.h"
#include "mist/Stream.h"

#include "UKN/Context.h"
#include "UKN/GraphicFactory.h"
#include "UKN/GraphicDevice.h"

namespace ukn {

    GLRenderView::GLRenderView():
    mTex(0),
    mFBO(0),
    mIndex(0) {

    }

    GLRenderView::~GLRenderView() {

    }
    
    void GLRenderView::clearColor(const Color& clr) {
        this->doClear(GL_COLOR_BUFFER_BIT, clr, 1.f, 0);
    }

    void GLRenderView::clearDepth(float depth) {
        this->doClear(GL_DEPTH_BUFFER_BIT, Color(), depth, 0);
    }

    void GLRenderView::clearStencil(int32 stencil) {
        this->doClear(GL_STENCIL_BUFFER_BIT, Color(), 0, stencil);
    }

    void GLRenderView::clearDepthStencil(float depth, int32 stencil) {
        this->doClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT, Color(), depth, stencil);
    }
    
    GLuint GLRenderView::getGLIndex() const {
        return mIndex;
    }
    
    GLuint GLRenderView::getGLFBO() const {
        return mFBO;
    }
    
    GLuint GLRenderView::getGLTex() const {
        return mTex;
    }

    void GLRenderView::doClear(uint32 flags, const class Color& clr, float depth, int32 stencil) {
        GLGraphicDevice& gd = *checked_cast<GLGraphicDevice*>(&Context::Instance().getGraphicFactory().getGraphicDevice());

        GLuint oldfbo = gd.getBindedGLFrameBuffer();
        gd.bindGLFrameBuffer(mFBO);

        if(flags & GL_DEPTH_BUFFER_BIT)
            glDepthMask(GL_TRUE);

        //if(flags & GL_STENCIL_BUFFER_BIT)

        if(flags & GL_COLOR_BUFFER_BIT)
            glClearColor(clr.r(), clr.g(), clr.b(), clr.a());

        if(flags & GL_DEPTH_BUFFER_BIT)
            glClearDepth(depth);

        if(flags & GL_STENCIL_BUFFER_BIT)
            glClearStencil(stencil);

        if(flags != 0)
            glClear(flags);

        if(flags & GL_DEPTH_BUFFER_BIT)
            glDepthMask(GL_FALSE);

        //if(flags & GL_STENCIL_BUFFER_BIT)

        gd.bindGLFrameBuffer(oldfbo);
    }

    GLScreenColorRenderView::GLScreenColorRenderView(uint32 width, uint32 height, ElementFormat ef) {
        mWidth = width;
        mHeight = height;
        mElementFormat = ef;
        
        glDepthFunc(GL_LESS);
    }

    void GLScreenColorRenderView::clearColor(const Color& clr) {
        this->doClear(GL_COLOR_BUFFER_BIT, clr, 0, 0);
    }

    void GLScreenColorRenderView::clearDepth(float depth) {
        // false
    }

    void GLScreenColorRenderView::clearStencil(int32 stencil) {
        // false
    }

    void GLScreenColorRenderView::clearDepthStencil(float depth, int32 stencil) {
        // false
    }

    void GLScreenColorRenderView::onAttached(FrameBuffer& fb, uint32 att) {
        GLGraphicDevice& gd = *checked_cast<GLGraphicDevice*>(&Context::Instance().getGraphicFactory().getGraphicDevice());
        gd.bindGLFrameBuffer(0);
    }

    void GLScreenColorRenderView::onDetached(FrameBuffer& fb, uint32 att) {
        GLGraphicDevice& gd = *checked_cast<GLGraphicDevice*>(&Context::Instance().getGraphicFactory().getGraphicDevice());
        gd.bindGLFrameBuffer(0);
    }

    GLScreenDepthStencilRenderView::GLScreenDepthStencilRenderView(uint32 width, uint32 height, ElementFormat ef) {
        mWidth = width;
        mHeight = height;
        mElementFormat = ef;
        
    }

    void GLScreenDepthStencilRenderView::clearColor(const Color& clr) {
        // false
    }

    void GLScreenDepthStencilRenderView::onAttached(FrameBuffer& fb, uint32 att) {
        mIndex = 0;
        GLGraphicDevice& gd = *checked_cast<GLGraphicDevice*>(&Context::Instance().getGraphicFactory().getGraphicDevice());
        gd.bindGLFrameBuffer(0);
    }

    void GLScreenDepthStencilRenderView::onDetached(FrameBuffer& fb, uint32 att) {
        GLGraphicDevice& gd = *checked_cast<GLGraphicDevice*>(&Context::Instance().getGraphicFactory().getGraphicDevice());
        gd.bindGLFrameBuffer(0);
    }

    GLTexture2DRenderView::GLTexture2DRenderView(const TexturePtr& texture, int32 index, int32 level):
    mTexture(texture),
    mIndex(index),
    mLevel(level) {
        GLTexture2D* glTexture = (GLTexture2D*)mTexture.get();
        mTex = (GLuint)glTexture->getTextureId();
        mWidth = glTexture->width(0);
        mHeight = glTexture->height(0);
        mElementFormat = glTexture->format();
    }

    void GLTexture2DRenderView::clearColor(const Color& clr) {
        if(mFBO != 0) {
            this->doClear(GL_COLOR_BUFFER_BIT, clr, 0, 0);
        } else {
            glBindTexture(GL_TEXTURE_2D, mTex);

            std::vector<Color> mem_clr(mWidth * mHeight, clr);
            CHECK_GL_CALL(glTexSubImage2D(GL_TEXTURE_2D, mLevel, 0, 0, mWidth, mHeight, GL_RGBA, GL_FLOAT, mem_clr.data()));
        }
    }

    void GLTexture2DRenderView::onAttached(FrameBuffer& fb, uint32 att) {
        mIndex = att - ATT_Color0;
        mFBO = checked_cast<GLFrameBuffer*>(&fb)->getGLFBO();

        GLGraphicDevice& gd = *checked_cast<GLGraphicDevice*>(&Context::Instance().getGraphicFactory().getGraphicDevice());

        gd.bindGLFrameBuffer(mFBO);
#ifndef UKN_OSX_REQUEST_OPENGL_32_CORE_PROFILE
        CHECK_GL_CALL(glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT + att - ATT_Color0, GL_TEXTURE_2D, mTex, mLevel));
#else
        CHECK_GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + att - ATT_Color0, GL_TEXTURE_2D, mTex, mLevel));
#endif
        gd.bindGLFrameBuffer(0);
    }

    void GLTexture2DRenderView::onDetached(FrameBuffer& fb, uint32 att) {
        mIndex = att - ATT_Color0;
        mFBO = checked_cast<GLFrameBuffer*>(&fb)->getGLFBO();

        GLGraphicDevice& gd = *checked_cast<GLGraphicDevice*>(&Context::Instance().getGraphicFactory().getGraphicDevice());

        gd.bindGLFrameBuffer(mFBO);
#ifndef UKN_OSX_REQUEST_OPENGL_32_CORE_PROFILE
        CHECK_GL_CALL(glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT + att - ATT_Color0, GL_TEXTURE_2D, 0, 0));
#else
        CHECK_GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + att - ATT_Color0, GL_TEXTURE_2D, 0, 0));
#endif
        gd.bindGLFrameBuffer(0);
        mFBO = 0;

    }
    
    GLTexture2DDepthStencilView::GLTexture2DDepthStencilView(uint32 width, uint32 height, ElementFormat ef) {
        
        GraphicFactory& gf = Context::Instance().getGraphicFactory();
        mTexture = gf.create2DTexture(width,
                                      height,
                                      0,
                                      ef,
                                      0,
                                      TB_DepthStencil);
        if(!mTexture)
            log_error(L"GLTexture2DDepthStencilView: error creating depth stencil texture");
        else {
            GLTexture2D* glTexture = (GLTexture2D*)mTexture.get();
            mTex = (GLuint)glTexture->getTextureId();
            mWidth = glTexture->width(0);
            mHeight = glTexture->height(0);
            mElementFormat = glTexture->format();
        }
    }
    
    GLTexture2DDepthStencilView::GLTexture2DDepthStencilView(const TexturePtr& texture):
    mTexture(texture) {
        GLTexture2D* glTexture = (GLTexture2D*)mTexture.get();
        mTex = (GLuint)glTexture->getTextureId();
        mWidth = glTexture->width(0);
        mHeight = glTexture->height(0);
        mElementFormat = glTexture->format();
    }
    
    void GLTexture2DDepthStencilView::clearDepth(float depth) {
        glClearDepth(depth);
    }
    
    void GLTexture2DDepthStencilView::clearStencil(int32 stencil) {
        glClearStencil(stencil);
    }
    
    void GLTexture2DDepthStencilView::clearDepthStencil(float depth, int32 stencil) {
        glClearDepth(depth);
        glClearStencil(stencil);
    }
    
    void GLTexture2DDepthStencilView::onAttached(FrameBuffer& fb, uint32 att) {
        mFBO = checked_cast<GLFrameBuffer*>(&fb)->getGLFBO();
        
        GLGraphicDevice& gd = *checked_cast<GLGraphicDevice*>(&Context::Instance().getGraphicFactory().getGraphicDevice());
        
        gd.bindGLFrameBuffer(mFBO);
#ifndef UKN_OSX_REQUEST_OPENGL_32_CORE_PROFILE
        CHECK_GL_CALL(glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, mTex, 0));
#else
        CHECK_GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, mTex, 0));
#endif
        gd.bindGLFrameBuffer(0);
    }
    
    void GLTexture2DDepthStencilView::onDetached(FrameBuffer& fb, uint32 att) {
        mFBO = checked_cast<GLFrameBuffer*>(&fb)->getGLFBO();
        
        GLGraphicDevice& gd = *checked_cast<GLGraphicDevice*>(&Context::Instance().getGraphicFactory().getGraphicDevice());
        
        gd.bindGLFrameBuffer(mFBO);
#ifndef UKN_OSX_REQUEST_OPENGL_32_CORE_PROFILE
        CHECK_GL_CALL(glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, 0, 0));
#else
        CHECK_GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, 0, 0));
#endif
        gd.bindGLFrameBuffer(0);
        mFBO = 0;
    }
    

} // namespace ukn