#include "D3D10Shader.h"

#include "mist/Logger.h"
#include "mist/StringUtil.h"

#include "ukn/GraphicSettings.h"
#include "ukn/Vertex.h"

#include "D3D10GraphicDevice.h"

#include <D3DX10.h>

namespace ukn {

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
			if(error) {
				this->logError(error);
				return false;
			}
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

		D3D10_INPUT_ELEMENT_DESC layout[5];

		UINT numElements = 0;

		if(format.checkFormat(VF_XYZ)) {
			layout[numElements].SemanticName = "POSITION";
			layout[numElements].SemanticIndex = 0;
			layout[numElements].Format = DXGI_FORMAT_R32G32B32_FLOAT;
			layout[numElements].InputSlot = 0;
			layout[numElements].AlignedByteOffset = format.offsetXYZ();
			layout[numElements].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
			layout[numElements].InstanceDataStepRate = 0;
			numElements += 1;
		}
		if(format.checkFormat(VF_Color0)) {
			layout[numElements].SemanticName = "COLOR";
			layout[numElements].SemanticIndex = 0;
			layout[numElements].Format = DXGI_FORMAT_R8G8B8A8_UINT;
			layout[numElements].InputSlot = 0;
			layout[numElements].AlignedByteOffset = format.offsetColor0();
			layout[numElements].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
			layout[numElements].InstanceDataStepRate = 0;
			numElements += 1;
		}
		if(format.checkFormat(VF_UV)) {
			layout[numElements].SemanticName = "TEXCOORD";
			layout[numElements].SemanticIndex = 0;
			layout[numElements].Format = DXGI_FORMAT_R32G32_FLOAT;
			layout[numElements].InputSlot = 0;
			layout[numElements].AlignedByteOffset = format.offsetUV();
			layout[numElements].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
			layout[numElements].InstanceDataStepRate = 0;
			numElements += 1;
		}
		if(format.checkFormat(VF_Color1)) {
			layout[numElements].SemanticName = "COLOR1";
			layout[numElements].SemanticIndex = 0;
			layout[numElements].Format = DXGI_FORMAT_R8G8B8A8_UINT;
			layout[numElements].InputSlot = 0;
			layout[numElements].AlignedByteOffset = format.offsetColor1();
			layout[numElements].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
			layout[numElements].InstanceDataStepRate = 0;
			numElements += 1;
		}
		if(format.checkFormat(VF_Normal)) {
			layout[numElements].SemanticName = "COLOR1";
			layout[numElements].SemanticIndex = 0;
			layout[numElements].Format = DXGI_FORMAT_R32G32B32_FLOAT;
			layout[numElements].InputSlot = 0;
			layout[numElements].AlignedByteOffset = format.offsetNormal();
			layout[numElements].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
			layout[numElements].InstanceDataStepRate = 0;
			numElements += 1;
		}
		
		D3D10_PASS_DESC passDesc;
		mTechnique->GetPassByIndex(0)->GetDesc(&passDesc);
		
		HRESULT result = mDevice->getD3DDevice()->CreateInputLayout(layout,
																	numElements,
																	passDesc.pIAInputSignature,
																	passDesc.IAInputSignatureSize,
																	&mLayout);
		if(FAILED(result)) {
			mEffect->Release();
			mEffect = 0;
			mTechnique = 0;
			return false;
		}

	}

	bool D3D10Effect::setMatrixVariable(const char* name, const D3DXMATRIX& mat) {
		if(!mEffect) return false;
		ID3D10EffectMatrixVariable* var = mEffect->GetVariableByName(name)->AsMatrix();
	
		if(var) {
			var->SetMatrix(const_cast<float*>((const float*)mat));
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

	bool D3D10Effect::getMatrixVariable(const char* name, D3DXMATRIX* mat) {
		if(!mEffect) return false;
		ID3D10EffectMatrixVariable* var = mEffect->GetVariableByName(name)->AsMatrix();
	
		if(var) {
			var->GetMatrix((float*)mat);
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

	bool D3D10Effect::setShaderResourceVariable(const char* name, ID3D10ShaderResourceView* resource) {
		if(!mEffect) return false;
		ID3D10EffectShaderResourceVariable* var = mEffect->GetVariableByName(name)->AsShaderResource();

		if(var) {
			var->SetResource(resource);
			return true;
		}
		return false;
	}

	void D3D10Effect::logError(ID3D10Blob* error) {
		std::string error_mssg((char*)error->GetBufferPointer(), (char*)error->GetBufferPointer() + error->GetBufferSize());
		log_error(error_mssg);
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

}