#include "ukn/Shader.h"
#include "mist/Resource.h"

#include "D3D10Preq.h"

#include <d3dx9.h>

namespace ukn {

	class D3D10GraphicDevice;

	class D3D10Effect: public Shader {
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
		bool setMatrixVariable(const char* name, const D3DXMATRIX& worldMat);
		bool setRawVariable(const char* name, void* data, uint32 length);
		// vector variables are four-components 
		bool setFloatVectorVariable(const char* name, float* vec);
		bool setIntVectorVariable(const char* name, int* vec);
		bool setShaderResourceVariable(const char* name, ID3D10ShaderResourceView* resource);
		/*
			Variable Getters
		*/
	    bool getMatrixVariable(const char* name, D3DXMATRIX* mat);
		bool getRawVariable(const char* name, void* data, uint32 len);
		bool getFloatVectorVariable(const char* name, float* vec);
		bool getIntVectorVariable(const char* name, int* vec);

		uint32 getPasses() const;
		void applyPass(uint32 pass);

	private:
		ID3D10Effect* mEffect;
		ID3D10EffectTechnique* mTechnique;
		ID3D10InputLayout* mLayout;

		D3D10GraphicDevice* mDevice;
	};

	class D3D10Shader: public Shader {
	public:
		virtual ~D3D10Shader() = 0;
		/*
			Variable Setters
		*/
		bool setMatrixVariable(const char* name, const D3DXMATRIX& worldMat);
		bool setRawVariable(const char* name, void* data, uint32 length);
		// vector variables are four-components 
		bool setFloatVectorVariable(const char* name, float* vec);
		bool setIntVectorVariable(const char* name, int* vec);
		bool setShaderResourceVariable(const char* name, ID3D10ShaderResourceView* resource);
		/*
			Variable Getters
		*/
	    bool getMatrixVariable(const char* name, D3DXMATRIX* mat);
		bool getRawVariable(const char* name, void* data, uint32 len);
		bool getFloatVectorVariable(const char* name, float* vec);
		bool getIntVectorVariable(const char* name, int* vec);

	protected:
		ID3DXConstantTable* mConstantTable;
	};

	class D3D10VertexShader: public D3D10Shader {
	public:
		D3D10VertexShader(D3D10GraphicDevice* device);
		virtual ~D3D10VertexShader();

		virtual bool initialize(const ResourcePtr& content, const ShaderDesc& desc) override;
		virtual void bind() override;
		virtual void unbind() override;

	private:
		D3D10GraphicDevice* mDevice;
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
		D3D10GraphicDevice* mDevice;
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
		D3D10GraphicDevice* mDevice;
		ID3D10GeometryShader* mShader;
	};

}