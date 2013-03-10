#ifndef PROJECT_UNKNOWN_TERRIAN_H_
#define PROJECT_UNKNOWN_TERRIAN_H_

#include "UKN/PreDeclare.h"
#include "UKN/Vertex.h"
#include "UKN/Renderable.h"
#include "UKN/Frustum.h"

#include "mist/Color.h"

namespace ukn {

    class UKN_API HeightMap {
    public:
        enum IndexMode {
            Triangle, // 2 triangles
            Grid, // 4 lines
        };
        static void Generate(float x, float y,
                             uint32 w, uint32 h, 
                             float grid_size,
                             const Function<float(float, float)>& heightFunc,
                             std::vector<float3>& positions,
                             std::vector<uint32>& indices,
                             IndexMode mode = Triangle);

    };

    class UKN_API Terrian: Uncopyable, public Renderable {
    public:
        virtual ~Terrian();

        virtual bool build() = 0;
        virtual void render(const EffectTechniquePtr& technique) = 0;
        virtual uint32 getVertexCount() const = 0;
        virtual uint32 getIndexCount() const = 0;
        virtual uint32 getDrawCount() const = 0;
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
        virtual bool build();
        virtual void render(const EffectTechniquePtr& technique);

        /* should be called before build */
        GridTerrian& position(const float3& pos);
        GridTerrian& noise(float noise);
        GridTerrian& noiseWeight(float noiseWeight);
        GridTerrian& size(const float2& size);
        GridTerrian& grid_size(float grid_size);

        uint32 getVertexCount() const;
        uint32 getIndexCount() const;
        virtual uint32 getDrawCount () const;

        virtual const UknString& getName() const;
        
        virtual Box getBound() const;
        virtual RenderBufferPtr getRenderBuffer() const;
        
        virtual void onRenderBegin();
        virtual void onRenderEnd();
        
    protected:
        float heightFunc(float x, float y);
        uint32 mWidth, mHeight;
        float3 mPosition;
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
        struct Node {
            Node(float _x, float _z, float _w);

            float x, z;
            float width;
            float min_h, max_h;
            uint32 index_start;
            uint32 index_count;
            
            Node* childs[4];
        };

        /* can be called after build */
        GridTerrianLightening& texture(const TexturePtr& tex);
        GridTerrianLightening& textureRepeat(uint32 texRepeat);

        virtual bool build();
        virtual void render(const EffectTechniquePtr& technique);

        uint32 getDrawCount() const;

    protected:
        Node* createNode(float x, float z, float w, VertexUVNormal* vertices, uint32 pitch, uint32 h, std::vector<uint32>& indices);
        uint32 triangleCount(float x, float z, float w, VertexUVNormal* vertices, uint32 pitch, uint32 h, std::vector<uint32>& indices, float& maxh, float &minh);
        void releaseNode(Node* node);
        void renderNode(const EffectTechniquePtr& technique, Node* node, GraphicDevice& device, const Frustum& frustum);

        Node* mRoot;
        uint32 mDrawCount;

        float heightFunc(float, float);

        // repeat in x,y defualt = size / 10;
        uint32 mTextureRepeat;
    };



} // namespace ukn

#endif