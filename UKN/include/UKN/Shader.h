//
//  Shader.h
//  Project Unknown
//
//  Created by Robert Bu on 11/23/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_Shader_h
#define Project_Unknown_Shader_h

#include "ukn/PreDeclare.h"
#include "ukn/Vertex.h"

namespace ukn {
    
	enum ShaderType {
        ST_FragmentShader,
        ST_VertexShader,
		ST_GeometryShader, /* todo, may not support */
    };

    struct ShaderDesc {
        std::string entry;
        std::string profile;

		ShaderType type;
		VertexFormat format; /* used by vertex shaders */

		ShaderDesc() { }
		ShaderDesc(ShaderType type, const std::string& entry, const std::string& profile, const VertexFormat& format):
		type(type),
		entry(entry),
		profile(profile),
		format(format) { }

		ShaderDesc(ShaderType type, const std::string& entry, const std::string& profile):
		type(type),
		entry(entry),
		profile(profile) { }
    };
	
    
    class Shader {
    public:
		virtual ~Shader()  { }

        virtual bool initialize(const ResourcePtr& content, const ShaderDesc& desc);
        virtual void bind() = 0;
        virtual void unbind() = 0;
    };
    
} // namespace ukn



#endif
