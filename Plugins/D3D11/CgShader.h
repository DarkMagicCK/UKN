#include "mist/Platform.h"
#include "ukn/Shader.h"

#include "Cg/cg.h"

#if defined(MIST_PLATFORM_32)
#pragma comment(lib, "lib/cg.lib")
#pragma comment(lib, "lib/cgD3D11.lib")
#else
#pragma comment(lib, "libx.64/cg.lib")
#pragma comment(lib, "libx.64/cgD3D11.lib")
#endif

class ID3D11InputLayout;

namespace ukn {

    class D3D11GraphicDevice;

    class CgDxEffect: public Effect {
    public:
        CgDxEffect(D3D11GraphicDevice* device);
        virtual ~CgDxEffect();

        virtual void onBind(uint32 pass) override;
        virtual void onUnbind() override;

        virtual ShaderPtr createShader(const ResourcePtr& resource, const ShaderDesc& desc) override;

        void setLayout(const VertexFormat& format);

        CGcontext getContext() const;

    private:
        CGcontext mContext;
    };

    class CgDxShader: public Shader {
    public:
        CgDxShader(CGcontext context);
        virtual ~CgDxShader();

        bool initialize(const ResourcePtr& resource, const ShaderDesc& desc);

        bool setMatrixVariable(const char* name, const Matrix4& mat) override;
	    bool setFloatVectorVariable(const char* name, const float4& vec) override;
	    bool setIntVectorVariable(const char* name, const int4& vec) override;
	    bool setTextureVariable(const char* name, const TexturePtr& tex) override;

	    bool getMatrixVariable(const char* name, Matrix4& mat) override;
	    bool getFloatVectorVariable(const char* name, float4& vec) override;
	    bool getIntVectorVariable(const char* name, int4& vec) override;

        void bind() override;
        void unbind() override;
        
        void setLayout(const VertexFormat& format);

    private:
        CGprogram mProgram;
        CGcontext mContext;
        ShaderDesc mDesc;

        ID3D11InputLayout* mLayout;
    };

}