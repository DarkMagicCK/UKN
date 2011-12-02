//
//  GLConvert.h
//  Project Unknown
//
//  Created by Ruiwei Bu on 12/2/11.
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
    
} // namespace ukn


#endif
