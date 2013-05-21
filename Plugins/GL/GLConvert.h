//
//  GLConvert.h
//  Project Unknown
//
//  Created by Robert Bu on 12/2/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_GLConvert_h
#define Project_Unknown_GLConvert_h

#include "GLPreq.h"

#include "UKN/GraphicSettings.h"
#include "UKN/Vertex.h"

namespace ukn {

    extern GLenum render_mode_to_gl_mode(RenderMode mode);
    extern GLenum element_format_to_gl_format(ElementFormat format);
    extern GLenum element_format_to_texdata_format(ElementFormat format);
    extern GLenum element_format_to_gl_element_type(ElementFormat format);
    
    extern GLenum element_format_to_gl_data_type(ElementFormat format);
    
    extern bool element_format_contains_stencil(ElementFormat format);
    
#if !defined(UKN_OSX_REQUEST_OPENGL_32_CORE_PROFILE)
    extern GLenum vertex_usage_to_gl_array(VertexUsage usage);
#endif
    
    extern GLenum render_state_to_gl_state(RenderStateType type);
    
    extern GLenum render_state_param_to_gl_state_param(RenderStateParam param);
    // cg generic attribute binding fpr vp40?
    // and it does not work for ATI & OSX GPU according to nvidia's document
    // ....
    enum AttributeLocations {
        AL_Position = 0,
        AL_BlendWeight,
        AL_Normal,
        AL_Diffuse,
        AL_Specular,
        AL_Tessfactor,
        AL_PSize,
        AL_BlendIndices,
        AL_TexCoord0,
        AL_TexCoord1,
        AL_TexCoord2,
        AL_TexCoord3,
        AL_TexCoord4,
        AL_TexCoord5,
        AL_TexCoord6,
        AL_TexCoord7,
        AL_Tangent,
        AL_Binormal,
        AL_NumLocations, // 18
    };
    
    
    static struct {
        const char* semantic;
        const char* attrib;
    } semantic_name_attribute_name_map[17] = {
        { "POSITION",       "ATTR0" },
        { "BLENDWEIGHT",    "ATTR1" },
        { "NORMAL",         "ATTR2" },
        { "COLOR0",         "ATTR3" },
        { "COLOR1",         "ATTR4" },
        { "PSIZE",          "ATTR6" },
        { "BLENDINDICIES",  "ATTR7" },
        { "TEXCOORD0",      "ATTR8" },
        { "TEXCOORD1",      "ATTR9" },
        { "TEXCOORD2",      "ATTR10" },
        { "TEXCOORD3",      "ATTR11" },
        { "TEXCOORD4",      "ATTR12" },
        { "TEXCOORD5",      "ATTR13" },
        { "TEXCOORD6",      "ATTR14" },
        { "TEXCOORD7",      "ATTR15" },
        { "BINORMAL",       "ATTR16" },
        { "TANGENT",        "ATTR17" }
    };
    
    extern int32 vertex_usage_to_attribute_location(VertexUsage usage);
    extern GLenum render_state_param_to_blend_equation(RenderStateParam param);
    
    
} // namespace ukn


#endif
