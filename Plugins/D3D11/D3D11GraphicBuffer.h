
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

        virtual void* map();
        virtual void unmap();

        virtual void activate();
        virtual void deactivate();
        virtual bool isInMemory() const;

        vertex_elements_type& format();
        const vertex_elements_type& format() const;

        uint32 count() const;
        void resize(uint32 desired_count);

		uint32 offset() const;
		void setOffset(uint32 offset);

    private:
        bool mMaped;
        uint32 mCount;
        vertex_elements_type mFormat;

		ID3D11Buffer* mBuffer;
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

        virtual void* map();
        virtual void unmap();

        virtual void activate();
        virtual void deactivate();
        virtual bool isInMemory() const;
        
        uint32 count() const;
        void resize(uint32 desired_count);
		
		uint32 offset() const;
		void setOffset(uint32 offset);

    private:
        bool mMaped;
        uint32 mCount;

		ID3D11Buffer* mBuffer;
		D3D11GraphicDevice* mDevice;

		uint32 mOffset;
    };

}