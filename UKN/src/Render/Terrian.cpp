#include "UKN/Terrian.h"
#include "UKN/RenderBuffer.h"
#include "UKN/GraphicBuffer.h"
#include "UKN/Context.h"
#include "UKN/GraphicFactory.h"
#include "UKN/GraphicDevice.h"
#include "UKN/Shader.h"
#include "UKN/FrameBuffer.h"
#include "UKN/Renderable.h"

#include "mist/MemoryUtil.h"
#include "mist/Profiler.h"

// FLT_MAX FLT_MIN
#include <cfloat>

namespace ukn {

    void HeightMap::Generate(float startx, float starty,
                             uint32 w, uint32 h,
                             float grid_size,
                             const Function<float(float, float)>& heightFunc,
                             std::vector<float3>& positions,
                             std::vector<uint32>& indices,
                             IndexMode mode) {
        uint32 vertexSize = w * h;
        uint32 indexSize = (w - 1) * (h - 1) * 6;
        if(mode == Grid)
            indexSize = (w - 1) * (h - 1) * 8;
        uint32 index = 0;

        positions.resize(vertexSize);
        indices.resize(indexSize);
        for(uint32 z = 0; z < h; ++z) {
            for(uint32 x = 0; x < w; ++x) {
                uint32 current (z * w + x);

                float height = heightFunc((float)x / w, 
                                          (float)z / h);
                positions[current] = float3(startx + x * grid_size, 
                                            height, 
                                            starty + z * grid_size);
                if(z < w - 1 &&
                    x < h - 1) {
                        uint32 r = z * w + x + 1;
                        uint32 br = (z + 1) * w + x + 1;
                        uint32 bl = (z + 1) * h + x;

                        if(mode == Grid) {
                            indices[index++] = current; indices[index++] = r;
                            indices[index++] = r; indices[index++] = br;
                            indices[index++] = br; indices[index++] = bl;
                            indices[index++] = bl; indices[index++] = current;
                        } else {
                            indices[index++] = current; indices[index++] = r;
                            indices[index++] = br; indices[index++] = br;
                            indices[index++] = bl; indices[index++] = current;
                        }
                }
            }
        }
    }
    
    Terrian::~Terrian() {

    }

    vertex_elements_type GridTerrian::VertexFormat::Format() {
        static vertex_elements_type _format = VertexElementsBuilder()
            .add(VertexElement(VU_Position, EF_Float3, 0))
            .add(VertexElement(VU_Diffuse, EF_UInt32, 0))
            .to_vector();
        return _format;
    }

    GridTerrian::GridTerrian():
    mWidth(100), mHeight(100),
    mPosition(float3(0, 0, 0)),
    mNoise(10), mNoiseWeight(5),
    mGridSize(1) {

    }

    GridTerrian::~GridTerrian() {

    }
    
    GridTerrian& GridTerrian::position(const float3& pos) {
        this->mPosition = pos;
        return *this;
    }

    GridTerrian& GridTerrian::noise(float noise) {
        this->mNoise = noise;
        return *this;
    }

    GridTerrian& GridTerrian::noiseWeight(float noiseWeight) {
        this->mNoiseWeight = noiseWeight;
        return *this;
    }

    GridTerrian& GridTerrian::size(const float2& size) {
        this->mWidth = (uint32)size[0]; this->mHeight = (uint32)size[1];
        return *this;
    }

    GridTerrian& GridTerrian::grid_size(float grid_size) {
        this->mGridSize = grid_size;
        return *this;
    }

    bool GridTerrian::build() {
        uint32 vertexSize = this->mWidth * this->mHeight;
        uint32 indexSize = (this->mWidth - 1) * (this->mHeight - 1) * 8;
        VertexFormat* vertices = mist_malloc_t(VertexFormat, vertexSize);
        uint32* indices = mist_malloc_t(uint32, indexSize);
        uint32 index = 0;
        for(uint32 z = 0; z < this->mHeight; ++z) {
            for(uint32 x = 0; x < this->mWidth; ++x) {
                uint32 current = z * this->mWidth + x;

                float h = mist::PerlinNoise::Gen((float)x / mWidth * mNoise, 
                                                 (float)z / mHeight * mNoise, 
                                                 0);
                vertices[current].position = float3(mPosition[0] + x * mGridSize, 
                                                    mPosition[1] * mGridSize + h * mNoiseWeight, 
                                                    mPosition[2] + z * mGridSize);
                vertices[current].color  = mist::color::Black.toARGB();
                if(z < this->mHeight - 1 &&
                    x < this->mWidth - 1) {
                        uint32 r = z * this->mWidth + x + 1;
                        uint32 br = (z + 1) * this->mWidth + x + 1;
                        uint32 bl = (z + 1) * this->mWidth + x;

                        indices[index++] = current; indices[index++] = r;
                        indices[index++] = r; indices[index++] = br;
                        indices[index++] = br; indices[index++] = bl;
                        indices[index++] = bl; indices[index++] = current;
                }
            }
        }

        GraphicFactory& gf = Context::Instance().getGraphicFactory();
        mRenderBuffer = gf.createRenderBuffer();
        mVertexBuffer = gf.createVertexBuffer(GraphicBuffer::None,
            GraphicBuffer::Static,
            vertexSize,
            vertices,
            VertexFormat::Format());
        mIndexBuffer = gf.createIndexBuffer(GraphicBuffer::None,
            GraphicBuffer::Static,
            indexSize,
            indices);

        bool error = false;
        if(mRenderBuffer && mIndexBuffer && mVertexBuffer) {
            mRenderBuffer->bindVertexStream(mVertexBuffer, VertexFormat::Format());
            mRenderBuffer->bindIndexStream(mIndexBuffer);
            mRenderBuffer->useIndexStream(true);

            EffectPtr effect = ukn::Context::Instance().getGraphicFactory().createEffect();
            if(effect) {
              
                mRenderBuffer->setRenderMode(RM_Line);
            } else 
                error = true;
        } else
            error = true;

        mist_free(vertices);
        mist_free(indices);

        return !error;
    }

    void GridTerrian::render(const EffectTechniquePtr& technique) {
        if(!mRenderBuffer)
            return;
        GraphicDevice& gd = Context::Instance().getGraphicFactory().getGraphicDevice();

        gd.bindTexture(TexturePtr());
        gd.renderBuffer(technique, mRenderBuffer);
    }

    uint32 GridTerrian::getDrawCount() const {
        return mVertexBuffer->count();
    }

    uint32 GridTerrian::getVertexCount() const {
        if(mVertexBuffer)
            return mVertexBuffer->count();
        return 0;
    }

    uint32 GridTerrian::getIndexCount() const {
        if(mIndexBuffer)
            return mIndexBuffer->count();
        return 0;
    }

    const UknString& GridTerrian::getName() const {
        static UknString _name = L"Terrian";
        return _name;
    }
        
    Box GridTerrian::getBound() const {
        // to do
        return Box();
    }

    RenderBufferPtr GridTerrian::getRenderBuffer() const {
        return mRenderBuffer;
    }
        
    void GridTerrian::onRenderBegin() {

    }

    void GridTerrian::onRenderEnd() {

    }

    GridTerrianLightening::GridTerrianLightening():
    mTextureRepeat(10) {

    }

    GridTerrianLightening::~GridTerrianLightening() {
        releaseNode(mRoot);
    }

    void GridTerrianLightening::releaseNode(Node* node) {
        if(node) {
            for(uint32 i=0; i<4; ++i)
                if(node->childs[i])
                    releaseNode(node->childs[i]);
        }
        delete node;
    }

    GridTerrianLightening& GridTerrianLightening::texture(const TexturePtr& tex) {
        this->mDiffuseTex = tex;
        return *this;
    }

    GridTerrianLightening& GridTerrianLightening::textureRepeat(uint32 texRepeat) {
        this->mTextureRepeat = texRepeat;
        return *this;
    }

    float GridTerrianLightening::heightFunc(float x, float y) {
        return PerlinNoise::Gen(x * 10 ,y * 10 , 0) * mNoiseWeight;
    }

    bool GridTerrianLightening::build() {
        MIST_PROFILE(L"GridTerrianLightening::build");

        std::vector<float3> positions;
        std::vector<uint32> _indices;

        HeightMap::Generate(mPosition[0], mPosition[2], 
                            this->mWidth, this->mHeight,
                            this->mGridSize,
                            Bind(this, &GridTerrianLightening::heightFunc),
                            positions,
                            _indices,
                            HeightMap::Triangle);

        float3* normals = mist_malloc_t(float3, (this->mWidth - 1) * (this->mHeight - 1));
        
        uint32 vertexSize = this->mWidth * this->mHeight;
        VertexUVNormal* vertices = mist_malloc_t(VertexUVNormal, vertexSize);
        
        float uInc = float(mTextureRepeat) / mWidth;
        float vInc = float(mTextureRepeat) / mHeight;

        uint32 uIncCount = mWidth / mTextureRepeat,
               vIncCount = mHeight / mTextureRepeat,
               tuCount = 0, tvCount = 0;

        float tu = 0;
        float tv = 0;
        
        for(uint32 z = 0; z < this->mHeight; ++z) {
            for(uint32 x = 0; x < this->mWidth; ++x) {
                uint32 current = z * this->mWidth + x;

                vertices[current].position = (positions[current] + float3(0, mPosition[1], 0));
                vertices[current].uv = float2(tu, 
                                              tv);
                {
                    tu += uInc;
                    tuCount ++;
                    if(tuCount == uIncCount) {
                        tu = 0.f;
                        tuCount = 0;
                    }
                }
            }

            {
                tv -= vInc;
                tvCount ++;
                if(tvCount == vIncCount) {
                    tv = 1.f;
                    tvCount = 0;
                }
            }
        }

        for(uint32 z = 0; z < this->mHeight - 1; ++z) {
            for(uint32 x = 0; x < this->mWidth - 1; ++x) {
                uint32 tr1 = (z * this->mWidth) + x;
                uint32 tr2 = (z * this->mWidth) + x + 1;
                uint32 tr3 = (z + 1) * this->mWidth + x;

                float3 p1 = vertices[tr1].position;
                float3 p2 = vertices[tr2].position;
                float3 p3 = vertices[tr3].position;

                Vector3 v1 = p1 - p3;
                Vector3 v2 = p3 - p2;

                uint32 i = (z * (mWidth - 1)) + x;
                normals[i]= v1.cross(v2);
            }
        }

        // calculate normals
        uint32 index = 0;
        for(int32 z = 0; z < this->mHeight; ++z) {
            for(int32 x = 0; x < this->mWidth; ++x) {
                float3 sum(0, 0, 0);
                uint32 c = 0;

                if((x-1) >= 0 && (z-1) >= 0) {
                    uint32 i = ((z-1) * (mWidth-1)) + (x-1);
                    sum += normals[i];
                    c++;
                }
                if(x < (mWidth-1) && (z-1) >= 0) {
                    uint32 i= (z-1) * (mWidth-1) + x;
                    sum += normals[i];
                    c++;
                }
                if((x-1) >= 0 && (z < (mHeight-1))) {
                    uint32 i = (z * (mWidth-1)) + (x-1);
                    sum += normals[i];
                    c++;
                }
                if(x < (mWidth-1) && (z < (mHeight-1))) {
                    uint32 i = (z * (mWidth-1)) + x;
                    sum += normals[i];
                    c++;
                }
                sum /= (float)c;

                uint32 current = z * this->mWidth + x;

                vertices[current].normal = sum.normalize();
            }
        }
        mist_free(normals);

        uint32 faceSize = (this->mWidth - 1) * (this->mHeight - 1) * 6;
        VertexUVNormal* faces = mist_malloc_t(VertexUVNormal, faceSize);
        index = 0;
        for(int32 z = 0; z < this->mHeight - 1; ++z) {
            for(int32 x = 0; x < this->mWidth - 1; ++x) {
                uint32 bl = z * this->mWidth + x;
                uint32 br = z * this->mWidth + x + 1;
                uint32 ul = (z + 1) * this->mWidth + x;
                uint32 ur = (z + 1) * this->mWidth + x + 1;

                // upper-left
                faces[index] = vertices[ul];
                if(vertices[ul].uv[1] == 1.0f) faces[index].uv[1] = 0;
                index++;

                // upper-right
                faces[index] = vertices[ur];
                if(vertices[ur].uv[1] == 1.0f) faces[index].uv[1] = 0;
                if(vertices[ur].uv[0] == 0.0f) faces[index].uv[0] = 1.0;
                index++;

                // bottom left
                faces[index++] = vertices[bl];

                // bottom left
                faces[index++] = vertices[bl];

                // upper-right
                faces[index] = vertices[ur];
                if(vertices[ur].uv[1] == 1.0f) faces[index].uv[1] = 0;
                if(vertices[ur].uv[0] == 0.0f) faces[index].uv[0] = 1.0;
                index++;

                // bottom-right
                faces[index] = vertices[br];
                if(vertices[br].uv[0] == 0.0f) faces[index].uv[0] = 1.0;
                index++;
            }
        }

        std::vector<uint32> indices;
        mRoot = this->createNode(mPosition[0] + mWidth / 2 * mGridSize, 
                                 mPosition[2] + mHeight / 2 * mGridSize, 
                                 mWidth * mGridSize, 
                                 faces, 
                                 (mWidth-1) * 6, 
                                 (mHeight-1),
                                 indices);

        GraphicFactory& gf = Context::Instance().getGraphicFactory();
        mRenderBuffer = gf.createRenderBuffer();
        
        mVertexBuffer = gf.createVertexBuffer(
            GraphicBuffer::None,
            GraphicBuffer::Static,
            faceSize,
            faces,
            VertexUVNormal::Format());
        mIndexBuffer = gf.createIndexBuffer(
            GraphicBuffer::None,
            GraphicBuffer::Static,
            (uint32)indices.size(),
            &indices[0]);

        bool error = false;
        if(mRenderBuffer && mVertexBuffer && mIndexBuffer) {
            mRenderBuffer->bindVertexStream(mVertexBuffer, VertexUVNormal::Format());
            mRenderBuffer->setRenderMode(RM_Triangle);

        } else
            error = true;

        mist_free(vertices);

        return !error;
    }

    GridTerrianLightening::Node::Node(float _x, float _z, float _w):
    x(_x), z(_z), width(_w),
    index_count(0), index_start(0) {
        childs[0] = childs[1] = childs[2] = childs[3] = 0;
    }

    GridTerrianLightening::Node* GridTerrianLightening::createNode(float x, float z, float w, VertexUVNormal* vertices, uint32 pitch, uint32 h, std::vector<uint32>& indices) {
        Node* node = new Node(x, z, w);

        std::vector<uint32> triangles;
        uint32 triangle_count = this->triangleCount(x, z, w, vertices, pitch, h, triangles, node->max_h, node->min_h);
        if(triangle_count > 10000) {
            for(int i=0; i<4; ++i) {
                float offX ( (((i % 2) < 1) ? -1.0f : 1.0f) * (w / 4) );
                float offZ ( (((i % 4) < 2) ? -1.0f: 1.0f) * (w / 4) );

                if(w / 2 > 0) {
                    node->childs[i] = createNode(x + offX, z + offZ, w / 2, vertices, pitch, h, indices);
                }
            }
            return node;
        }
        log_info(format_string("created node with %d vertices, pos = %.2f, %.2f, %.2f, %.2f, %.2f", 
                                triangles.size(),
                                x,
                                z,
                                w,
                                node->max_h,
                                node->min_h));
        node->index_start = (uint32)indices.size();
        node->index_count = (uint32)triangles.size();
        indices.insert(indices.end(), triangles.begin(), triangles.end());

        return node;
    }
    
    uint32 GridTerrianLightening::triangleCount(float x, float z, float w, VertexUVNormal* vertices, uint32 pitch, uint32 h, std::vector<uint32>& triangles, float& maxh, float &minh) {
        uint32 count = 0;
        float r = w/2;
        float _min_h = FLT_MAX, _max_h = FLT_MIN;
        for(uint32 i=0; i<h; ++i) {
            uint32 j = 0;
            while(j < pitch) {
                uint32 idx = i * pitch + j;
                VertexUVNormal& vtx1 = vertices[idx];
                VertexUVNormal& vtx2 = vertices[idx + 1];
                VertexUVNormal& vtx3 = vertices[idx + 2];

                float minX = MIST_MIN(vtx1.position[0], MIST_MIN(vtx2.position[0], vtx3.position[0]));
                if(minX > x + r)
                    break;
                float minZ = MIST_MIN(vtx1.position[2], MIST_MIN(vtx2.position[2], vtx3.position[2]));
                if(minZ > z + r) 
                    break;
                
                j += 3;

                float maxX = MIST_MAX(vtx1.position[0], MIST_MAX(vtx2.position[0], vtx3.position[0]));
                if(maxX < x - r)
                    continue;
                float maxZ = MIST_MIN(vtx1.position[2], MIST_MIN(vtx2.position[2], vtx3.position[2]));
                if(maxZ < z - r) 
                    continue;

                float maxY = MIST_MAX(vtx1.position[1], MIST_MAX(vtx2.position[1], vtx3.position[1]));
                float minY = MIST_MIN(vtx1.position[1], MIST_MIN(vtx2.position[1], vtx3.position[1]));
                if(_max_h < maxY) _max_h = maxY;
                if(_min_h > minY) _min_h = minY;

                count += 1;
                triangles.push_back(idx);
                triangles.push_back(idx+1);
                triangles.push_back(idx+2);

            }
        }
        maxh = _max_h;
        minh = _min_h;
        return count;
    }

    void GridTerrianLightening::renderNode(const EffectTechniquePtr& technique, Node* node, GraphicDevice& device, const Frustum& frustum) {
        if(node->index_count &&
            frustum.isBoxVisible(Box(Vector3(node->x - node->width / 2,
                                             mPosition[1] - node->min_h,
                                             node->z - node->width / 2),
                                     Vector3(node->x + node->width / 2,
                                             mPosition[1] + node->max_h,
                                             node->z + node->width / 2))) != Frustum::No) {
            mRenderBuffer->setIndexStartIndex(node->index_start);
            mRenderBuffer->setIndexCount(node->index_count);

            device.renderBuffer(technique, mRenderBuffer);
            mDrawCount += node->index_count;
        }
        for(uint32 i=0; i<4; ++i) {
            if(node->childs[i]) {
                renderNode(technique, node->childs[i], device, frustum);
            }
        }
    }

    uint32 GridTerrianLightening::getDrawCount() const {
        return mDrawCount;
    }

    void GridTerrianLightening::render(const EffectTechniquePtr& technique) {
        if(!mRenderBuffer)
            return;
        GraphicDevice& gd = Context::Instance().getGraphicFactory().getGraphicDevice();

        CameraPtr cam = gd.getCurrFrameBuffer()->getViewport().camera;
     
        mDrawCount = 0;
        renderNode(technique, mRoot, gd, cam->getViewFrustum());
    }

}