#include "D3D10Shader.h"

#include "mist/Logger.h"
#include "mist/StringUtil.h"

#include "ukn/GraphicSettings.h"
#include "ukn/Vertex.h"

#include "D3D10GraphicDevice.h"
#include "D3D10Debug.h"
#include "D3D10Texture.h"

namespace ukn {

	struct D3D10ShaderUtilities {
		static ID3D10InputLayout* CreateLayout(ID3D10Device* device, const void* signature, SIZE_T size, const ukn::VertexFormat& format) {
			D3D10_INPUT_ELEMENT_DESC layoutDesc[5];

			UINT numElements = 0;

			if(format.checkFormat(VF_XYZ)) {
				layoutDesc[numElements].SemanticName = "POSITION";
				layoutDesc[numElements].SemanticIndex = 0;
				layoutDesc[numElements].Format = DXGI_FORMAT_R32G32B32_FLOAT;
				layoutDesc[numElements].InputSlot = 0;
				layoutDesc[numElements].AlignedByteOffset = format.offsetXYZ();
				layoutDesc[numElements].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
				layoutDesc[numElements].InstanceDataStepRate = 0;
				numElements += 1;
			}
			if(format.checkFormat(VF_Color0)) {
				layoutDesc[numElements].SemanticName = "COLOR";
				layoutDesc[numElements].SemanticIndex = 0;
				layoutDesc[numElements].Format = DXGI_FORMAT_R8G8B8A8_UINT;
				layoutDesc[numElements].InputSlot = 0;
				layoutDesc[numElements].AlignedByteOffset = format.offsetColor0();
				layoutDesc[numElements].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
				layoutDesc[numElements].InstanceDataStepRate = 0;
				numElements += 1;
			}
			if(format.checkFormat(VF_UV)) {
				layoutDesc[numElements].SemanticName = "TEXCOORD";
				layoutDesc[numElements].SemanticIndex = 0;
				layoutDesc[numElements].Format = DXGI_FORMAT_R32G32_FLOAT;
				layoutDesc[numElements].InputSlot = 0;
				layoutDesc[numElements].AlignedByteOffset = format.offsetUV();
				layoutDesc[numElements].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
				layoutDesc[numElements].InstanceDataStepRate = 0;
				numElements += 1;
			}
			if(format.checkFormat(VF_Color1)) {
				layoutDesc[numElements].SemanticName = "COLOR1";
				layoutDesc[numElements].SemanticIndex = 0;
				layoutDesc[numElements].Format = DXGI_FORMAT_R8G8B8A8_UINT;
				layoutDesc[numElements].InputSlot = 0;
				layoutDesc[numElements].AlignedByteOffset = format.offsetColor1();
				layoutDesc[numElements].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
				layoutDesc[numElements].InstanceDataStepRate = 0;
				numElements += 1;
			}
			if(format.checkFormat(VF_Normal)) {
				layoutDesc[numElements].SemanticName = "NORMAL";
				layoutDesc[numElements].SemanticIndex = 0;
				layoutDesc[numElements].Format = DXGI_FORMAT_R32G32B32_FLOAT;
				layoutDesc[numElements].InputSlot = 0;
				layoutDesc[numElements].AlignedByteOffset = format.offsetNormal();
				layoutDesc[numElements].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
				layoutDesc[numElements].InstanceDataStepRate = 0;
				numElements += 1;
			}

			ID3D10InputLayout* layout = 0;

			HRESULT result = device->CreateInputLayout(layoutDesc,
				numElements,
				signature,
				size,
				&layout);
			if(!D3D10Debug::CHECK_RESULT(result)) {
				return 0;
			}
			return layout;
		}
		static void LogError(ID3D10Blob* error) {
			if(error) {
				std::string error_mssg((char*)error->GetBufferPointer(), (char*)error->GetBufferPointer() + error->GetBufferSize());
				log_error(error_mssg);
			}
		}

	};

	D3D10Effect::D3D10Effect(D3D10GraphicDevice* device):
		mEffect(0),
		mTechnique(0),
		mLayout(0),
		mDevice(device) {

	}

	D3D10Effect::~D3D10Effect() {

	}

	bool D3D10Effect::initialize(mist::ResourcePtr& content) {
		StreamPtr memStream = content->readIntoMemory();
		MemoryStream* memContent = (MemoryStream*)memStream.get();

		ID3D10Blob* error;
		HRESULT result = D3DX10CreateEffectFromMemory(memContent->data(),
			memContent->size(),
			mist::string::WStringToString(content->getName()).c_str(),
			NULL,
			NULL,
			"fx_4_0",
			D3D10_SHADER_ENABLE_STRICTNESS,
			0,
			mDevice->getD3DDevice(),
			0,
			0,
			&mEffect,
			&error,
			0);
		if(FAILED(result)) {
			D3D10ShaderUtilities::LogError(error);
            return false;
		}

		mTechnique = mEffect->GetTechniqueByIndex(0);
		if(!mTechnique) {
			mEffect->Release();
			mEffect = 0;
			return false;
		}

		return true;
	}

	bool D3D10Effect::setLayout(const ukn::VertexFormat& format) {
		if(!mEffect)
			return false;

		D3D10_PASS_DESC desc;
		mTechnique->GetPassByIndex(0)->GetDesc(&desc);

		mLayout = D3D10ShaderUtilities::CreateLayout(mDevice->getD3DDevice(), desc.pIAInputSignature, desc.IAInputSignatureSize, format);
		if(!mLayout)
			return false;

		return true;
	}

	bool D3D10Effect::setMatrixVariable(const char* name, const Matrix4& mat) {
		if(!mEffect) return false;
		ID3D10EffectMatrixVariable* var = mEffect->GetVariableByName(name)->AsMatrix();

		if(var) {
            D3DXMATRIX dxmat;
            for(int col = 0; col < 4; ++col) {
                for(int row = 0; row < 4; ++row) {
                    dxmat(row, col) = mat.c[col][row];
                }
            }
			var->SetMatrix((float*)(dxmat.m));
			return true;
		}
		return false;
	}

	bool D3D10Effect::setRawVariable(const char* name, void* data, uint32 length) {
		if(!mEffect) return false;
		ID3D10EffectVariable* var = mEffect->GetVariableByName(name);

		if(var) {
			var->SetRawValue(data, 0, length);
			return true;
		}
		return false;
	}

	bool D3D10Effect::setFloatVectorVariable(const char* name, float* vec) {
		if(!mEffect) return false;
		ID3D10EffectVectorVariable* var = mEffect->GetVariableByName(name)->AsVector();

		if(var) {
			var->SetFloatVector(vec);
			return true;
		}
		return false;
	}

	bool D3D10Effect::setIntVectorVariable(const char* name, int* vec) {
		if(!mEffect) return false;
		ID3D10EffectVectorVariable* var = mEffect->GetVariableByName(name)->AsVector();

		if(var) {
			var->SetIntVector(vec);
			return true;
		}
		return false;
	}

	bool D3D10Effect::getMatrixVariable(const char* name, Matrix4* mat) {
		if(!mEffect) return false;
		ID3D10EffectMatrixVariable* var = mEffect->GetVariableByName(name)->AsMatrix();

		if(var) {
            D3DXMATRIX dxmat;

			var->GetMatrix((float*)dxmat);
            for(int col = 0; col < 4; ++col) {
                for(int row = 0; row < 4; ++row) {
                    mat->c[col][row] = dxmat(row, col);
                }
            }
			return true;
		}
		return false;
	}

	bool D3D10Effect::getRawVariable(const char* name, void* data, uint32 len) {
		if(!mEffect) return false;
		ID3D10EffectMatrixVariable* var = mEffect->GetVariableByName(name)->AsMatrix();

		if(var) {
			var->GetRawValue(data, 0, len);
			return true;
		}
		return false;
	}

	bool D3D10Effect::getFloatVectorVariable(const char* name, float* vec) {
		if(!mEffect) return false;
		ID3D10EffectVectorVariable* var = mEffect->GetVariableByName(name)->AsVector();

		if(var) {
			var->GetFloatVector(vec);
			return true;
		}
		return false;
	}

	bool D3D10Effect::getIntVectorVariable(const char* name, int* vec) {
		if(!mEffect) return false;
		ID3D10EffectVectorVariable* var = mEffect->GetVariableByName(name)->AsVector();

		if(var) {
			var->GetIntVector(vec);
			return true;
		}
		return false;
	}

	bool D3D10Effect::setTextureVariable(const char* name, Texture* resource) {
		if(!mEffect) return false;
		ID3D10EffectShaderResourceVariable* var = mEffect->GetVariableByName(name)->AsShaderResource();

		if(var) {
			var->SetResource((ID3D10ShaderResourceView*)resource->getTextureId());
			return true;
		}
		return false;
	}

	void D3D10Effect::bind() {
		if(mLayout) {
			mDevice->getD3DDevice()->IASetInputLayout(mLayout);
		}
		else {
			log_error("D3D10Effect::bind - no layout available, forgot to set vertex format?");
		}
	}

	void D3D10Effect::unbind() {
		if(mLayout) {
			mDevice->getD3DDevice()->IASetInputLayout(0);
		}
	}


	inline ID3D10Blob* CompileShader(const ResourcePtr& content, const ShaderDesc& desc) {
		StreamPtr memStream = content->readIntoMemory();
		MemoryStream* memContent = (MemoryStream*)memStream.get();

		ID3D10Blob* error;
		ID3D10Blob* compiled;
		DWORD compileFlags = 0;
#if defined(MIST_DEBUG)
		compileFlags |= D3D10_SHADER_DEBUG;
#endif
		if(!D3D10Debug::CHECK_RESULT(D3DX10CompileFromMemory((LPCSTR)memContent->data(),
			memContent->size(),
			mist::string::WStringToString(content->getName()).c_str(),
			0,
			0,
			desc.entry.c_str(),
			desc.profile.c_str(),
			compileFlags,
			0,
			0,
			&compiled,
			&error,
			0))) {
				D3D10ShaderUtilities::LogError(error);
				return 0;
		}
		return compiled;
	}

	uint32 D3D10Effect::getPasses() const {
		if(!mEffect || !mTechnique) 
			return 0;
		D3D10_TECHNIQUE_DESC desc;
		mTechnique->GetDesc(&desc);
		return desc.Passes;
	}

	void D3D10Effect::applyPass(uint32 pass) {
		mTechnique->GetPassByIndex(pass)->Apply(0);
	}

    D3D10Shader::D3D10Shader(D3D10GraphicDevice* device):
	mDevice(device) {

	}
	
	D3D10Shader::~D3D10Shader() {
	}
    /*
    bool D3D10Shader::setMatrixVariable(const char* name, const D3DXMATRIX& worldMat) {
        if(mConstantTable) {
            mConstantTable->SetMatrix(mDevice->getD3DDevice(),
                                      
        }
    }

	bool D3D10Shader::setRawVariable(const char* name, void* data, uint32 length) {

    }

	bool D3D10Shader::setFloatVectorVariable(const char* name, float* vec) {

    }

	bool D3D10Shader::setIntVectorVariable(const char* name, int* vec) {

    }

	bool D3D10Shader::setShaderResourceVariable(const char* name, ID3D10ShaderResourceView* resource) {

    }

	bool D3D10Shader::getMatrixVariable(const char* name, D3DXMATRIX* mat) {

    }

    bool D3D10Shader::getRawVariable(const char* name, void* data, uint32 len) {

    }

    bool D3D10Shader::getFloatVectorVariable(const char* name, float* vec) {

    }

	bool D3D10Shader::getIntVectorVariable(const char* name, int* vec) {

    }
    */
	D3D10VertexShader::D3D10VertexShader(D3D10GraphicDevice* device):
	D3D10Shader(device),
	mShader(0) {

	}

	D3D10VertexShader::~D3D10VertexShader() {
		if(mShader)
			mShader->Release();
	}

	bool D3D10VertexShader::initialize(const ResourcePtr& content, const ShaderDesc& desc) {
		ID3D10Blob* compiled = CompileShader(content, desc);
		if(compiled) {
			HRESULT result = mDevice->getD3DDevice()->CreateVertexShader(compiled->GetBufferPointer(),
																		 compiled->GetBufferSize(),
																		 &mShader);
			if(!D3D10Debug::CHECK_RESULT(result))
				return false;

			mLayout = D3D10ShaderUtilities::CreateLayout(mDevice->getD3DDevice(),
														 compiled->GetBufferPointer(),
														 compiled->GetBufferSize(),
														 desc.format);
			compiled->Release();
			
			if(!mLayout) {
				mShader->Release();
				mShader = 0;
				return false;
			}
			return true;
		}
		return false;
	}

	void D3D10VertexShader::bind() {
		if(mLayout) {
			mDevice->getD3DDevice()->IASetInputLayout(mLayout);
		}

		if(mShader) {
			mDevice->getD3DDevice()->VSSetShader(mShader);
		}
	}

	void D3D10VertexShader::unbind() {
		mDevice->getD3DDevice()->VSSetShader(0);
	}

	D3D10FragmentShader::D3D10FragmentShader(D3D10GraphicDevice* device):
	D3D10Shader(device),
	mShader(0) {

	}

	D3D10FragmentShader::~D3D10FragmentShader() {
		if(mShader)
			mShader->Release();
	}

	bool D3D10FragmentShader::initialize(const ResourcePtr& content, const ShaderDesc& desc) {
		ID3D10Blob* compiled = CompileShader(content, desc);
		if(compiled) {
			HRESULT result = mDevice->getD3DDevice()->CreatePixelShader(compiled->GetBufferPointer(),
																		compiled->GetBufferSize(),
																		&mShader);
			compiled->Release();
			return D3D10Debug::CHECK_RESULT(result);
		}
		return false;
	}

	void D3D10FragmentShader::bind() {
		if(mShader) {
			mDevice->getD3DDevice()->PSSetShader(mShader);
		}
	}

	void D3D10FragmentShader::unbind() {
		mDevice->getD3DDevice()->PSSetShader(0);
	}

	D3D10GeometryShader::D3D10GeometryShader(D3D10GraphicDevice* device):
	D3D10Shader(device),
	mShader(0) {

	}

	D3D10GeometryShader::~D3D10GeometryShader() {
		if(mShader)
			mShader->Release();
	}

	bool D3D10GeometryShader::initialize(const ResourcePtr& content, const ShaderDesc& desc) {
		ID3D10Blob* compiled = CompileShader(content, desc);
		if(compiled) {
			HRESULT result = mDevice->getD3DDevice()->CreateGeometryShader(compiled->GetBufferPointer(),
																		   compiled->GetBufferSize(),
																		   &mShader);
			compiled->Release();
			return D3D10Debug::CHECK_RESULT(result);
		}
		return false;
	}

	void D3D10GeometryShader::bind() {
		if(mShader) {
			mDevice->getD3DDevice()->GSSetShader(mShader);
		}
	}

	void D3D10GeometryShader::unbind() {
		mDevice->getD3DDevice()->GSSetShader(0);
	}



}