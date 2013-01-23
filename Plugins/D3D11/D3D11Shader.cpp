#include "D3D11Shader.h"

#include "mist/Logger.h"
#include "mist/StringUtil.h"

#include "ukn/GraphicSettings.h"
#include "ukn/Vertex.h"

#include "D3D11GraphicDevice.h"
#include "D3D11Debug.h"
#include "D3D11Texture.h"

#include <D3Dcompiler.h>
#include <D3DX10.h>

// d3d11 Effect is a temporary solution
// plan to move to cg for cross-platform compabilities

// these headers comes from effect11 library, 
// located in the samples directory of DX SDK :(
#include "d3dx11effect.h"

#if defined(MIST_DEBUG)
#pragma comment(lib, "Effects11vs110d.lib")
#else
#pragma comment(lib, "Effects11vs110.lib")
#endif

#pragma comment(lib, "d3dcompiler.lib")

namespace ukn {

	struct D3D11ShaderUtilities {
		static ID3D11InputLayout* CreateLayout(ID3D11Device* device, const void* signature, SIZE_T size, const ukn::VertexFormat& format) {
			D3D11_INPUT_ELEMENT_DESC layoutDesc[5];

			UINT numElements = 0;

			if(format.checkFormat(VF_XYZ)) {
				layoutDesc[numElements].SemanticName = "POSITION";
				layoutDesc[numElements].SemanticIndex = 0;
				layoutDesc[numElements].Format = DXGI_FORMAT_R32G32B32_FLOAT;
				layoutDesc[numElements].InputSlot = 0;
				layoutDesc[numElements].AlignedByteOffset = format.offsetXYZ();
				layoutDesc[numElements].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
				layoutDesc[numElements].InstanceDataStepRate = 0;
				numElements += 1;
			}
			if(format.checkFormat(VF_Color0)) {
				layoutDesc[numElements].SemanticName = "COLOR";
				layoutDesc[numElements].SemanticIndex = 0;
				layoutDesc[numElements].Format = DXGI_FORMAT_R8G8B8A8_UINT;
				layoutDesc[numElements].InputSlot = 0;
				layoutDesc[numElements].AlignedByteOffset = format.offsetColor0();
				layoutDesc[numElements].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
				layoutDesc[numElements].InstanceDataStepRate = 0;
				numElements += 1;
			}
			if(format.checkFormat(VF_UV)) {
				layoutDesc[numElements].SemanticName = "TEXCOORD";
				layoutDesc[numElements].SemanticIndex = 0;
				layoutDesc[numElements].Format = DXGI_FORMAT_R32G32_FLOAT;
				layoutDesc[numElements].InputSlot = 0;
				layoutDesc[numElements].AlignedByteOffset = format.offsetUV();
				layoutDesc[numElements].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
				layoutDesc[numElements].InstanceDataStepRate = 0;
				numElements += 1;
			}
			if(format.checkFormat(VF_Color1)) {
				layoutDesc[numElements].SemanticName = "COLOR1";
				layoutDesc[numElements].SemanticIndex = 0;
				layoutDesc[numElements].Format = DXGI_FORMAT_R8G8B8A8_UINT;
				layoutDesc[numElements].InputSlot = 0;
				layoutDesc[numElements].AlignedByteOffset = format.offsetColor1();
				layoutDesc[numElements].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
				layoutDesc[numElements].InstanceDataStepRate = 0;
				numElements += 1;
			}
			if(format.checkFormat(VF_Normal)) {
				layoutDesc[numElements].SemanticName = "NORMAL";
				layoutDesc[numElements].SemanticIndex = 0;
				layoutDesc[numElements].Format = DXGI_FORMAT_R32G32B32_FLOAT;
				layoutDesc[numElements].InputSlot = 0;
				layoutDesc[numElements].AlignedByteOffset = format.offsetNormal();
				layoutDesc[numElements].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
				layoutDesc[numElements].InstanceDataStepRate = 0;
				numElements += 1;
			}

			ID3D11InputLayout* layout = 0;

			HRESULT result = device->CreateInputLayout(layoutDesc,
				numElements,
				signature,
				size,
				&layout);
			if(!D3D11Debug::CHECK_RESULT(result)) {
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

	D3D11Effect::D3D11Effect(D3D11GraphicDevice* device):
		mEffect(0),
		mTechnique(0),
		mLayout(0),
		mDevice(device) {

	}

	D3D11Effect::~D3D11Effect() {

	}

	bool D3D11Effect::initialize(mist::ResourcePtr& content) {
		StreamPtr memStream = content->readIntoMemory();
		MemoryStream* memContent = (MemoryStream*)memStream.get();

		ID3D10Blob* error;
		HRESULT result = D3DX11CreateEffectFromMemory(
            (LPCSTR)memContent->data(),
			memContent->size(),
            0,
			mDevice->getD3DDevice(),
			&mEffect);
		if(FAILED(result)) {
			D3D11ShaderUtilities::LogError(error);
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

	bool D3D11Effect::setLayout(const ukn::VertexFormat& format) {
		if(!mEffect)
			return false;

		D3DX11_PASS_DESC desc;
		mTechnique->GetPassByIndex(0)->GetDesc(&desc);

		mLayout = D3D11ShaderUtilities::CreateLayout(mDevice->getD3DDevice(), desc.pIAInputSignature, desc.IAInputSignatureSize, format);
		if(!mLayout)
			return false;

		return true;
	}

	bool D3D11Effect::setMatrixVariable(const char* name, const Matrix4& mat) {
		if(!mEffect) return false;
		ID3DX11EffectMatrixVariable* var = mEffect->GetVariableByName(name)->AsMatrix();

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

	bool D3D11Effect::setRawVariable(const char* name, void* data, uint32 length) {
		if(!mEffect) return false;
		ID3DX11EffectVariable* var = mEffect->GetVariableByName(name);

		if(var) {
			var->SetRawValue(data, 0, length);
			return true;
		}
		return false;
	}

	bool D3D11Effect::setFloatVectorVariable(const char* name, float* vec) {
		if(!mEffect) return false;
		ID3DX11EffectVectorVariable* var = mEffect->GetVariableByName(name)->AsVector();

		if(var) {
			var->SetFloatVector(vec);
			return true;
		}
		return false;
	}

	bool D3D11Effect::setIntVectorVariable(const char* name, int* vec) {
		if(!mEffect) return false;
		ID3DX11EffectVectorVariable* var = mEffect->GetVariableByName(name)->AsVector();

		if(var) {
			var->SetIntVector(vec);
			return true;
		}
		return false;
	}

	bool D3D11Effect::getMatrixVariable(const char* name, Matrix4* mat) {
		if(!mEffect) return false;
		ID3DX11EffectMatrixVariable* var = mEffect->GetVariableByName(name)->AsMatrix();

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

	bool D3D11Effect::getRawVariable(const char* name, void* data, uint32 len) {
		if(!mEffect) return false;
		ID3DX11EffectMatrixVariable* var = mEffect->GetVariableByName(name)->AsMatrix();

		if(var) {
			var->GetRawValue(data, 0, len);
			return true;
		}
		return false;
	}

	bool D3D11Effect::getFloatVectorVariable(const char* name, float* vec) {
		if(!mEffect) return false;
		ID3DX11EffectVectorVariable* var = mEffect->GetVariableByName(name)->AsVector();

		if(var) {
			var->GetFloatVector(vec);
			return true;
		}
		return false;
	}

	bool D3D11Effect::getIntVectorVariable(const char* name, int* vec) {
		if(!mEffect) return false;
		ID3DX11EffectVectorVariable* var = mEffect->GetVariableByName(name)->AsVector();

		if(var) {
			var->GetIntVector(vec);
			return true;
		}
		return false;
	}

	bool D3D11Effect::setTextureVariable(const char* name, Texture* resource) {
		if(!mEffect) return false;
		ID3DX11EffectShaderResourceVariable* var = mEffect->GetVariableByName(name)->AsShaderResource();

		if(var) {
			var->SetResource((ID3D11ShaderResourceView*)resource->getTextureId());
			return true;
		}
		return false;
	}

	void D3D11Effect::bind() {
		if(mLayout) {
			mDevice->getD3DDeviceContext()->IASetInputLayout(mLayout);
		}
		else {
			log_error("D3D11Effect::bind - no layout available, forgot to set vertex format?");
		}
	}

	void D3D11Effect::unbind() {
		if(mLayout) {
			mDevice->getD3DDeviceContext()->IASetInputLayout(0);
		}
	}


	inline ID3D10Blob* CompileShader(const ResourcePtr& content, const ShaderDesc& desc) {
		StreamPtr memStream = content->readIntoMemory();
		MemoryStream* memContent = (MemoryStream*)memStream.get();

		ID3D10Blob* error;
		ID3D10Blob* compiled;
		DWORD compileFlags = 0;
#if defined(MIST_DEBUG)
		compileFlags |= D3DCOMPILE_DEBUG;
#endif
		if(!D3D11Debug::CHECK_RESULT(D3DX10CompileFromMemory((LPCSTR)memContent->data(),
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
				D3D11ShaderUtilities::LogError(error);
				return 0;
		}
		return compiled;
	}

	uint32 D3D11Effect::getPasses() const {
		if(!mEffect || !mTechnique) 
			return 0;
		D3DX11_TECHNIQUE_DESC desc;
		mTechnique->GetDesc(&desc);
		return desc.Passes;
	}

	void D3D11Effect::applyPass(uint32 pass) {
		mTechnique->GetPassByIndex(pass)->Apply(0, mDevice->getD3DDeviceContext());
	}

    D3D11Shader::D3D11Shader(D3D11GraphicDevice* device):
	mDevice(device) {

	}
	
	D3D11Shader::~D3D11Shader() {
	}
    /*
    bool D3D11Shader::setMatrixVariable(const char* name, const D3DXMATRIX& worldMat) {
        if(mConstantTable) {
            mConstantTable->SetMatrix(mDevice->getD3DDevice(),
                                      
        }
    }

	bool D3D11Shader::setRawVariable(const char* name, void* data, uint32 length) {

    }

	bool D3D11Shader::setFloatVectorVariable(const char* name, float* vec) {

    }

	bool D3D11Shader::setIntVectorVariable(const char* name, int* vec) {

    }

	bool D3D11Shader::setShaderResourceVariable(const char* name, ID3D11ShaderResourceView* resource) {

    }

	bool D3D11Shader::getMatrixVariable(const char* name, D3DXMATRIX* mat) {

    }

    bool D3D11Shader::getRawVariable(const char* name, void* data, uint32 len) {

    }

    bool D3D11Shader::getFloatVectorVariable(const char* name, float* vec) {

    }

	bool D3D11Shader::getIntVectorVariable(const char* name, int* vec) {

    }
    */
	D3D11VertexShader::D3D11VertexShader(D3D11GraphicDevice* device):
	D3D11Shader(device),
	mShader(0) {

	}

	D3D11VertexShader::~D3D11VertexShader() {
		if(mShader)
			mShader->Release();
	}

	bool D3D11VertexShader::initialize(const ResourcePtr& content, const ShaderDesc& desc) {
		ID3D10Blob* compiled = CompileShader(content, desc);
		if(compiled) {
			HRESULT result = mDevice->getD3DDevice()->CreateVertexShader(compiled->GetBufferPointer(),
																		 compiled->GetBufferSize(),
                                                                         0,
																		 &mShader);
			if(!D3D11Debug::CHECK_RESULT(result))
				return false;

			mLayout = D3D11ShaderUtilities::CreateLayout(mDevice->getD3DDevice(),
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

	void D3D11VertexShader::bind() {
		if(mLayout) {
			mDevice->getD3DDeviceContext()->IASetInputLayout(mLayout);
		}

		if(mShader) {
			mDevice->getD3DDeviceContext()->VSSetShader(mShader, 0, 0);
		}
	}

	void D3D11VertexShader::unbind() {
		mDevice->getD3DDeviceContext()->VSSetShader(0, 0, 0);
	}

	D3D11FragmentShader::D3D11FragmentShader(D3D11GraphicDevice* device):
	D3D11Shader(device),
	mShader(0) {

	}

	D3D11FragmentShader::~D3D11FragmentShader() {
		if(mShader)
			mShader->Release();
	}

	bool D3D11FragmentShader::initialize(const ResourcePtr& content, const ShaderDesc& desc) {
		ID3D10Blob* compiled = CompileShader(content, desc);
		if(compiled) {
			HRESULT result = mDevice->getD3DDevice()->CreatePixelShader(compiled->GetBufferPointer(),
																		compiled->GetBufferSize(),
                                                                        0,
																		&mShader);
			compiled->Release();
			return D3D11Debug::CHECK_RESULT(result);
		}
		return false;
	}

	void D3D11FragmentShader::bind() {
		if(mShader) {
			mDevice->getD3DDeviceContext()->PSSetShader(mShader, 0, 0);
		}
	}

	void D3D11FragmentShader::unbind() {
		mDevice->getD3DDeviceContext()->PSSetShader(0, 0, 0);
	}

	D3D11GeometryShader::D3D11GeometryShader(D3D11GraphicDevice* device):
	D3D11Shader(device),
	mShader(0) {

	}

	D3D11GeometryShader::~D3D11GeometryShader() {
		if(mShader)
			mShader->Release();
	}

	bool D3D11GeometryShader::initialize(const ResourcePtr& content, const ShaderDesc& desc) {
		ID3D10Blob* compiled = CompileShader(content, desc);
		if(compiled) {
			HRESULT result = mDevice->getD3DDevice()->CreateGeometryShader(compiled->GetBufferPointer(),
																		   compiled->GetBufferSize(),
                                                                           0,
																		   &mShader);
			compiled->Release();
			return D3D11Debug::CHECK_RESULT(result);
		}
		return false;
	}

	void D3D11GeometryShader::bind() {
		if(mShader) {
			mDevice->getD3DDeviceContext()->GSSetShader(mShader, 0, 0);
		}
	}

	void D3D11GeometryShader::unbind() {
		mDevice->getD3DDeviceContext()->GSSetShader(0, 0, 0);
	}



}