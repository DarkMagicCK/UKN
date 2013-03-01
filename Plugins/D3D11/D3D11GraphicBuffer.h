
#include "ukn/GraphicBuffer.h"

#include "D3D11Preq.h"

namespace ukn {

	class D3D11GraphicDevice;

	class D3D11VertexBuffer: public GraphicBuffer {
    public:
        D3D11VertexBuffer(GraphicBuffer::Access access,
						  GraphicBuffer::Usage usage,
						  uint32 desired_count,
						  const void* initData,
                          const vertex_elements_type& format,
					      D3D11GraphicDevice* device);

        virtual ~D3D11VertexBuffer();

        void* map() override;
        void unmap() override;

        void activate() override;
        void deactivate() override;

        vertex_elements_type& format();
        const vertex_elements_type& format() const;

        uint32 count() const override;
        void resize(uint32 desired_count) override;
        void copyBuffer(const GraphicBufferPtr& to) override;

		uint32 offset() const;
		void setOffset(uint32 offset);

        ID3D11Buffer* getD3DBuffer() const;

    private:
        bool mMaped;
        uint32 mCount;
        vertex_elements_type mFormat;

		COM<ID3D11Buffer>::Ptr mBuffer;
		D3D11GraphicDevice* mDevice;

		uint32 mOffset;
    };

    class D3D11IndexBuffer: public GraphicBuffer {
    public:
        D3D11IndexBuffer(GraphicBuffer::Access acess,
						 GraphicBuffer::Usage usage,
						 uint32 desired_count,
						 const void* initData,
						 D3D11GraphicDevice* device);

        virtual ~D3D11IndexBuffer();

        void* map() override;
        void unmap() override;

        void activate() override;
        void deactivate() override;
        
        uint32 count() const override;
        void resize(uint32 desired_count) override;
        void copyBuffer(const GraphicBufferPtr& to) override;
		
		uint32 offset() const;
		void setOffset(uint32 offset);

        ID3D11Buffer* getD3DBuffer() const;

    private:
        bool mMaped;
        uint32 mCount;

		COM<ID3D11Buffer>::Ptr mBuffer;
		D3D11GraphicDevice* mDevice;

		uint32 mOffset;
    };

}