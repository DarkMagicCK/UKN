//
//  BlendStateObject.h
//  Project Unknown
//
//  Created by Robert Bu on 2/5/13.
//  Copyright (c) 2013 heizi. All rights reserved.
//

#ifndef Project_Unknown_BlendStateObject_h
#define Project_Unknown_BlendStateObject_h

#include "UKN/PreDeclare.h"
#include "UKN/GraphicSettings.h"

namespace ukn {
    
    // blend state descriptor
    // used in d3d10+ to improve performance
    // currently one render target only
    
    struct UKN_API BlendStateDesc {
        // gl_sample_alpha_to_coverage_arb
        bool alpha_to_converage;
        
        /* rgba blend factor */
        float4 blend_factor;
        
        struct BlendState {
            bool enabled;
            RenderStateParam src;
            RenderStateParam dst;
            RenderStateParam op;
            RenderStateParam src_alpha;
            RenderStateParam dst_alpha;
            RenderStateParam op_alpha;
            uint8 write_mask; // D3D*_COLOR_WRITE_ENABLE_ALL

            BlendState():
            enabled(false),
            src(RSP_BlendFuncOne),
            dst(RSP_BlendFuncZero),
            op(RSP_BlendOpAdd),
            src_alpha(RSP_BlendFuncOne),
            dst_alpha(RSP_BlendFuncZero),
            op_alpha(RSP_BlendOpAdd),
            write_mask(0x0f) { }
        };
        BlendState blend_state;

        BlendStateDesc():
        alpha_to_converage(false),
        blend_factor(float4(0, 0, 0, 0)) { }
    };
    
    class UKN_API BlendStateObject {
    public:
        BlendStateObject(const BlendStateDesc& desc);
        
        // force base
        virtual ~BlendStateObject() = 0;
      
        const BlendStateDesc& getDesc() const;
     
    public:
    //  opaque
    //      src = one, src_alpha = one, dst = 0, dst_alpha = 0
    //  additive
    //      src = source_alpha, src_alpha = source_alpha, dst = one, dst_alpha = one
    //  alphablend
    //      src = one, src_alpha = one, dst = dst_alpha = inv_source_alpha
    //  nonpremultiplied
    //      src = source, src_alpha = souce, dst = dst_alpha = inv_srouce_alpha
        /* intialize will be called automatically by app */
        static void InitializeBuildInStates();

        static BlendStatePtr Opaque();
        static BlendStatePtr Addtive();
        static BlendStatePtr AlphaBlend();
        static BlendStatePtr NonPreMultiplied();
        static BlendStatePtr BlendOff();

    private:
        BlendStateDesc mDesc;
    };
    
}


#endif
