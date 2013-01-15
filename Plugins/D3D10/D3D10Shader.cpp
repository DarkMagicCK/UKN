#include "D3D10Shader.h"
#include "mist/Logger.h"
#include "mist/StringUtil.h"

#include "D3D10GraphicDevice.h"

#include <D3DX10.h>

namespace ukn {

	D3D10Effect::D3D10Effect(D3D10GraphicDevice* device):
	mDevice(device) {

	}

	D3D10Effect::~D3D10Effect() {

	}

	bool D3D10Effect::initialize(const ukn::VertexFormat& format, const mist::Resource& content) {
		SharedPtr<MemoryStream> memContent = content.readIntoMemory();

		ID3D10Blob* error;
		HRESULT result = D3DX10CreateEffectFromMemory(memContent->data(),
													  memContent->size(),
													  mist::string::WStringToString(content.getName()).c_str(),
													  NULL,
													  NULL,
													  "fx_4_0",
													  0,
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
			}
		}

	}

	void D3D10Effect::logError(ID3D10Blob* error) {
		std::string error_mssg((char*)error->GetBufferPointer(), (char*)error->GetBufferPointer() + error->GetBufferSize());
		log_error(error_mssg);
	}

	void D3D10Effect::bind() {


	}

	void D3D10Effect::unbind() {

	}


}