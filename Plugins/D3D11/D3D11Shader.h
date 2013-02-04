#ifndef Project_Unknown_D3D11_SHADER_H
#define Project_Unknown_D3D11_SHADER_H

#include "ukn/Shader.h"
#include "mist/Resource.h"

#include "D3D11Preq.h"

    struct ID3DX11Effect;
    struct ID3DX11EffectTechnique;

namespace ukn {

	class D3D11GraphicDevice;

	class D3D11Effect: public Effect {
	public:
		D3D11Effect(D3D11GraphicDevice* device);
		virtual ~D3D11Effect();

		bool initialize(mist::ResourcePtr& content);
		void setVertexFormat(const ukn::vertex_elements_type& format) override;
        
        ShaderPtr createShader(const ResourcePtr& content, const ShaderDesc& desc) override;

        virtual void bind(uint32 pass) override;
        virtual void unbind() override;

	private:
		ID3DX11Effect* mEffect;
		ID3DX11EffectTechnique* mTechnique;
		ID3D11InputLayout* mLayout;

		D3D11GraphicDevice* mDevice;
	};

	class D3D11Shader: public Shader {
	public:
		D3D11Shader(D3D11GraphicDevice* device);
		virtual ~D3D11Shader() = 0;
	
        virtual bool setMatrixVariable(const char* name, const Matrix4& worldMat) override;
		virtual bool setFloatVectorVariable(const char* name, const float4& vec) override;
		virtual bool setIntVectorVariable(const char* name, const int4& vec) override;
		virtual bool setTextureVariable(const char* name, const TexturePtr& resource) override;

	    virtual bool getMatrixVariable(const char* name, Matrix4& mat) override;
		virtual bool getFloatVectorVariable(const char* name, float4& vec) override;
		virtual bool getIntVectorVariable(const char* name, int4& vec) override;

	protected:
		D3D11GraphicDevice* mDevice;
	};

	class D3D11VertexShader: public D3D11Shader {
	public:
		D3D11VertexShader(D3D11GraphicDevice* device);
		virtual ~D3D11VertexShader();

	    bool initialize(const ResourcePtr& content, const ShaderDesc& desc);
		virtual void bind() override;
		virtual void unbind() override;


	private:
		ID3D11VertexShader* mShader;
		ID3D11InputLayout* mLayout;
	};

	class D3D11FragmentShader: public D3D11Shader {
	public:
		D3D11FragmentShader(D3D11GraphicDevice* device);
		virtual ~D3D11FragmentShader();
		
		bool initialize(const ResourcePtr& content, const ShaderDesc& desc);
		virtual void bind() override;
		virtual void unbind() override;


	private:
		ID3D11PixelShader* mShader;
	};

	class D3D11GeometryShader: public D3D11Shader {
	public:
		D3D11GeometryShader(D3D11GraphicDevice* device);
		virtual ~D3D11GeometryShader();

	    bool initialize(const ResourcePtr& content, const ShaderDesc& desc);
		virtual void bind() override;
		virtual void unbind() override;


	private:
		ID3D11GeometryShader* mShader;
	};

}

#endif