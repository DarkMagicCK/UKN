//
//  CompositeRenderTarget.cpp
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

    RenderTarget::RenderTarget(uint32 width,
                               uint32 height,
                               int32 num_of_levels,
                               ElementFormat color_format) {
        GraphicFactory& gf = Context::Instance().getGraphicFactory();
        
        mTexture = gf.create2DTexture(width, 
                                                      height, 
                                                      num_of_levels, 
                                                      color_format, 
                                                      0,
                                                      TB_RenderTarget | TB_Texture);
        if(!mTexture) {
            MIST_THROW_EXCEPTION(L"ukn::CompositeRenderTarget: error creating target texture");
        }
        
        mRenderView = gf.createRenderView(mTexture);
        if(!mRenderView) {
            MIST_THROW_EXCEPTION(L"ukn::CompositeRenderTarget: error creating 2d render target");
        }
    }

    RenderTarget::RenderTarget(uint32 width,
                    uint32 height,
                    ElementFormat depth_stencil_format) {
        GraphicFactory& gf = Context::Instance().getGraphicFactory();
        
        mTexture = gf.create2DTexture(width, 
                                      height, 
                                      0, 
                                      depth_stencil_format, 
                                      0,
                                      TB_DepthStencil);
        if(!mTexture) {
            MIST_THROW_EXCEPTION(L"ukn::CompositeRenderTarget: error creating target texture");
        }
        
        mRenderView = gf.createDepthStencilView(mTexture);
        if(!mRenderView) {
            MIST_THROW_EXCEPTION(L"ukn::CompositeRenderTarget: error creating 2d depth stencil target");
        }
    }

    RenderTarget::~RenderTarget() {
    
    }

    uint32 RenderTarget::width() const {
        if(mTexture)
            return mTexture->width();
        return 0;
    }

    uint32 RenderTarget::height() const {
        if(mTexture)
            return mTexture->height();
        return 0;
    }

    RenderViewPtr RenderTarget::getRenderView() const {
        return mRenderView;
    }

    TexturePtr RenderTarget::getTexture() const {
        return mTexture;
    }

    ElementFormat RenderTarget::getElementFormat() const {
        return mFormat;
    }

    CompositeRenderTarget::CompositeRenderTarget() {
        this->createFrameBuffer();

        mColorTargets.resize(ATT_ColorMax);
    }

    CompositeRenderTarget::~CompositeRenderTarget() {

    }
    
    bool CompositeRenderTarget::attach(Attachment attach, RenderTargetPtr renderTarget) {
        if(!renderTarget)
            return false;

        if(mFrameBuffer)
            mFrameBuffer->attach(attach, renderTarget->getRenderView());
      
        if(attach == ATT_DepthStencil)
            mDepthStencilTarget = renderTarget;
        else {
            mColorTargets[attach - ATT_Color0] = renderTarget;
        }
        
        return true;
    }

    RenderTargetPtr CompositeRenderTarget::detach(Attachment attach) {
        if(mFrameBuffer)
            mFrameBuffer->detach(attach);

        if(attach == ATT_DepthStencil)
            return mDepthStencilTarget;
        else 
            return mColorTargets[attach - ATT_Color0];
    }
    
    void CompositeRenderTarget::attachToRender() {
        if(mFrameBuffer) {
            GraphicDevice& gd = Context::Instance().getGraphicFactory().getGraphicDevice();
            
            mPrevFrameBuffer = gd.getCurrFrameBuffer();
            gd.bindFrameBuffer(mFrameBuffer);

        } else {
            log_error(L"CompositeRenderTarget::attach: framebuffer not valid");
        }
    }

    void CompositeRenderTarget::detachFromRender() {
        GraphicDevice& gd = Context::Instance().getGraphicFactory().getGraphicDevice();
        gd.bindFrameBuffer(mPrevFrameBuffer);
    }

    uint32 CompositeRenderTarget::width() const {
        if(mFrameBuffer)
            return mFrameBuffer->getViewport().width;
        return 0;
    }
    
    uint32 CompositeRenderTarget::height() const {
        if(mFrameBuffer)
            return mFrameBuffer->getViewport().height;
        return 0;
    }
    
    TexturePtr CompositeRenderTarget::getTargetTexture(Attachment attach) const {
        if(attach == ATT_DepthStencil)
            return mDepthStencilTarget->getTexture();

        if(mColorTargets[attach - ATT_Color0]) {
            return mColorTargets[attach - ATT_Color0]->getTexture();
        }

        return TexturePtr();
    }

    RenderTargetPtr CompositeRenderTarget::getTarget(Attachment attach) const {
        if(attach == ATT_DepthStencil)
            return mDepthStencilTarget;

        return mColorTargets[attach - ATT_Color0];
    }

    void CompositeRenderTarget::createFrameBuffer() {
        GraphicFactory& gf = Context::Instance().getGraphicFactory();
        mFrameBuffer = gf.createFrameBuffer();
    }

    void CompositeRenderTarget::attachCamera(const CameraPtr& camera) {
        if(mFrameBuffer)
            mFrameBuffer->getViewport().camera = camera;
    }

} // namespace ukn
