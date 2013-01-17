
#include "ukn/GraphicBuffer.h"

#include "D3D10Preq.h"

namespace ukn {

	class D3D10GraphicDevice;

	class D3D10VertexBuffer: public GraphicBuffer {
    public:
        D3D10VertexBuffer(GraphicBuffer::Access access,
						  GraphicBuffer::Usage usage,
						  uint32 desired_count,
						  const void* initData,
                          const VertexFormat& format,
					      D3D10GraphicDevice* device);

        virtual ~D3D10VertexBuffer();

        virtual void* map();
        virtual void unmap();

        virtual void activate();
        virtual void deactivate();
        virtual bool isInMemory() const;

        VertexFormat& format();
        const VertexFormat& format() const;

        uint32 count() const;
        void resize(uint32 desired_count);

		uint32 offset() const;
		void setOffset(uint32 offset);

    private:
        bool mMaped;
        uint32 mCount;
        VertexFormat mFormat;

		ID3D10Buffer* mBuffer;
		D3D10GraphicDevice* mDevice;

		uint32 mOffset;
    };

    class D3D10IndexBuffer: public GraphicBuffer {
    public:
        D3D10IndexBuffer(GraphicBuffer::Access acess,
						 GraphicBuffer::Usage usage,
						 uint32 desired_count,
						 const void* initData,
						 D3D10GraphicDevice* device);

        virtual ~D3D10IndexBuffer();

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

		ID3D10Buffer* mBuffer;
		D3D10GraphicDevice* mDevice;

		uint32 mOffset;
    };

}