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
    ukn::ShaderDesc(ukn::ST_VertexShader, (entry))

#define FRAGMENT_SHADER_DESC(entry) \
    ukn::ShaderDesc(ukn::ST_FragmentShader, (entry))

#define GEOMETRY_SHADER_DESC(entry) \
    ukn::ShaderDesc(ukn::ST_GeometryShader, (entry))
    
    class Shader;
    typedef SharedPtr<Shader> ShaderPtr;
	
    class UKN_API EffectPass: Uncopyable {
    public:
        EffectPass(EffectTechnique* parent);
        virtual ~EffectPass();
        
        void setFragmentShader(const ShaderPtr& shader);
        void setVertexShader(const ShaderPtr& shader);
        void setGeometryShader(const ShaderPtr& shader);

        const ShaderPtr& getFragmentShader() const;
        const ShaderPtr& getVertexShader() const;
        const ShaderPtr& getGeometryShader() const;

        bool isOK() const;

        /* overridable */
        virtual void begin();
        virtual void end();

    protected:
        EffectTechnique* mParent;

        ShaderPtr mFragmentShader;
        ShaderPtr mVertexShader;
        ShaderPtr mGeometryShader;

        vertex_elements_type mFormat;
    };

    typedef SharedPtr<EffectPass> EffectPassPtr;

    class UKN_API EffectTechnique: Uncopyable {
    public:
        EffectTechnique(Effect* parent);
        virtual ~EffectTechnique();
        
        /* create and append a pass */
        const EffectPassPtr& appendPass();
        const EffectPassPtr& appendPass(const ShaderPtr& fragmentShader, const ShaderPtr& vertexShader, const ShaderPtr& geometryShader);
       
        void appendPass(const EffectPassPtr& pass);
        const EffectPassPtr& getPass(uint32 pass) const;
        
        /* begin */
        void bind(uint32 pass);
        void unbind(uint32 pass);

        void clear();
        
        uint32 getNumPasses() const;

        virtual EffectPassPtr createPass();

    protected:
        Effect* mParent;
        std::vector<EffectPassPtr> mPasses;
    };

    typedef SharedPtr<EffectTechnique> EffectTechniquePtr;

    /* 
    currently
    effect is only a container of shaders
    not acctually "effect" files such as CgFx, D3D Effect etc
    */
    class UKN_API Effect: Uncopyable {
    public:
        Effect();
        virtual ~Effect();

        /* to do in custom shader compiler, no use now */		
        // virtual bool initialize(const ResourcePtr& content, const VertexFormat& format) = 0;
        
        virtual ShaderPtr createShader(const ResourcePtr& content, const ShaderDesc& desc) = 0;
        virtual EffectTechniquePtr createTechnique();

        const EffectTechniquePtr& appendTechnique();
        const EffectTechniquePtr& appendTechnique(const ShaderPtr& fragment, const ShaderPtr& vertex, const ShaderPtr& geometry);
        const EffectTechniquePtr& getTechnique(uint32 index) const;

        uint32 getNumTechniques() const;

    protected:
        typedef std::vector<EffectTechniquePtr> TechniqueVec;
        TechniqueVec mTechniques;
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
