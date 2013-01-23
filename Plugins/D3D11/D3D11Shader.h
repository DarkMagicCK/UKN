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
		bool setLayout(const ukn::VertexFormat& format);

		virtual void bind() override;
		virtual void unbind() override;

		/*
			Variable Setters
		*/
		virtual bool setMatrixVariable(const char* name, const Matrix4& worldMat) override;
		// vector variables are four-components 
		virtual bool setFloatVectorVariable(const char* name, float* vec) override;
		virtual bool setIntVectorVariable(const char* name, int* vec) override;
		virtual bool setTextureVariable(const char* name, Texture* resource) override;
		/*
			Variable Getters
		*/
	    virtual bool getMatrixVariable(const char* name, Matrix4* mat) override;
		virtual bool getFloatVectorVariable(const char* name, float* vec) override;
		virtual bool getIntVectorVariable(const char* name, int* vec) override;

        
        bool setRawVariable(const char* name, void* data, uint32 length);
	    bool getRawVariable(const char* name, void* data, uint32 len);

		virtual uint32 getPasses() const;
		virtual void applyPass(uint32 pass);

	private:
		ID3DX11Effect* mEffect;
		ID3DX11EffectTechnique* mTechnique;
		ID3D11InputLayout* mLayout;

		D3D11GraphicDevice* mDevice;
	};

    /* 
    if we use seperate shaders in D3D11
    constant buffer is needed to set shader variables
    however, name mapping might be a problem,
    since there's no such thing in glsl
    to do with this part
    */
	class D3D11Shader: public Shader {
	public:
		D3D11Shader(D3D11GraphicDevice* device);
		virtual ~D3D11Shader() = 0;
	
	protected:
		D3D11GraphicDevice* mDevice;
        ID3D11Buffer* mConstantBuffer;
	};

	class D3D11VertexShader: public D3D11Shader {
	public:
		D3D11VertexShader(D3D11GraphicDevice* device);
		virtual ~D3D11VertexShader();

		virtual bool initialize(const ResourcePtr& content, const ShaderDesc& desc) override;
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
		
		virtual bool initialize(const ResourcePtr& content, const ShaderDesc& desc) override;
		virtual void bind() override;
		virtual void unbind() override;

	private:
		ID3D11PixelShader* mShader;
	};

	class D3D11GeometryShader: public D3D11Shader {
	public:
		D3D11GeometryShader(D3D11GraphicDevice* device);
		virtual ~D3D11GeometryShader();

		virtual bool initialize(const ResourcePtr& content, const ShaderDesc& desc) override;
		virtual void bind() override;
		virtual void unbind() override;

	private:
		ID3D11GeometryShader* mShader;
	};

}

#endif