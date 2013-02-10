#include "UKN/PreDeclare.h"
#include "UKN/Vertex.h"
#include "mist/Color.h"

namespace ukn {

    class UKN_API Terrian: Uncopyable {
    public:
        virtual ~Terrian();

        virtual bool build() = 0;
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
        // noise = perlin noise weight
        // noiseWeight = vertex height weight 
        bool build();

        void render();

        /* should be called before build */
        GridTerrian& y(float y);
        GridTerrian& noise(float noise);
        GridTerrian& noiseWeight(float noiseWeight);
        GridTerrian& size(const float2& size);
        GridTerrian& grid_size(float grid_size);

        uint32 getVertexCount() const;
        uint32 getIndexCount() const;

    protected:
        uint32 mWidth, mHeight;
        float mY;
        float mNoise, mNoiseWeight;
        float mGridSize;
        
        RenderBufferPtr mRenderBuffer;
        GraphicBufferPtr mVertexBuffer;
        GraphicBufferPtr mIndexBuffer;
    };

    class UKN_API GridTerrianLightening: public GridTerrian {
    public:
        GridTerrianLightening();
        virtual ~GridTerrianLightening();

    public:
        struct VertexFormat {
            float3 position;
            float3 normal;
            float2 uv;

            static vertex_elements_type Format();
        };

        /* can be called after build */
        GridTerrianLightening& texture(const TexturePtr& tex);

        GridTerrianLightening& textureRepeat(uint32 texRepeat);

        bool build();

        void render();

    protected:
        TexturePtr mTexture;
        // repeat in x,y defualt = size / 10;
        uint32 mTextureRepeat;
    };



} // namespace ukn