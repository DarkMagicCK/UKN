//
//  RenderTarget.h
//  Project Unknown
//
//  Created by Griffin me on 1/23/12.
//  Copyright (c) 2012 heizi. All rights reserved.
//

#ifndef Project_Unknown_RenderTarget_h
#define Project_Unknown_RenderTarget_h

#include "mist/Platform.h"
#include "mist/Uncopyable.h"

#include "UKN/FrameBuffer.h"
#include "UKN/PreDeclare.h"
#include "UKN/GraphicSettings.h"

namespace ukn {
    
    class UKN_API RenderTarget: Uncopyable {
    public:
        /* create a texture render target */
        RenderTarget(uint32 width,
                     uint32 height,
                     int32 num_of_levels,
                     ElementFormat color_format = EF_RGBA8);
        /* create a depth stencil render target */
        RenderTarget(uint32 width,
                     uint32 height,
                     ElementFormat depth_stencil_format);

        ~RenderTarget();

        uint32  width() const;
        uint32  height() const;

        RenderViewPtr getRenderView() const;
        TexturePtr getTexture() const;

        ElementFormat getElementFormat() const;

    private:
        RenderViewPtr mRenderView;
        TexturePtr mTexture;
        ElementFormat mFormat;
    };

    /**
     * A composite render target is a composite of 
        multiple texture render view and 
        a optional depth stencil render view
        Also manages a framebuffer
     **/
    class UKN_API CompositeRenderTarget: Uncopyable {
    public:
        CompositeRenderTarget(); 
        ~CompositeRenderTarget();
        
        /* attach a render target */
        bool attach(Attachment attach, const RenderTargetPtr&);
        RenderTargetPtr detach(Attachment attach);
        
        uint32  width() const;
        uint32  height() const;

        const FrameBufferPtr& getFrameBuffer() const;

        void attachToRender();
        void detachFromRender();

        void attachCamera(const CameraPtr& camera);

        TexturePtr getTargetTexture(Attachment attach) const;
        RenderTargetPtr getTarget(Attachment attach) const;

    private:
        void createFrameBuffer();

        RenderTargetPtr mDepthStencilTarget;
        std::vector<RenderTargetPtr> mColorTargets;

        FrameBufferPtr mFrameBuffer;
        FrameBufferPtr mPrevFrameBuffer;
    };

    typedef SharedPtr<CompositeRenderTarget> CompositeRenderTargetPtr;
    
} // namespace ukn


#endif
