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
            case TextureWrapClamp:
                return GL_CLAMP;
            case TextureWrapRepeat:
                return GL_REPEAT;
            case TextureWrapClampToBorder:
                return GL_CLAMP_TO_BORDER;
            case TextureWrapMirror:
                return GL_MIRRORED_REPEAT;
                
            case FilterNearest:
                return GL_NEAREST;
            case FilterLinear:
                return GL_LINEAR;
            case FilterNearestMipmapNearest:
                return GL_NEAREST_MIPMAP_NEAREST;
            case FilterNearestMipmapLinear:
                return GL_NEAREST_MIPMAP_LINEAR;
            case FilterLinearMipmapNearest:
                return GL_LINEAR_MIPMAP_NEAREST;
                
            case StencilOpZero:
                return GL_ZERO;
            case StencilOpKeep:
                return GL_KEEP;
            case StencilOpReplace:
                return GL_REPLACE;
            case StencilOpIncr:
                return GL_INCR;
            case StencilOpDecr:
                return GL_DECR;
            case StencilOpInvert:
                return GL_INVERT;
            case StencilOpIncrWrap:
                return GL_INCR_WRAP;
            case StencilOpDecrWrap:
                return GL_DECR_WRAP;
                
            case BlendOpAdd:
                return GL_ADD;
            case BlendOpSubstract:
                return GL_SUBTRACT;
                
            case BlendFuncZero:
                return GL_ZERO;
            case BlendFuncOne:
                return GL_ONE;
            case BlendFuncSrcColor:
                return GL_SRC_COLOR;
            case BlendFuncOneMinusSrcColor:
                return GL_ONE_MINUS_SRC_COLOR;
            case BlendFuncDstColor:
                return GL_DST_COLOR;
            case BlendFuncOneMinusDstColor:
                return GL_ONE_MINUS_DST_COLOR;
            case BlendFuncSrcAlpha:
                return GL_SRC_ALPHA;
            case BlendFuncOneMinusSrcAlpha:
                return GL_ONE_MINUS_SRC_ALPHA;
            case BlendFuncDstAlpha:
                return GL_DST_ALPHA;
            case BlendFuncOneMinusDstAlpha:
                return GL_ONE_MINUS_DST_ALPHA;
                
            case ColorOpAdd:
                return GL_ADD;
            case ColorOpModulate:
                return GL_MODULATE;
                
            case CompNever:
                return GL_NEVER;
            case CompAlways:
                return GL_NEVER;
            case CompLess:
                return GL_LESS;
            case CompLEqual:
                return GL_LEQUAL;
            case CompEqual:
                return GL_EQUAL;
            case CompGreater:
                return GL_GREATER;
            case CompGEqual:
                return GL_GEQUAL;
            case CompNotEqual:
                return GL_NOTEQUAL;
                
            case Enable:
            case Disable: 
                return param;
        }
    }
    
    
} // namespace ukn


#endif
