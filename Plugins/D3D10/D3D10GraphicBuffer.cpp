#include "D3D10GraphicBuffer.h"
#include "D3D10GraphicDevice.h"

#include "D3D10Convert.h"

#include "mist/Logger.h"

namespace ukn {

	
	inline D3D10_USAGE GraphicBufferUsageToD3D10Usage(ukn::GraphicBuffer::Usage usage) {
		switch (usage) {
		case ukn::GraphicBuffer::Static:
			return D3D10_USAGE_DEFAULT;
		case ukn::GraphicBuffer::Dynamic:
			return D3D10_USAGE_DYNAMIC;
		}
	}

	inline UINT GraphicBufferAccessToD3D10Access(ukn::GraphicBuffer::Access access) {
		switch (access) {
		case ukn::GraphicBuffer::ReadOnly:
			return D3D10_CPU_ACCESS_READ;

		case ukn::GraphicBuffer::WriteOnly:
			return D3D10_CPU_ACCESS_WRITE;
			
		case ukn::GraphicBuffer::ReadWrite:
			return D3D10_CPU_ACCESS_READ | D3D10_CPU_ACCESS_WRITE;
		}
	}

	inline D3D10_MAP GraphicBufferAccessToD3D10Map(ukn::GraphicBuffer::Access access) {
		switch (access) {
		case ukn::GraphicBuffer::ReadOnly:
			return D3D10_MAP_READ;

		case ukn::GraphicBuffer::WriteOnly:
			return D3D10_MAP_WRITE;
			
		case ukn::GraphicBuffer::ReadWrite:
			return D3D10_MAP_READ_WRITE;
		}
	}

	D3D10VertexBuffer::D3D10VertexBuffer(GraphicBuffer::Access access,
				GraphicBuffer::Usage usage,
				uint32 desired_count,
				const void* initData,
				const VertexFormat& format,
				D3D10GraphicDevice* device):
	GraphicBuffer(access, usage),
	mFormat(format),
	mMaped(false),
	mDevice(device),
	mOffset(0) {

		D3D10_BUFFER_DESC vertexBufferDesc;

		ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
		vertexBufferDesc.Usage = GraphicBufferUsageToD3D10Usage(usage);
		vertexBufferDesc.ByteWidth = format.totalSize() * desired_count;
		vertexBufferDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = GraphicBufferAccessToD3D10Access(access);
		vertexBufferDesc.MiscFlags = 0;

		HRESULT result;
		if(initData) {
			D3D10_SUBRESOURCE_DATA vertexData;
			ZeroMemory(&vertexData, sizeof(vertexData));
			vertexData.pSysMem = initData;

			result = mDevice->getD3DDevice()->CreateBuffer(&vertexBufferDesc, &vertexData, &mBuffer);
			if(!D3D10Debug::CHECK_RESULT(result, L"CreateVertexBuffer")) {
				log_error(L"error creating vertex buffer");
				mBuffer = 0;
			}
		} else {
			result = mDevice->getD3DDevice()->CreateBuffer(&vertexBufferDesc, 0, &mBuffer);
			if(!D3D10Debug::CHECK_RESULT(result, L"CreateVertexBuffer")) {
				log_error(L"error creating vertex buffer");
				mBuffer = 0;
			}
		}

		mCount = desired_count;
	}

	D3D10VertexBuffer::~D3D10VertexBuffer() {
		if(mBuffer) {
			mBuffer->Release();
		}
	}

	void* D3D10VertexBuffer::map() {
		if(!mBuffer || mMaped)
			return 0;

		void* data;
		HRESULT result = mBuffer->Map(GraphicBufferAccessToD3D10Map(this->access()), 0, &data);
		if(FAILED(result))
			return 0;
		mMaped = true;
		return data;
	}

	void D3D10VertexBuffer::unmap() {
		if(mBuffer && mMaped)
			mBuffer->Unmap();
	}

	void D3D10VertexBuffer::activate() {
		if(mBuffer && mDevice) {
			UINT stride = this->format().totalSize();
			mDevice->getD3DDevice()->IASetVertexBuffers(0, 
													    1, 
														&mBuffer, 
														&stride, 
														&mOffset);
		}
	}

	void D3D10VertexBuffer::deactivate() {
		UINT empty = NULL;
		ID3D10Buffer* emptyBuffer = NULL;
		if(mDevice) {
			mDevice->getD3DDevice()->IASetVertexBuffers(0, 1, &emptyBuffer, &empty, &empty);
		}
	}

	uint32 D3D10VertexBuffer::offset() const {
		return mOffset;
	}

	void D3D10VertexBuffer::setOffset(uint32 offset) {
		mOffset = offset;
	}

	bool D3D10VertexBuffer::isInMemory() const {
		return false;
	}

	VertexFormat& D3D10VertexBuffer::format() {
		return mFormat;
	}

	const VertexFormat& D3D10VertexBuffer::format() const {
		return mFormat;
	}

	uint32 D3D10VertexBuffer::count() const {
		return mCount;
	}

	void D3D10VertexBuffer::resize(uint32 desired_count) {
		ID3D10Buffer* buffer;

		D3D10_BUFFER_DESC vertexBufferDesc;

		vertexBufferDesc.Usage = GraphicBufferUsageToD3D10Usage(usage());
		vertexBufferDesc.ByteWidth = format().totalSize() * desired_count;
		vertexBufferDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = GraphicBufferAccessToD3D10Access(access());
		vertexBufferDesc.MiscFlags = 0;

		if(!D3D10Debug::CHECK_RESULT(mDevice->getD3DDevice()->CreateBuffer(&vertexBufferDesc, 0, &buffer),
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

	D3D10IndexBuffer::D3D10IndexBuffer(GraphicBuffer::Access access,
				GraphicBuffer::Usage usage,
				uint32 desired_count,
				const void* initData,
				D3D10GraphicDevice* device):
	GraphicBuffer(access, usage),
	mMaped(false),
	mDevice(device) {

		D3D10_BUFFER_DESC indexBufferDesc;

		indexBufferDesc.Usage = GraphicBufferUsageToD3D10Usage(usage);
		indexBufferDesc.ByteWidth = sizeof(uint32) * desired_count;
		indexBufferDesc.BindFlags = D3D10_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = GraphicBufferAccessToD3D10Access(access);
		indexBufferDesc.MiscFlags = 0;

		HRESULT result;
		if(initData) {
			D3D10_SUBRESOURCE_DATA indexData;
			indexData.pSysMem = initData;

			result = mDevice->getD3DDevice()->CreateBuffer(&indexBufferDesc, &indexData, &mBuffer);
			if(!D3D10Debug::CHECK_RESULT(result, L"CreateIndexBuffer")) {
				log_error(L"error creating index buffer");
				mBuffer = 0;
			}
		} else {
			result = mDevice->getD3DDevice()->CreateBuffer(&indexBufferDesc, 0, &mBuffer);
			if(!D3D10Debug::CHECK_RESULT(result, L"CreateIndexBuffer")) {
				log_error(L"error creating index buffer");
				mBuffer = 0;
			}
		}

		mCount = desired_count;
	}

	D3D10IndexBuffer::~D3D10IndexBuffer() {
		if(mBuffer)
			mBuffer->Release();
	}

	void* D3D10IndexBuffer::map() {
		if(!mBuffer || mMaped)
			return 0;

		void* data;
		HRESULT result = mBuffer->Map(GraphicBufferAccessToD3D10Map(this->access()), 0, &data);
		if(FAILED(result))
			return 0;
		mMaped = true;
		return data;
	}

	void D3D10IndexBuffer::unmap() {
		if(mBuffer && mMaped)
			mBuffer->Unmap();
	}

	void D3D10IndexBuffer::activate() {
		if(mBuffer && mDevice) {
			mDevice->getD3DDevice()->IASetIndexBuffer(mBuffer,
													  DXGI_FORMAT_R32_UINT,
													  0);
		}
	}

	void D3D10IndexBuffer::deactivate() {
		ID3D10Buffer* emptyBuffer = NULL;
		if(mDevice) {
			mDevice->getD3DDevice()->IASetIndexBuffer(emptyBuffer,
													  DXGI_FORMAT_R32_UINT,
													  0);
		}
	}
	
	uint32 D3D10IndexBuffer::offset() const {
		return mOffset;
	}

	void D3D10IndexBuffer::setOffset(uint32 offset) {
		mOffset = offset;
	}

	bool D3D10IndexBuffer::isInMemory() const {
		return false;
	}

	uint32 D3D10IndexBuffer::count() const {
		return mCount;
	}

	void D3D10IndexBuffer::resize(uint32 desired_count) {
		ID3D10Buffer* buffer;

		D3D10_BUFFER_DESC indexBufferDesc;

		indexBufferDesc.Usage = GraphicBufferUsageToD3D10Usage(usage());
		indexBufferDesc.ByteWidth = sizeof(uint32) * desired_count;
		indexBufferDesc.BindFlags = D3D10_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = GraphicBufferAccessToD3D10Access(access());
		indexBufferDesc.MiscFlags = 0;

		HRESULT result = mDevice->getD3DDevice()->CreateBuffer(&indexBufferDesc, 0, &buffer);
		if(!D3D10Debug::CHECK_RESULT(mDevice->getD3DDevice()->CreateBuffer(&indexBufferDesc, 0, &buffer),
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