//
//  RenderTarget.h
//  Project Unknown
//
//  Created by Griffin me on 1/23/12.
//  Copyright (c) 2012 heizi. All rights reserved.
//

#ifndef Project_Unknown_RenderTarget_h
#define Project_Unknown_RenderTarget_h

#include "UKN/Platform.h"
#include "UKN/Uncopyable.h"
#include "UKN/PreDeclare.h"
#include "UKN/GraphicSettings.h"

namespace ukn {
    
    /**
     * A render target is a composit of a texture render view and a depth stencil render view(optional)
     **/
    class RenderTarget2D: Uncopyable {
    public:
        RenderTarget2D(uint32 width,
                       uint32 height,
                       int32 num_of_levels,
                       ElementFormat color_format = EF_ARGB8);
        
        RenderTarget2D(uint32 width,
                       uint32 height,
                       int32 num_of_levels,
                       ElementFormat color_format,
                       ElementFormat depth_stencil_format);
        
        ~RenderTarget2D();
                     
        const TexturePtr& getTargetTexture() const;
        const TexturePtr& getDepthStencilTexture() const;
        
        const RenderViewPtr& getTargetView() const;
        const RenderViewPtr& getDepthStencilView() const;
        
        uint32          width() const;
        uint32          height() const;
        ElementFormat   format() const;
            
    private:
        TexturePtr mTargetTexture;
        TexturePtr mDepthStencilTexture;
        
        RenderViewPtr mTarget;
        RenderViewPtr mDepthStencil;
    };
    
} // namespace ukn


#endif
