//
//  SamplerStateObject.h
//  Project Unknown
//
//  Created by Robert Bu on 2/5/13.
//  Copyright (c) 2013 heizi. All rights reserved.
//

#ifndef Project_Unknown_SamplerStateObject_h
#define Project_Unknown_SamplerStateObject_h

#include "UKN/PreDeclare.h"
#include "UKN/GraphicSettings.h"

namespace ukn {
 
    // opengl introduced sampler objects in 3.3
    // so the behavior of sampler objects is simulated in OGL Plugin
    //  for each FrameBuffer
    struct UKN_API SamplerStateDesc {
        RenderStateParam filter;
        RenderStateParam address_u;
        RenderStateParam address_v;
        RenderStateParam address_w;
        uint32           max_anisotropy; // GL_TEXTURE_MAX_ANISOTROPY_EXT
        
        // camparison & lod in gl?
        SamplerStateDesc():
        filter(RSP_FilterMinMagMipLinear),
        address_u(RSP_TextureWrapClamp),
        address_v(RSP_TextureWrapClamp),
        address_w(RSP_TextureWrapClamp),
        max_anisotropy(1) { }
    };
    
    class UKN_API SamplerStateObject {
    public:
        SamplerStateObject(const SamplerStateDesc& desc);
      
        // force base
        virtual ~SamplerStateObject() = 0;
        
        const SamplerStateDesc& getDesc() const;
        
    private:
        SamplerStateDesc mDesc;
    };
    
}


#endif
