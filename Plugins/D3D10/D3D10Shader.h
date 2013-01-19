#include "ukn/Shader.h"
#include "mist/Resource.h"

#include "D3D10Preq.h"

namespace ukn {

	class D3D10GraphicDevice;

	class D3D10Effect: public Effect {
	public:
		D3D10Effect(D3D10GraphicDevice* device);
		virtual ~D3D10Effect();

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
		ID3D10Effect* mEffect;
		ID3D10EffectTechnique* mTechnique;
		ID3D10InputLayout* mLayout;

		D3D10GraphicDevice* mDevice;
	};

    /* 
    if we use seperate shaders in d3d10
    constant buffer is needed to set shader variables
    however, name mapping might be a problem,
    since there's no such thing in glsl
    to do with this part
    */
	class D3D10Shader: public Shader {
	public:
		D3D10Shader(D3D10GraphicDevice* device);
		virtual ~D3D10Shader() = 0;
	
	protected:
		D3D10GraphicDevice* mDevice;
        ID3D10Buffer* mConstantBuffer;
	};

	class D3D10VertexShader: public D3D10Shader {
	public:
		D3D10VertexShader(D3D10GraphicDevice* device);
		virtual ~D3D10VertexShader();

		virtual bool initialize(const ResourcePtr& content, const ShaderDesc& desc) override;
		virtual void bind() override;
		virtual void unbind() override;

	private:
		ID3D10VertexShader* mShader;
		ID3D10InputLayout* mLayout;
	};

	class D3D10FragmentShader: public D3D10Shader {
	public:
		D3D10FragmentShader(D3D10GraphicDevice* device);
		virtual ~D3D10FragmentShader();
		
		virtual bool initialize(const ResourcePtr& content, const ShaderDesc& desc) override;
		virtual void bind() override;
		virtual void unbind() override;

	private:
		ID3D10PixelShader* mShader;
	};

	class D3D10GeometryShader: public D3D10Shader {
	public:
		D3D10GeometryShader(D3D10GraphicDevice* device);
		virtual ~D3D10GeometryShader();

		virtual bool initialize(const ResourcePtr& content, const ShaderDesc& desc) override;
		virtual void bind() override;
		virtual void unbind() override;

	private:
		ID3D10GeometryShader* mShader;
	};

}