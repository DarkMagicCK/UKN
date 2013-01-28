#include "mist/Platform.h"
#include "ukn/Shader.h"

#include "Cg/cg.h"

#if defined(MIST_PLATFORM_32)
#pragma comment(lib, "lib/cg.lib")
#pragma comment(lib, "lib/cgGL.lib")
#else
#pragma comment(lib, "libx.64/cg.lib")
#pragma comment(lib, "libx.64/cgGL.lib")
#endif

namespace ukn {

    class GLGraphicDevice;

    class CgGLEffect: public Effect {
    public:
        CgGLEffect(GLGraphicDevice* device);
        virtual ~CgGLEffect();

        void setVertexFormat(const VertexFormat& format) override;

        void bind(uint32 pass) override;
        void unbind() override;
        uint32 getPasses() const override;

        ShaderPtr createShader(const ResourcePtr& resource, const ShaderDesc& desc) override;

        CGcontext getContext() const;

    private:
        CGcontext mContext;
        GLGraphicDevice* mDevice;
    };

    class CgGLShader: public Shader {
    public:
        CgGLShader(CGcontext context);
        virtual ~CgGLShader();

        bool initialize(const ResourcePtr& resource, const ShaderDesc& desc);

        bool setMatrixVariable(const char* name, const Matrix4& mat) override;
	    bool setFloatVectorVariable(const char* name, const float4& vec) override;
	    bool setIntVectorVariable(const char* name, const int4& vec) override;
	    bool setTextureVariable(const char* name, const TexturePtr& tex) override;
        
        bool setFloatVariable(const char* name, uint32 len, const float* vals) override;
        bool setIntVariable(const char* name, uint32 len, const int* vals) override;
        
	    bool getMatrixVariable(const char* name, Matrix4& mat) override;
	    bool getFloatVectorVariable(const char* name, float4& vec) override;
	    bool getIntVectorVariable(const char* name, int4& vec) override;

        void bind() override;
        void unbind() override;
        
        CGprogram getProgram() const;
        CGcontext getContext() const;
        const ShaderDesc& getDesc() const;

    private:
        CGprogram mProgram;
        CGprofile mProfile;
        CGcontext mContext;
        ShaderDesc mDesc;
    };

}