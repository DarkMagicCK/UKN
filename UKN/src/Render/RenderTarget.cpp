//
//  RenderTarget.cpp
//  Project Unknown
//
//  Created by Griffin me on 1/23/12.
//  Copyright (c) 2012 heizi. All rights reserved.
//

#include "UKN/RenderTarget.h"
#include "UKN/GraphicFactory.h"
#include "UKN/GraphicDevice.h"
#include "UKN/RenderView.h"
#include "UKN/Context.h"
#include "UKN/Texture.h"
#include "UKN/RenderBuffer.h"
#include "UKN/FrameBuffer.h"

#include "mist/Logger.h"

namespace ukn {

    RenderTarget2D::RenderTarget2D() {

    }

    RenderTarget2D::~RenderTarget2D() {

    }
    
    bool RenderTarget2D::create(uint32 width,
                                uint32 height,
                                int32 num_of_levels,
                                ElementFormat color_format) {
        GraphicFactory& gf = Context::Instance().getGraphicFactory();
        
        mTargetTexture = gf.create2DTexture(width, 
                                            height, 
                                            num_of_levels, 
                                            color_format, 
                                            0,
                                            TB_RenderTarget | TB_Texture);
        if(!mTargetTexture) {
            log_error(L"ukn::RenderTarget2D: error creating target texture");
            return false;
        }
        
        mTarget = gf.createRenderView(mTargetTexture);
        if(!mTarget) {
            log_error(L"ukn::RenderTarget2D: error creating 2d render target");
            return false;
        }

        this->createFrameBuffer(gf);
    }
    
    bool RenderTarget2D::create(uint32 width,
                                uint32 height,
                                int32 num_of_levels,
                                ElementFormat color_format,
                                ElementFormat depth_stencil_format) {
        GraphicFactory& gf = Context::Instance().getGraphicFactory();
        
        mTargetTexture = gf.create2DTexture(width, 
                                            height, 
                                            num_of_levels, 
                                            color_format, 
                                            0,
                                            TB_RenderTarget | TB_Texture);
        if(!mTargetTexture) {
            log_error(L"ukn::RenderTarget2D: error creating target texture");
            return false;
        }
        
        mTarget = gf.createRenderView(mTargetTexture);
        if(!mTarget) {
            log_error(L"ukn::RenderTarget2D: error creating 2d render target");
            return false;
        }
        
        mDepthStencilTexture = gf.create2DTexture(width, 
                                                  height, 
                                                  0, 
                                                  depth_stencil_format, 
                                                  0,
                                                  TB_DepthStencil);
        if(!mDepthStencilTexture) {
            log_error(L"ukn::RenderTarget2D: error creating target texture");
            return false;
        }
        
        mDepthStencil = gf.createDepthStencilView(mDepthStencilTexture);
        if(!mDepthStencil) {
            log_error(L"ukn::RenderTarget2D: error creating 2d depth stencil target");
            return false;
        }

        return this->createFrameBuffer(gf);
    }

    bool RenderTarget2D::createFrameBuffer(GraphicFactory& gf) {
        mFrameBuffer = gf.createFrameBuffer();
        if(!mFrameBuffer)
            return false;

        if(mTarget)
            mFrameBuffer->attach(ATT_Color0, mTarget);
        if(mDepthStencil)
            mFrameBuffer->attach(ATT_DepthStencil, mDepthStencil);

        mFrameBuffer->updateScreenSize(0, 0, mTarget->width(), mTarget->height());
        return true;
    }
    
    void RenderTarget2D::attach() {
        if(mFrameBuffer) {
            GraphicDevice& gd = Context::Instance().getGraphicFactory().getGraphicDevice();
            
            mPrevFrameBuffer = gd.getCurrFrameBuffer();
            gd.bindFrameBuffer(mFrameBuffer);

        } else {
            log_error(L"RenderTarget2D::attach: framebuffer not valid");
        }
    }

    void RenderTarget2D::detach() {
        GraphicDevice& gd = Context::Instance().getGraphicFactory().getGraphicDevice();
        gd.bindFrameBuffer(mPrevFrameBuffer);
    }

    uint32 RenderTarget2D::width() const {
        if(mTarget)
            return mTarget->width();
        return 0;
    }
    
    uint32 RenderTarget2D::height() const {
        if(mTarget)
            return mTarget->height();
        return 0;
    }
    
    ElementFormat RenderTarget2D::format() const {
        if(mTargetTexture)
            return mTargetTexture->format();
        return EF_RGBA8;
    }

    const TexturePtr& RenderTarget2D::getTargetTexture() const {
        return mTargetTexture;
    }
    
    const TexturePtr& RenderTarget2D::getDepthStencilTexture() const {
        return mDepthStencilTexture;
    }
    
} // namespace ukn
