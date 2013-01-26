#include "D3D11RenderBuffer.h"

#include "D3D11GraphicDevice.h"
#include "D3D11Shader.h"
#include "CgShader.h"

#include "ukn/RenderBuffer.h"

namespace ukn {

	D3D11RenderBuffer::D3D11RenderBuffer(D3D11GraphicDevice* device):
	mDevice(device) {
	}

	D3D11RenderBuffer::~D3D11RenderBuffer() {

	}

	void D3D11RenderBuffer::onBindVertexStream(GraphicBufferPtr vertexStream, const VertexFormat& format) {
		if(mEffect) {
			((CgDxEffect*)mEffect.get())->setLayout(format);
		}
	}

	void D3D11RenderBuffer::onSetVertexFormat(const VertexFormat& format) {
		if(mEffect) {
			((CgDxEffect*)mEffect.get())->setLayout(format);
		}
	}

	void D3D11RenderBuffer::onBindIndexStream(GraphicBufferPtr indexStream) {

	}

	void D3D11RenderBuffer::onSetIndexCount(uint32 count) {

	}

	void D3D11RenderBuffer::onSetVertexCount(uint32 count) {

	}

	void D3D11RenderBuffer::onSetIndexStartIndex(uint32 index) {

	}

	void D3D11RenderBuffer::onUseIndexStream(bool flag) {

	}

    void D3D11RenderBuffer::onSetEffect(const EffectPtr& effect) {
        if(getVertexStream().isValid()) {
            ((CgDxEffect*)mEffect.get())->setLayout(getVertexFormat());
		}
    }

}