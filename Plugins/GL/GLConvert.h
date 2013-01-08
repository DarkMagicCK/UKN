//
//  GLConvert.h
//  Project Unknown
//
//  Created by Robert Bu on 12/2/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_GLConvert_h
#define Project_Unknown_GLConvert_h

#include "glfw/glfw3.h"

#include "UKN/GraphicSettings.h"

namespace ukn {

    inline GLenum render_mode_to_gl_mode(RenderMode mode) {
        switch (mode) {
            case RM_Point:              return GL_POINTS;
			case RM_Line:               return GL_LINES;
            case RM_LineLoop:           return GL_LINE_LOOP;
			case RM_Triangle:           return GL_TRIANGLES;
			case RM_TriangleFan:        return GL_TRIANGLE_FAN;
			case RM_TriangleStrip:      return GL_TRIANGLE_STRIP;
		}
		return GL_INVALID_VALUE;
    }
    
    inline GLenum element_format_to_gl_format(ElementFormat format) {
        switch(format) {
            case EF_RGBA8: return GL_RGBA;
            case EF_RGBA4444: return GL_RGBA;
            case EF_RGB565: return GL_RGB;
            case EF_RGB5A1: return GL_RGBA;
            case EF_D32: return GL_DEPTH_COMPONENT;
            case EF_D16: return GL_DEPTH_COMPONENT;
            case EF_D24S8: return GL_DEPTH_STENCIL;
        }
		return GL_UNSIGNED_BYTE;
    }
    
    inline GLenum element_format_to_gl_element_type(ElementFormat format) {
        switch(format) {
            case EF_RGBA8: return GL_UNSIGNED_BYTE;
            case EF_RGBA4444: return GL_UNSIGNED_SHORT_4_4_4_4;
            case EF_RGB565: return GL_UNSIGNED_SHORT_5_6_5;
            case EF_RGB5A1: return GL_UNSIGNED_SHORT_5_5_5_1;
            case EF_D32: return GL_UNSIGNED_INT;
            case EF_D16: return GL_UNSIGNED_SHORT;
            case EF_D24S8: return GL_UNSIGNED_INT_24_8;
        }
		return GL_UNSIGNED_BYTE;
    }
    
    inline GLenum render_state_to_gl_state(RenderStateType type) {
        switch(type) {
            case RS_TextureWrap0:
                return GL_TEXTURE_WRAP_S;
            case RS_TextureWrap1:
                return GL_TEXTURE_WRAP_T;
                
            case RS_MinFilter:
                return GL_TEXTURE_MIN_FILTER;
            case RS_MagFilter:      
                return GL_TEXTURE_MAG_FILTER;
                
            case RS_StencilOp:
                return type;
            case RS_StencilFunc:
                return GL_STENCIL_FUNC;
                
            case RS_DepthOp:
            case RS_DepthMask:
                return type;
                
            case RS_ColorOp:
                return GL_TEXTURE_ENV_MODE;
                
            case RS_SrcBlend:
                return GL_SRC_COLOR;
            case RS_DstBlend:
                return GL_DST_COLOR;
            case RS_SrcAlpha:
                return GL_SRC_ALPHA;
            case RS_DstAlpha:
                return GL_DST_COLOR;
                
            default:
                return type;
        }
    }
    
    inline GLenum render_state_param_to_gl_state_param(RenderStateParam param) {
        switch(param) {
            case RSP_TextureWrapClamp:
                return GL_CLAMP;
            case RSP_TextureWrapRepeat:
                return GL_REPEAT;
            case RSP_TextureWrapClampToBorder:
                return GL_CLAMP_TO_BORDER;
            case RSP_TextureWrapMirror:
                return GL_MIRRORED_REPEAT;
                
            case RSP_FilterNearest:
                return GL_NEAREST;
            case RSP_FilterLinear:
                return GL_LINEAR;
            case RSP_FilterNearestMipmapNearest:
                return GL_NEAREST_MIPMAP_NEAREST;
            case RSP_FilterNearestMipmapLinear:
                return GL_NEAREST_MIPMAP_LINEAR;
            case RSP_FilterLinearMipmapNearest:
                return GL_LINEAR_MIPMAP_NEAREST;
                
            case RSP_StencilOpZero:
                return GL_ZERO;
            case RSP_StencilOpKeep:
                return GL_KEEP;
            case RSP_StencilOpReplace:
                return GL_REPLACE;
            case RSP_StencilOpIncr:
                return GL_INCR;
            case RSP_StencilOpDecr:
                return GL_DECR;
            case RSP_StencilOpInvert:
                return GL_INVERT;
            case RSP_StencilOpIncrWrap:
                return GL_INCR_WRAP;
            case RSP_StencilOpDecrWrap:
                return GL_DECR_WRAP;
                
            case RSP_BlendOpAdd:
                return GL_ADD;
            case RSP_BlendOpSubstract:
                return GL_SUBTRACT;
                
            case RSP_BlendFuncZero:
                return GL_ZERO;
            case RSP_BlendFuncOne:
                return GL_ONE;
            case RSP_BlendFuncSrcColor:
                return GL_SRC_COLOR;
            case RSP_BlendFuncOneMinusSrcColor:
                return GL_ONE_MINUS_SRC_COLOR;
            case RSP_BlendFuncDstColor:
                return GL_DST_COLOR;
            case RSP_BlendFuncOneMinusDstColor:
                return GL_ONE_MINUS_DST_COLOR;
            case RSP_BlendFuncSrcAlpha:
                return GL_SRC_ALPHA;
            case RSP_BlendFuncOneMinusSrcAlpha:
                return GL_ONE_MINUS_SRC_ALPHA;
            case RSP_BlendFuncDstAlpha:
                return GL_DST_ALPHA;
            case RSP_BlendFuncOneMinusDstAlpha:
                return GL_ONE_MINUS_DST_ALPHA;
                
            case RSP_ColorOpAdd:
                return GL_ADD;
            case RSP_ColorOpModulate:
                return GL_MODULATE;
                
            case RSP_CompNever:
                return GL_NEVER;
            case RSP_CompAlways:
                return GL_NEVER;
            case RSP_CompLess:
                return GL_LESS;
            case RSP_CompLEqual:
                return GL_LEQUAL;
            case RSP_CompEqual:
                return GL_EQUAL;
            case RSP_CompGreater:
                return GL_GREATER;
            case RSP_CompGEqual:
                return GL_GEQUAL;
            case RSP_CompNotEqual:
                return GL_NOTEQUAL;
                
            case RSP_Enable:
            case RSP_Disable: 
                return param;
        }
    }
    
    
} // namespace ukn


#endif
