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

		ShaderDesc(ShaderType type, const std::string& entry):
		type(type),
		entry(entry) { }

        ShaderDesc() { }
    };

#define VERTEX_SHADER_DESC(entry) \
    ShaderDesc(ST_VertexShader, (entry))

#define FRAGMENT_SHADER_DESC(entry) \
    ShaderDesc(ST_FragmentShader, (entry))

#define GEOMETRY_SHADER_DESC(entry) \
    ShaderDesc(ST_GeometryShader, (entry))
    
    class Shader;
    typedef SharedPtr<Shader> ShaderPtr;
	
    class UKN_API EffectPass: Uncopyable {
    public:
        EffectPass(Effect* parent);
        virtual ~EffectPass();
        
        void setFragmentShader(const ShaderPtr& shader);
        void setVertexShader(const ShaderPtr& shader);
        void setGeometryShader(const ShaderPtr& shader);

        ShaderPtr getFragmentShader() const;
        ShaderPtr getVertexShader() const;
        ShaderPtr getGeometryShader() const;

        const vertex_elements_type& getVertexFormat() const;
       
        virtual void setVertexFormat(const vertex_elements_type& format);

        /* overridable */
        virtual void begin();
        virtual void end();

    protected:
        Effect* mParent;

        ShaderPtr mFragmentShader;
        ShaderPtr mVertexShader;
        ShaderPtr mGeometryShader;

        vertex_elements_type mFormat;
    };

    typedef SharedPtr<EffectPass> EffectPassPtr;

    /* 
    currently
    effect is only a container of shaders
    not acctually "effect" files such as CgFx, D3D Effect etc
    */
    class UKN_API Effect: Uncopyable {
    public:
        Effect();
        virtual ~Effect();

        uint32 getNumPasses() const;

        /* to do in custom shader compiler, no use now */		
        // virtual bool initialize(const ResourcePtr& content, const VertexFormat& format) = 0;
        void setVertexFormat(uint32 pass, const vertex_elements_type& format);
        
        /* begin */
        virtual void bind(uint32 pass) = 0;
        virtual void unbind(uint32 pass) = 0;

        virtual ShaderPtr createShader(const ResourcePtr& content, const ShaderDesc& desc) = 0;
        virtual EffectPassPtr createPass();

        /* create and append a pass */
        EffectPassPtr appendPass();

        void appendPass(const EffectPassPtr& pass);
        EffectPassPtr getPass(uint32 pass) const;

    protected:
        std::vector<EffectPassPtr> mPasses;
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

    public:
        /* helper functions */
        bool setFloatVectorVariable(const char* name, const float3& vec);
        bool setFloatVectorVariable(const char* name, const float2& vec);
        bool setFloatVariable(const char* name, float v);

        bool setIntVectorVariable(const char* name, const int3& vec);
        bool setIntVectorVariable(const char* name, const int2& vec);
        bool setIntVariable(const char* name, int32 v);

    };
    
} // namespace ukn



#endif
