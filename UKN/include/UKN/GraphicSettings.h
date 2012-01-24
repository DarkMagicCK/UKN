//
//  GraphicSettings.h
//  Project Unknown
//
//  Created by Robert Bu on 11/23/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_GraphicSettings_h
#define Project_Unknown_GraphicSettings_h

namespace ukn {
    
    enum ElementFormat {
        EF_ARGB8,
        
        EF_D24S8,
        EF_D32,
        EF_D16
    };
        
    struct RenderSettings {
        RenderSettings():
        full_screen(false),
        resizable(false),
        show_mouse(true),
        left(0),
        top(0),
        width(0),
        height(0),
        color_fmt(EF_ARGB8),
        depth_stencil_fmt(EF_D16),
        sample_count(1),
        sample_quality(0) {
            
        }
        
        bool full_screen;
        bool resizable;
        bool show_mouse;
        
        int32 left;
        int32 top;
        uint32 width;
        uint32 height;
        
        ElementFormat color_fmt;
        ElementFormat depth_stencil_fmt;
        
        uint32 sample_count;
        uint32 sample_quality;        
        uint32 fsaa_samples;
    };

    enum BlendMode {
        BM_Alpha, 
        BM_Addictive,
        BM_None,
    };
    
    enum RenderMode {
        // single points
        RM_Point,              // GL_POINTS, D3DPT_POINTLIST
        
        // line mode
        RM_Line = 1,           // GL_LINES, D3DPT_LINELIST
        RM_LineLoop = 2,       // GL_LINE_LOOP, D3DPT_LINESTRIP (no lineloop in d3d)
        
        // triangle mode
        RM_Triangle,           // GL_TRIANGLE, D3DPT_TRIANGLELIST
        RM_TriangleFan,        // GL_TRIANGLE_FAN, D3DPT_TRIANGLEFSN
        RM_TriangleStrip,      // GL_TRIANGLE_STRIP, D3DPT_TRIANGLESTRIP
    };
    
    enum ColorFormat {
        CF_RGBA8,      // GL_RGBA8, D3DFMT_A8R8G8B8
        CF_RGB565,     // GL_RGB565, D3DFMT_RGB6B5
        CF_RGB8,       // GL_RGB8, D3DFMT_X8R8G8B8
    };
    
    enum RenderStateType {
        RS_TextureWrap0 = 0,   // u
        RS_TextureWrap1,       // v
        
        RS_MinFilter,      // GL_TEXTURE_MIN_FILTER, D3DSAMP_MINFILTER
        RS_MagFilter,      
        
        RS_StencilOp,      // glStencilOp
        RS_StencilFunc,
        
        RS_DepthOp,        // glDepthOp, 
        RS_DepthMask,      // glDepthMask, D3DRS_ZWRITTABLE
        
        RS_ColorOp,        // GL_TEXTURE_ENV_MODE, D3DTTS_COLOROP
        
        RS_SrcBlend,       // GL_SRC_BLEND, D3DRS_SRC_BLEND
        RS_DstBlend,
        RS_SrcAlpha,
        RS_DstAlpha,
        
        RS_Blend,
        RS_DepthTest,
    };
    
    enum RenderStateParam {
        TextureWrapClamp = 0,       // D3DTADDRESS_CLAMP, GL_CLAMP
        TextureWrapRepeat,          // D3DTADDRESS_REPEAT, GL_REPEAT
        TextureWrapClampToBorder,  // D3DTADDRESS_BORDER, GL_CLAMP_TO_BORDER
        TextureWrapMirror,          // D3DTADDRESS_MIRROR, GL_MIRRORED_REPEAT
        
        FilterNearest,              // GL_NEAREST, (D3DTEXF_POINT, NONE)
        FilterLinear,               // GL_LINEAR, (D3DTEXF_LINEAR, NONE)
        FilterNearestMipmapNearest, // GL_NEAREST_MIPMAP_NEAREST, (D3DTXEFT_POINT, POINT)
        FilterNearestMipmapLinear,  // GL_NEAREST_MIPMAP_LINEAR, (D3DTXEFT_POINT, LINEAR)
        FilterLinearMipmapNearest,  // GL_LINEAR_MIPMAP_NEAREST, (D3DTXEFT_LINEAR, POINT)
        
        StencilOpZero,          // GL_ZERO, D3DSTENCILOP_ZERO
        StencilOpKeep,          
        StencilOpReplace,
        StencilOpIncr,
        StencilOpDecr,
        StencilOpInvert,
        StencilOpIncrWrap,
        StencilOpDecrWrap,
        
        BlendOpAdd,             // GL_FUNC_ADD, D3DBLENDOP_ADD
        BlendOpSubstract,
        
        BlendFuncZero,              // GL_ZERO, D3DBLEND_ZERO
        BlendFuncOne,
        BlendFuncSrcColor,
        BlendFuncOneMinusSrcColor,
        BlendFuncDstColor,
        BlendFuncOneMinusDstColor,
        BlendFuncSrcAlpha,
        BlendFuncOneMinusSrcAlpha,
        BlendFuncDstAlpha,
        BlendFuncOneMinusDstAlpha,
        
        ColorOpAdd,  // GL_ADD
        ColorOpModulate, // GL_MODULATE
        
        CompNever,      // GL_NEVER, D3DCMP_NEVER
        CompAlways,
        CompLess,
        CompLEqual,
        CompEqual,
        CompGreater,
        CompGEqual,
        CompNotEqual,
        
        Enable,
        Disable, 
    };
    
} // namespace ukn  

#endif
