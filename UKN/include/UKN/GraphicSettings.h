//
//  GraphicSettings.h
//  Project Unknown
//
//  Created by Robert Bu on 11/23/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_GraphicSettings_h
#define Project_Unknown_GraphicSettings_h

#include "mist/Platform.h"

namespace ukn {

#undef FAR
#undef NEAR
#undef far
#undef near
    
    using namespace mist;
    
    enum ElementFormat {
        EF_RGBA8,   // GL_RGBA8, D3DFMT_A8R8G8B8
        EF_RGB565,   // GL_RGB565, D3DFMT_RGB6B5
        EF_RGBA4444,  // GL_RGBA4,
        EF_RGB5A1,  // GL_RGB5A1,
        
        EF_D24S8,
        EF_D32,
        EF_D16
    };
    
    template<int T>
    struct ElementSize;
    
    template<> struct ElementSize<EF_RGBA8> {
        enum { Size = 4 };
    };
    template<> struct ElementSize<EF_RGB565> {
        enum { Size = 2 };
    };
    template<> struct ElementSize<EF_RGBA4444> {
        enum { Size = 2 };
    };
    template<> struct ElementSize<EF_RGB5A1> {
        enum { Size = 2 };
    };
    template<> struct ElementSize<EF_D24S8> {
        enum { Size = 4 };
    };
    template<> struct ElementSize<EF_D32> {
        enum { Size = 4 };
    };
    template<> struct ElementSize<EF_D16> {
        enum { Size = 2 };
    };
    
    static uint8 GetElementSize(ElementFormat format) {
        switch(format) {
            case EF_RGBA8: return 4;
            case EF_RGB565: return 2;
            case EF_RGBA4444: return 2;
            case EF_RGB5A1: return 2;
            case EF_D24S8: return 4;
            case EF_D32: return 4;
            case EF_D16: return 2;
        };
		return 0;
    };
        
    struct RenderSettings {
        RenderSettings():
        full_screen(false),
        resizable(true),
        show_mouse(true),
        left(0),
        top(0),
        width(640),
        height(480),
        color_fmt(EF_RGBA8),
        depth_stencil_fmt(EF_D16),
        sample_count(1),
        sample_quality(0),
		far(1.f),
		near(0.f) {
            
        }
        
        bool full_screen;
        bool resizable;
		bool vsync;
        bool show_mouse;
        
        int32 left;
        int32 top;
        uint32 width;
        uint32 height;

		float far;
		float near;
        
        ElementFormat color_fmt;
        ElementFormat depth_stencil_fmt;
        
        uint32 sample_count;
        uint32 sample_quality;        
        uint32 fsaa_samples;

		MistString icon;
		MistString cursor;
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
        
        RS_PointSize,       // glPointSize, D3DRS_POINTSIZE
    };
    
    enum RenderStateParam {
        RSP_TextureWrapClamp = 0,       // D3DTADDRESS_CLAMP, GL_CLAMP
        RSP_TextureWrapRepeat,          // D3DTADDRESS_REPEAT, GL_REPEAT
        RSP_TextureWrapClampToBorder,  // D3DTADDRESS_BORDER, GL_CLAMP_TO_BORDER
        RSP_TextureWrapMirror,          // D3DTADDRESS_MIRROR, GL_MIRRORED_REPEAT
        
        RSP_FilterNearest,              // GL_NEAREST, (D3DTEXF_POINT, NONE)
        RSP_FilterLinear,               // GL_LINEAR, (D3DTEXF_LINEAR, NONE)
        RSP_FilterNearestMipmapNearest, // GL_NEAREST_MIPMAP_NEAREST, (D3DTXEFT_POINT, POINT)
        RSP_FilterNearestMipmapLinear,  // GL_NEAREST_MIPMAP_LINEAR, (D3DTXEFT_POINT, LINEAR)
        RSP_FilterLinearMipmapNearest,  // GL_LINEAR_MIPMAP_NEAREST, (D3DTXEFT_LINEAR, POINT)
        
        RSP_StencilOpZero,          // GL_ZERO, D3DSTENCILOP_ZERO
        RSP_StencilOpKeep,          
        RSP_StencilOpReplace,
        RSP_StencilOpIncr,
        RSP_StencilOpDecr,
        RSP_StencilOpInvert,
        RSP_StencilOpIncrWrap,
        RSP_StencilOpDecrWrap,
        
        RSP_BlendOpAdd,             // GL_FUNC_ADD, D3DBLENDOP_ADD
        RSP_BlendOpSubstract,
        
        RSP_BlendFuncZero,              // GL_ZERO, D3DBLEND_ZERO
        RSP_BlendFuncOne,
        RSP_BlendFuncSrcColor,
        RSP_BlendFuncOneMinusSrcColor,
        RSP_BlendFuncDstColor,
        RSP_BlendFuncOneMinusDstColor,
        RSP_BlendFuncSrcAlpha,
        RSP_BlendFuncOneMinusSrcAlpha,
        RSP_BlendFuncDstAlpha,
        RSP_BlendFuncOneMinusDstAlpha,
        
        RSP_ColorOpAdd,  // GL_ADD
        RSP_ColorOpModulate, // GL_MODULATE
        
        RSP_CompNever,      // GL_NEVER, D3DCMP_NEVER
        RSP_CompAlways,
        RSP_CompLess,
        RSP_CompLEqual,
        RSP_CompEqual,
        RSP_CompGreater,
        RSP_CompGEqual,
        RSP_CompNotEqual,
        
        RSP_Enable,
        RSP_Disable, 
    };
    
} // namespace ukn  

#endif
