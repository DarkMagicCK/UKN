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
        case ukn::GraphicBuffer::Staging:
            return D3D11_USAGE_STAGING;
		}
        return D3D11_USAGE_DEFAULT;
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
        return 0;
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
        return D3D11_MAP_READ;
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
        ID3D11Buffer* buffer;
		if(initData) {
			D3D11_SUBRESOURCE_DATA vertexData;
			ZeroMemory(&vertexData, sizeof(vertexData));
			vertexData.pSysMem = initData;

			result = mDevice->getD3DDevice()->CreateBuffer(&vertexBufferDesc, &vertexData, &buffer);
			if(!D3D11Debug::CHECK_RESULT(result, L"CreateVertexBuffer")) {
				log_error(L"error creating vertex buffer");
			} else 
                mBuffer = MakeCOMPtr(buffer);
		} else {
			result = mDevice->getD3DDevice()->CreateBuffer(&vertexBufferDesc, 0, &buffer);
			if(!D3D11Debug::CHECK_RESULT(result, L"CreateVertexBuffer")) {
				log_error(L"error creating vertex buffer");
			} else 
                mBuffer = MakeCOMPtr(buffer);
		}

		mCount = desired_count;
	}

	D3D11VertexBuffer::~D3D11VertexBuffer() {

	}

	void* D3D11VertexBuffer::map() {
        if(access() == None) {
            log_error("D3D11VertexBuffer: trying to map buffer with access = None");
            return 0;
        }
		if(!mBuffer || mMaped)
			return 0;

        D3D11_MAPPED_SUBRESOURCE mappedDtata;
		
		if(FAILED(mDevice->getD3DDeviceContext()->Map(
            mBuffer.get(),
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
            mDevice->getD3DDeviceContext()->Unmap(mBuffer.get(), 0);
            mMaped = false;
        }
	}

	void D3D11VertexBuffer::activate() {
		// pass
	}

	void D3D11VertexBuffer::deactivate() {
		// pass
	}

    ID3D11Buffer* D3D11VertexBuffer::getD3DBuffer() const {
        return mBuffer.get();
    }

	uint32 D3D11VertexBuffer::offset() const {
		return mOffset;
	}

	void D3D11VertexBuffer::setOffset(uint32 offset) {
		mOffset = offset;
	}

	void D3D11VertexBuffer::copyBuffer(const GraphicBufferPtr& to) {
        if(to) {
            D3D11VertexBuffer* dest = checked_cast<D3D11VertexBuffer*>(to.get());
            mDevice->getD3DDeviceContext()->CopyResource(this->mBuffer.get(),
                                                         dest->getD3DBuffer());
        }
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
            mBuffer = MakeCOMPtr(buffer);
		}
	}

	D3D11IndexBuffer::D3D11IndexBuffer(GraphicBuffer::Access access,
				GraphicBuffer::Usage usage,
				uint32 desired_count,
				const void* initData,
				D3D11GraphicDevice* device):
	GraphicBuffer(access, usage),
	mMaped(false),
	mDevice(device),
    mOffset(0) {

		D3D11_BUFFER_DESC indexBufferDesc;

		indexBufferDesc.Usage = GraphicBufferUsageToD3D11Usage(usage);
		indexBufferDesc.ByteWidth = sizeof(uint32) * desired_count;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = GraphicBufferAccessToD3D11Access(access);
		indexBufferDesc.MiscFlags = 0;

		HRESULT result;
        ID3D11Buffer* buffer;
		if(initData) {
			D3D11_SUBRESOURCE_DATA indexData;
			indexData.pSysMem = initData;

			result = mDevice->getD3DDevice()->CreateBuffer(&indexBufferDesc, &indexData, &buffer);
			if(!D3D11Debug::CHECK_RESULT(result, L"CreateIndexBuffer")) {
				log_error(L"error creating index buffer");
			} else
                mBuffer = MakeCOMPtr(buffer);
		} else {
			result = mDevice->getD3DDevice()->CreateBuffer(&indexBufferDesc, 0, &buffer);
			if(!D3D11Debug::CHECK_RESULT(result, L"CreateIndexBuffer")) {
				log_error(L"error creating index buffer");
			} else
                mBuffer = MakeCOMPtr(buffer);
		}

		mCount = desired_count;
	}

	D3D11IndexBuffer::~D3D11IndexBuffer() {

	}

	void* D3D11IndexBuffer::map() {
        if(access() == None) {
            log_error("D3D11IndexBuffer: trying to map buffer with access = None");
            return 0;
        }
		if(!mBuffer || mMaped)
			return 0;

		D3D11_MAPPED_SUBRESOURCE mappedDtata;
		
		if(FAILED(mDevice->getD3DDeviceContext()->Map(
            mBuffer.get(),
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
               mDevice->getD3DDeviceContext()->Unmap(mBuffer.get(), 0);
               mMaped = false;
        }
	}

	void D3D11IndexBuffer::activate() {
		if(mBuffer && mDevice) {
			mDevice->getD3DDeviceContext()->IASetIndexBuffer(mBuffer.get(),
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

	void D3D11IndexBuffer::copyBuffer(const GraphicBufferPtr& to) {
        if(to) {
            D3D11IndexBuffer* dest = checked_cast<D3D11IndexBuffer*>(to.get());
            mDevice->getD3DDeviceContext()->CopyResource(this->mBuffer.get(),
                                                         dest->getD3DBuffer());
        }
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
	        mBuffer = MakeCOMPtr(buffer);
		}
	}

    ID3D11Buffer* D3D11IndexBuffer::getD3DBuffer() const {
        return mBuffer.get();
    }

}