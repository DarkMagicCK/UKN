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
		ShaderType type;
		VertexFormat format; /* used by vertex shaders */

		ShaderDesc() { }
		ShaderDesc(ShaderType type, const std::string& entry, const VertexFormat& format):
		type(type),
		entry(entry),
		format(format) { }

		ShaderDesc(ShaderType type, const std::string& entry):
		type(type),
		entry(entry) { }
    };
    
    class Shader;
    typedef SharedPtr<Shader> ShaderPtr;
	
    /* 
    currently
    effect is only a container of shaders
    not acctually "effect" files such as CgFx, D3D Effect etc
    */
    class UKN_API Effect: public Uncopyable {
    public:
        Effect();
        virtual ~Effect();

        /* to do in custom shader compiler, no use now */		
       // virtual bool initialize(const ResourcePtr& content, const VertexFormat& format) = 0;
        virtual void setVertexFormat(const VertexFormat& format) = 0;
        virtual uint32 getPasses() const = 0;

        /* begin */
        virtual void bind(uint32 pass = 0) = 0;
        virtual void unbind() = 0;

        virtual ShaderPtr createShader(const ResourcePtr& content, const ShaderDesc& desc) = 0;

        void setFragmentShader(const ShaderPtr& shader);
        void setVertexShader(const ShaderPtr& shader);
        void setGeometryShader(const ShaderPtr& shader);

        ShaderPtr getFragmentShader() const;
        ShaderPtr getVertexShader() const;
        ShaderPtr getGeometryShader() const;

    protected:
        ShaderPtr mFragmentShader;
        ShaderPtr mVertexShader;
        ShaderPtr mGeometryShader;
    };
    
    class UKN_API Shader: public Uncopyable {
    public:
		virtual ~Shader()  { }

        virtual void bind() = 0;
        virtual void unbind() = 0;

        /*
			Variable Setters
		*/
		virtual bool setMatrixVariable(const char* name, const Matrix4& worldMat) = 0;
		virtual bool setFloatVectorVariable(const char* name, const float4& vec) = 0;
		virtual bool setIntVectorVariable(const char* name, const int4& vec) = 0;
		virtual bool setTextureVariable(const char* name, const TexturePtr& resource) = 0;
		
        virtual bool setFloatVariable(const char* name, uint32 len, const float* vals) = 0;
        virtual bool setIntVariable(const char* name, uint32 len, const int* val) = 0;
        
        /*
			Variable Getters
		*/
	    virtual bool getMatrixVariable(const char* name, Matrix4& mat) = 0;
		virtual bool getFloatVectorVariable(const char* name, float4& vec) = 0;
		virtual bool getIntVectorVariable(const char* name, int4& vec) = 0;

    };
    
} // namespace ukn



#endif
