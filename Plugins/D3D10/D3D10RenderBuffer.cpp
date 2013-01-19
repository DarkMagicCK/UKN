#include "D3D10RenderBuffer.h"

#include "D3D10GraphicDevice.h"
#include "D3D10Shader.h"

#include "ukn/RenderBuffer.h"

namespace ukn {

	D3D10RenderBuffer::D3D10RenderBuffer(D3D10GraphicDevice* device):
	mDevice(device) {
	}

	D3D10RenderBuffer::~D3D10RenderBuffer() {

	}

	void D3D10RenderBuffer::onBindVertexStream(GraphicBufferPtr vertexStream, const VertexFormat& format) {
		if(mEffect) {
			((D3D10Effect*)mEffect.get())->setLayout(format);
		}
	}

	void D3D10RenderBuffer::onSetVertexFormat(const VertexFormat& format) {
		if(mEffect) {
			((D3D10Effect*)mEffect.get())->setLayout(format);
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

    void D3D10RenderBuffer::onSetEffect(const EffectPtr& effect) {
        if(getVertexStream().isValid()) {
			((D3D10Effect*)mEffect.get())->setLayout(this->getVertexFormat());
		}
    }

}