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

#include "UKN/PreDeclare.h"
#include "UKN/GraphicSettings.h"

namespace ukn {
    
    /**
     * A render target is a composit of a texture render view and 
        a depth stencil render view(optional)
        Also manages a framebuffer
     **/
    class UKN_API RenderTarget2D: Uncopyable {
    public:
        RenderTarget2D(); 
        virtual ~RenderTarget2D();
        
        /* without depthstencil */
        bool create(uint32 width,
                    uint32 height,
                    int32 num_of_levels,
                    ElementFormat color_format = EF_RGBA8);
        /* with depthstencil */
        bool create(uint32 width,
                    uint32 height,
                    int32 num_of_levels,
                    ElementFormat color_format,
                    ElementFormat depth_stencil_format);
        
        uint32          width() const;
        uint32          height() const;
        ElementFormat   format() const;
        
        const FrameBufferPtr& getFrameBuffer() const;

        void attach();
        void detach();

        const TexturePtr& getTargetTexture() const;
        const TexturePtr& getDepthStencilTexture() const;

    private:
        bool createFrameBuffer(GraphicFactory& gf);

        TexturePtr mTargetTexture;
        TexturePtr mDepthStencilTexture;
        
        RenderViewPtr mTarget;
        RenderViewPtr mDepthStencil;

        FrameBufferPtr mFrameBuffer;
        FrameBufferPtr mPrevFrameBuffer;
    };
    
} // namespace ukn


#endif
