#include "D3D11GraphicBuffer.h"
#include "D3D11GraphicDevice.h"

#include "D3D11Convert.h"

#include "mist/Logger.h"

namespace ukn {

	
	inline D3D11_USAGE GraphicBufferUsageToD3D11Usage(ukn::GraphicBuffer::Usage usage) {
		switch (usage) {
		case ukn::GraphicBuffer::Static:
			return D3D11_USAGE_DEFAULT;
		case ukn::GraphicBuffer::Dynamic:
			return D3D11_USAGE_DYNAMIC;
		}
	}

	inline UINT GraphicBufferAccessToD3D11Access(ukn::GraphicBuffer::Access access) {
		switch (access) {
		case ukn::GraphicBuffer::ReadOnly:
			return D3D11_CPU_ACCESS_READ;

		case ukn::GraphicBuffer::WriteOnly:
			return D3D11_CPU_ACCESS_WRITE;
			
		case ukn::GraphicBuffer::ReadWrite:
			return D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
		}
	}

	inline D3D11_MAP GraphicBufferAccessToD3D11Map(ukn::GraphicBuffer::Access access) {
		switch (access) {
		case ukn::GraphicBuffer::ReadOnly:
			return D3D11_MAP_READ;

		case ukn::GraphicBuffer::WriteOnly:
			return D3D11_MAP_WRITE_DISCARD;
			
		case ukn::GraphicBuffer::ReadWrite:
			return D3D11_MAP_READ_WRITE;
		}
	}

	D3D11VertexBuffer::D3D11VertexBuffer(GraphicBuffer::Access access,
				GraphicBuffer::Usage usage,
				uint32 desired_count,
				const void* initData,
				const vertex_elements_type& format,
				D3D11GraphicDevice* device):
	GraphicBuffer(access, usage),
	mFormat(format),
	mMaped(false),
	mDevice(device),
	mOffset(0) {

		D3D11_BUFFER_DESC vertexBufferDesc;

		ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
		vertexBufferDesc.Usage = GraphicBufferUsageToD3D11Usage(usage);
		vertexBufferDesc.ByteWidth = GetVertexElementsTotalSize(format) * desired_count;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = GraphicBufferAccessToD3D11Access(access);
		vertexBufferDesc.MiscFlags = 0;

		HRESULT result;
		if(initData) {
			D3D11_SUBRESOURCE_DATA vertexData;
			ZeroMemory(&vertexData, sizeof(vertexData));
			vertexData.pSysMem = initData;

			result = mDevice->getD3DDevice()->CreateBuffer(&vertexBufferDesc, &vertexData, &mBuffer);
			if(!D3D11Debug::CHECK_RESULT(result, L"CreateVertexBuffer")) {
				log_error(L"error creating vertex buffer");
				mBuffer = 0;
			}
		} else {
			result = mDevice->getD3DDevice()->CreateBuffer(&vertexBufferDesc, 0, &mBuffer);
			if(!D3D11Debug::CHECK_RESULT(result, L"CreateVertexBuffer")) {
				log_error(L"error creating vertex buffer");
				mBuffer = 0;
			}
		}

		mCount = desired_count;
	}

	D3D11VertexBuffer::~D3D11VertexBuffer() {
		if(mBuffer) {
			mBuffer->Release();
		}
	}

	void* D3D11VertexBuffer::map() {
		if(!mBuffer || mMaped)
			return 0;

        D3D11_MAPPED_SUBRESOURCE mappedDtata;
		
		if(FAILED(mDevice->getD3DDeviceContext()->Map(
            mBuffer,
            0,
            GraphicBufferAccessToD3D11Map(this->access()), 
            0, 
            &mappedDtata))) {
		    log_error("Error mapping d3d11 vertex buffer");
            return 0;
        }
		mMaped = true;
		return mappedDtata.pData;
	}

	void D3D11VertexBuffer::unmap() {
		if(mBuffer && mMaped) {
               mDevice->getD3DDeviceContext()->Unmap(mBuffer, 0);
               mMaped = false;
        }
	}

	void D3D11VertexBuffer::activate() {
		if(mBuffer && mDevice) {
			UINT stride = GetVertexElementsTotalSize(this->format());
			mDevice->getD3DDeviceContext()->IASetVertexBuffers(0, 
													           1, 
														       &mBuffer, 
														       &stride, 
														       &mOffset);
		}
	}

	void D3D11VertexBuffer::deactivate() {
		UINT empty = NULL;
		ID3D11Buffer* emptyBuffer = NULL;
		if(mDevice) {
			mDevice->getD3DDeviceContext()->IASetVertexBuffers(0, 
                                                               1, 
                                                               &emptyBuffer, 
                                                               &empty, 
                                                               &empty);
		}
	}

	uint32 D3D11VertexBuffer::offset() const {
		return mOffset;
	}

	void D3D11VertexBuffer::setOffset(uint32 offset) {
		mOffset = offset;
	}

	bool D3D11VertexBuffer::isInMemory() const {
		return false;
	}

	vertex_elements_type& D3D11VertexBuffer::format() {
		return mFormat;
	}

	const vertex_elements_type& D3D11VertexBuffer::format() const {
		return mFormat;
	}

	uint32 D3D11VertexBuffer::count() const {
		return mCount;
	}

	void D3D11VertexBuffer::resize(uint32 desired_count) {
		ID3D11Buffer* buffer;

		D3D11_BUFFER_DESC vertexBufferDesc;

		vertexBufferDesc.Usage = GraphicBufferUsageToD3D11Usage(usage());
		vertexBufferDesc.ByteWidth = GetVertexElementsTotalSize(format()) * desired_count;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = GraphicBufferAccessToD3D11Access(access());
		vertexBufferDesc.MiscFlags = 0;

		if(!D3D11Debug::CHECK_RESULT(mDevice->getD3DDevice()->CreateBuffer(&vertexBufferDesc, 0, &buffer),
									 L"Resize vertex bfufer")) {
			log_error(L"error resizing vertex buffer");
		} else {
			mCount = desired_count;
			
			if(mBuffer) {
				mBuffer->Release();
			}
			mBuffer = buffer;
		}
	}

	D3D11IndexBuffer::D3D11IndexBuffer(GraphicBuffer::Access access,
				GraphicBuffer::Usage usage,
				uint32 desired_count,
				const void* initData,
				D3D11GraphicDevice* device):
	GraphicBuffer(access, usage),
	mMaped(false),
	mDevice(device) {

		D3D11_BUFFER_DESC indexBufferDesc;

		indexBufferDesc.Usage = GraphicBufferUsageToD3D11Usage(usage);
		indexBufferDesc.ByteWidth = sizeof(uint32) * desired_count;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = GraphicBufferAccessToD3D11Access(access);
		indexBufferDesc.MiscFlags = 0;

		HRESULT result;
		if(initData) {
			D3D11_SUBRESOURCE_DATA indexData;
			indexData.pSysMem = initData;

			result = mDevice->getD3DDevice()->CreateBuffer(&indexBufferDesc, &indexData, &mBuffer);
			if(!D3D11Debug::CHECK_RESULT(result, L"CreateIndexBuffer")) {
				log_error(L"error creating index buffer");
				mBuffer = 0;
			}
		} else {
			result = mDevice->getD3DDevice()->CreateBuffer(&indexBufferDesc, 0, &mBuffer);
			if(!D3D11Debug::CHECK_RESULT(result, L"CreateIndexBuffer")) {
				log_error(L"error creating index buffer");
				mBuffer = 0;
			}
		}

		mCount = desired_count;
	}

	D3D11IndexBuffer::~D3D11IndexBuffer() {
		if(mBuffer)
			mBuffer->Release();
	}

	void* D3D11IndexBuffer::map() {
		if(!mBuffer || mMaped)
			return 0;

		D3D11_MAPPED_SUBRESOURCE mappedDtata;
		
		if(FAILED(mDevice->getD3DDeviceContext()->Map(
            mBuffer,
            0,
            GraphicBufferAccessToD3D11Map(this->access()), 
            0, 
            &mappedDtata))) {
		    log_error("Error mapping d3d11 vertex buffer");
            return 0;
        }
		mMaped = true;
		return mappedDtata.pData;
	}

	void D3D11IndexBuffer::unmap() {
		if(mBuffer && mMaped) {
               mDevice->getD3DDeviceContext()->Unmap(mBuffer, 0);
               mMaped = false;
        }
	}

	void D3D11IndexBuffer::activate() {
		if(mBuffer && mDevice) {
			mDevice->getD3DDeviceContext()->IASetIndexBuffer(mBuffer,
													         DXGI_FORMAT_R32_UINT,
													         0);
		}
	}

	void D3D11IndexBuffer::deactivate() {
		ID3D11Buffer* emptyBuffer = NULL;
		if(mDevice) {
			mDevice->getD3DDeviceContext()->IASetIndexBuffer(emptyBuffer,
													         DXGI_FORMAT_R32_UINT,
													         0);
		}
	}
	
	uint32 D3D11IndexBuffer::offset() const {
		return mOffset;
	}

	void D3D11IndexBuffer::setOffset(uint32 offset) {
		mOffset = offset;
	}

	bool D3D11IndexBuffer::isInMemory() const {
		return false;
	}

	uint32 D3D11IndexBuffer::count() const {
		return mCount;
	}

	void D3D11IndexBuffer::resize(uint32 desired_count) {
		ID3D11Buffer* buffer;

		D3D11_BUFFER_DESC indexBufferDesc;

		indexBufferDesc.Usage = GraphicBufferUsageToD3D11Usage(usage());
		indexBufferDesc.ByteWidth = sizeof(uint32) * desired_count;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = GraphicBufferAccessToD3D11Access(access());
		indexBufferDesc.MiscFlags = 0;

		HRESULT result = mDevice->getD3DDevice()->CreateBuffer(&indexBufferDesc, 0, &buffer);
		if(!D3D11Debug::CHECK_RESULT(mDevice->getD3DDevice()->CreateBuffer(&indexBufferDesc, 0, &buffer),
									 L"Resize index bfufer")) {
			log_error(L"error resizeing index buffer");

		} else {
			mCount = desired_count;
	
			if(mBuffer)
				mBuffer->Release();
			mBuffer = buffer;
		}
	}

}