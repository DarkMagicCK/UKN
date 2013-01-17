#include "D3D10RenderBuffer.h"

#include "D3D10GraphicDevice.h"
#include "D3D10Shader.h"

#include "ukn/RenderBuffer.h"

namespace ukn {

	D3D10RenderBuffer::D3D10RenderBuffer(D3D10GraphicDevice* device):
	mEffect(0),
	mDevice(device) {
		mEffect = new D3D10Effect(mDevice);
		if(!mEffect->initialize(mist::ResourceLoader::Instance().loadResource(L"color.fx"))) {
			delete mEffect;
			mEffect = 0;
		}
	}

	D3D10RenderBuffer::~D3D10RenderBuffer() {

	}

	bool D3D10RenderBuffer::setEffect(mist::ResourcePtr& resourcePtr) {
		D3D10Effect* effect  = new D3D10Effect(mDevice);
		if(!effect->initialize(resourcePtr)) {
			delete effect;
			return false;
		}
		if(getVertexStream().isValid()) {
			if(!effect->setLayout(this->getVertexFormat())) {
				delete effect;
				return false;
			}
		}

		mEffect = effect;
		return true;
	}

	SharedPtr<D3D10Effect> D3D10RenderBuffer::getEffect() const {
		return mEffect;
	}

	void D3D10RenderBuffer::onBindVertexStream(GraphicBufferPtr vertexStream, const VertexFormat& format) {
		if(mEffect) {
			mEffect->setLayout(format);
		}
	}

	void D3D10RenderBuffer::onSetVertexFormat(const VertexFormat& format) {
		if(mEffect) {
			mEffect->setLayout(format);
		}
	}

	void D3D10RenderBuffer::onBindIndexStream(GraphicBufferPtr indexStream) {

	}

	void D3D10RenderBuffer::onSetIndexCount(uint32 count) {

	}

	void D3D10RenderBuffer::onSetVertexCount(uint32 count) {

	}

	void D3D10RenderBuffer::onSetIndexStartIndex(uint32 index) {

	}

	void D3D10RenderBuffer::onUseIndexStream(bool flag) {

	}

}