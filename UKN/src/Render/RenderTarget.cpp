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

#include "mist/Logger.h"

namespace ukn {
    
    RenderTarget2D::RenderTarget2D(uint32 width,
                                   uint32 height,
                                   int32 num_of_levels,
                                   ElementFormat color_format) {
        GraphicFactory& gf = Context::Instance().getGraphicFactory();
        
        mTargetTexture = gf.create2DTexture(width, height, num_of_levels, color_format, 0);
        if(!mTargetTexture) {
            log_error("ukn::RenderTarget2D: error creating target texture");
            return;
        }
        
        mTarget = gf.create2DRenderView(mTargetTexture);
        if(!mTarget) {
            log_error("ukn::RenderTarget2D: error creating 2d render target");
            return;
        }
    }
    
    RenderTarget2D::RenderTarget2D(uint32 width,
                                   uint32 height,
                                   int32 num_of_levels,
                                   ElementFormat color_format,
                                   ElementFormat depth_stencil_format) {
        GraphicFactory& gf = Context::Instance().getGraphicFactory();
        
        mTargetTexture = gf.create2DTexture(width, height, num_of_levels, color_format, 0);
        if(!mTargetTexture) {
            log_error("ukn::RenderTarget2D: error creating target texture");
            return;
        }
        
        mTarget = gf.create2DRenderView(mTargetTexture);
        if(!mTarget) {
            log_error("ukn::RenderTarget2D: error creating 2d render target");
            return;
        }
        
        mDepthStencilTexture = gf.create2DTexture(width, height, 0, depth_stencil_format, 0);
        if(!mDepthStencilTexture) {
            log_error("ukn::RenderTarget2D: error creating target texture");
            return;
        }
        
        mDepthStencil = gf.create2DDepthStencilView(mDepthStencilTexture);
        if(!mDepthStencil) {
            log_error("ukn::RenderTarget2D: error creating 2d depth stencil target");
            return;
        }
    }
    
    RenderTarget2D::~RenderTarget2D() {
        
    }
    
    const TexturePtr& RenderTarget2D::getTargetTexture() const {
        return mTargetTexture;
    }
    
    const TexturePtr& RenderTarget2D::getDepthStencilTexture() const {
        return mDepthStencilTexture;
    }
    
    const RenderViewPtr& RenderTarget2D::getTargetView() const {
        return mTarget;
    }
    
    const RenderViewPtr& RenderTarget2D::getDepthStencilView() const {
        return mDepthStencil;
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
            return mTargetTexture->getFormat();
        return EF_RGBA8;
    }
    
} // namespace ukn
