#include "UKN/PreDeclare.h"
#include "UKN/Vertex.h"
#include "mist/Color.h"

namespace ukn {

    class UKN_API Terrian: Uncopyable {
    public:
        virtual ~Terrian();

        virtual void render() = 0;
        virtual uint32 getVertexCount() const = 0;
        virtual uint32 getIndexCount() const = 0;
    };

    typedef SharedPtr<Terrian> TerrianPtr;

    class UKN_API GridTerrian: public Terrian {
    public:
        GridTerrian();
        virtual ~GridTerrian();

    public:
        struct VertexFormat {
            float3 position;
            uint32 color;

            static vertex_elements_type Format();
        };

        // vertex count = w * h, default 100x100
        // index count = (w-1) * (h-1) * 8
        bool build(float y, const float2& size = float2(100, 100), float grid_size = 1);

        void render();

        uint32 getVertexCount() const;
        uint32 getIndexCount() const;

    private:
        uint32 mWidth, mHeight;
        
        RenderBufferPtr mRenderBuffer;
        GraphicBufferPtr mVertexBuffer;
        GraphicBufferPtr mIndexBuffer;
    };



} // namespace ukn