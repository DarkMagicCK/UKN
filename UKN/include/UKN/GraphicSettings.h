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

    using namespace mist;
    
    enum ElementFormat {
        EF_Unknown = -1,

        EF_RGBA8,   // GL_RGBA8, D3DFMT_A8R8G8B8
        EF_RGB565,   // GL_RGB565, D3DFMT_RGB6B5
        EF_RGBA4444,  // GL_RGBA4,
        EF_RGB5A1,  // GL_RGB5A1,
    
        EF_D24S8,
        EF_D32,
        EF_D16,
        
        // 32bit each component
        EF_Float4,  
        EF_Float3,
        EF_Float2,
        EF_Float,
        EF_UInt32,

        EF_RGBA64, /* unsigned rgba 16bit */
        EF_RG32,   /* unsigned rg 16 bit */
        EF_HalfFloat2, /* 32bit, 16bit r, g */
        EF_HalfFloat4, /* 64bit, 16bit rgba */
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
    
    /* element size measured in uint8 */
    static uint8 GetElementSize(ElementFormat format) {
        switch(format) {
            case EF_RGBA8: return 4;
            case EF_RGB565: return 2;
            case EF_RGBA4444: return 2;
            case EF_RGB5A1: return 2;
            case EF_D24S8: return 4;
            case EF_D32: return 4;
            case EF_D16: return 2;
            case EF_Float: return 4;
            case EF_Float2: return 8;
            case EF_Float3: return 12;
            case EF_Float4: return 16;
            case EF_UInt32: return 4;
            case EF_RGBA64: return 8;
            case EF_RG32:   return 4;
            case EF_HalfFloat2: return 4;
            case EF_HalfFloat4: return 8;
            default:
                return 0;
        };
		return 0;
    };

    static uint8 GetElementComponentSize(ElementFormat format) {
        switch(format) {
            case EF_RGBA8: return 4;
            case EF_RGB565: return 3;
            case EF_RGBA4444: return 4;
            case EF_RGB5A1: return 4;
            case EF_D24S8: return 2;
            case EF_D32: return 1;
            case EF_D16: return 1;
            case EF_Float: return 1;
            case EF_Float2: return 2;
            case EF_Float3: return 3;
            case EF_Float4: return 4;
            case EF_UInt32: return 1;
            case EF_RGBA64: return 4;
            case EF_RG32: return 2;
            case EF_HalfFloat2: return 2;
            case EF_HalfFloat4: return 4;
            default:
                return 4;
        };
        return 0;
    }
        
    struct RenderSettings {
        RenderSettings():
        full_screen(false),
        resizable(true),
        show_mouse(true),
		vsync(false),
        left(0),
        top(0),
        width(640),
        height(480),
        color_fmt(EF_RGBA8),
        depth_stencil_fmt(EF_D16),
        sample_count(1),
        sample_quality(0),
		zfar(1.f),
		znear(0.f),
        native_window_handle(0) {
            
        }
        
        bool full_screen;
        bool resizable;
		bool vsync;
        bool show_mouse;
        
        int32 left;
        int32 top;
        uint32 width;
        uint32 height;

		float zfar;
		float znear;
        
        ElementFormat color_fmt;
        ElementFormat depth_stencil_fmt;
        
        uint32 sample_count;
        uint32 sample_quality;

		MistString icon;
		MistString cursor;
        
        /* to do
           if this value is not equal to zero
           ukn will try to create a device with this window handle and work in slave mode
           the value of the handle MUST be corresponding window handle of current platform
           that is
            Windows: HWND
            OS X: NSView/NSWindow*
            X11: XWindow
           with this, it's easier to intergret ukn into other GUI frameworks, such as qt
         */
        uintPtr native_window_handle;
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

    enum TextureBindFlag {
        TB_Texture          = 1,     // D3D*_SHADERRESOURCE_
        TB_DepthStencil     = 1U << 1, // D3D*_DEPTHSTENCIL
        TB_RenderTarget     = 1U << 2, // D3D*_RenderTarget
    };
    
    enum RenderStateType {
        RS_TextureWrap0 = 0,   // u
        RS_TextureWrap1,       // v
        RS_TextureWrap2,       // w
        
        RS_Filter,      // GL_TEXTURE_MIN_FILTER, D3DSAMP_MINFILTER
        
        RS_StencilOp,      // glStencilOp
        RS_StencilFunc,
        
        RS_DepthOp,        // glDepthOp, 
        RS_DepthMask,      // glDepthMask, D3DRS_ZWRITTABLE
        
        RS_ColorOp,        // GL_TEXTURE_ENV_MODE, D3DTTS_COLOROP
        
        RS_BlendOp,
        RS_SrcBlend,       // GL_SRC_BLEND, D3DRS_SRC_BLEND
        RS_DstBlend,
        RS_SrcAlpha,
        RS_DstAlpha,
        
        RS_Blend,
        RS_DepthTest,
        
        RS_PointSize,       // glPointSize, D3DRS_POINTSIZE

        RS_CullFace,
        RS_FillMode,
        RS_FrontFace,
    };
    
    enum RenderStateParam {
        RSP_TextureWrapClamp = 0,       // D3DTADDRESS_CLAMP, GL_CLAMP
        RSP_TextureWrapWrap,          // D3DTADDRESS_REPEAT, GL_WRAP
        RSP_TextureWrapClampToBorder,  // D3DTADDRESS_BORDER, GL_CLAMP_TO_BORDER
        RSP_TextureWrapMirror,          // D3DTADDRESS_MIRROR, GL_MIRRORED_REPEAT
        
        RSP_FilterMinMagMipPoint,       // GL_NEAREST * al
        RSP_FilterMinMagPointMipLinear,
        RSP_FilterMinPointMagLinearMipPoint,
        RSP_FilterMinPointMagMipLinear,
        RSP_FilterMinLinearMagMipPoint,
        RSP_FilterMinLinearMagPointMipLinear,
        RSP_FilterMinMagLinearMipPoint,
        RSP_FilterMinMagMipLinear,
        RSP_FilterAnisotropic,
        
        RSP_StencilOpZero,          // GL_ZERO, D3DSTENCILOP_ZERO
        RSP_StencilOpKeep,          
        RSP_StencilOpReplace,
        RSP_StencilOpIncr,
        RSP_StencilOpDecr,
        RSP_StencilOpInvert,
        RSP_StencilOpIncrWrap,
        RSP_StencilOpDecrWrap,
        
        RSP_BlendOpAdd,             // GL_FUNC_ADD, D3DBLENDOP_ADD
        RSP_BlendOpSubtract,
        RSP_BlendOpRevSubtract,
        RSP_BlendOpMax,
        RSP_BlendOpMin,
        
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
    
        RSP_CullFront,
        RSP_CullBack,
        RSP_CullNone,

        RSP_FillWireFrame,
        RSP_FillSolid,

        RSP_FrontFaceClockwise,
        RSP_FrontFaceCounterClockwise,
    };

    enum BufferAccess {
        BA_CPU_Read,        /* STAGING buffer */
        BA_CPU_Write,
        BA_GPU_Access,      /* default buffer */
    };
    
} // namespace ukn  

#endif
