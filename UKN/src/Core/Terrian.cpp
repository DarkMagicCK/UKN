#include "UKN/Terrian.h"
#include "UKN/RenderBuffer.h"
#include "UKN/GraphicBuffer.h"
#include "UKN/Context.h"
#include "UKN/GraphicFactory.h"
#include "UKN/GraphicDevice.h"
#include "UKN/Shader.h"

#include "mist/MemoryUtil.h"

namespace ukn {

    static const char* _color_vert_program = "\
                                             const uniform float4x4 worldMatrix;         \
                                             const uniform float4x4 viewMatrix;          \
                                             const uniform float4x4 projectionMatrix;    \
                                             struct VertexOutputType {\
                                                float4 position: POSITION;\
                                                float4 color: COLOR;\
                                             };\
                                             VertexOutputType VertexProgram(in float3 position: ATTR0: POSITION,  \
                                                                            in float4 color: ATTR3: COLOR) {      \
                                             VertexOutputType output;\
                                             output.position = float4(position, 1);\
                                             output.position = mul(output.position, worldMatrix);\
                                             output.position = mul(output.position, viewMatrix);\
                                             output.position = mul(output.position, projectionMatrix);\
                                             output.color = color;\
                                             return output;\
                                             }\0";

    static const char* _color_frag_program = "\
                                             struct VertexOutputType {\
                                             float4 position: POSITION;\
                                             float4 color: COLOR;\
                                             };\
                                             float4 FragmentProgram(VertexOutputType input): COLOR {\
                                             return input.color;\
                                             }\0";

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
    mY(0),
    mNoise(10), mNoiseWeight(5),
    mGridSize(1) {

    }

    GridTerrian::~GridTerrian() {

    }
    
    GridTerrian& GridTerrian::y(float y) {
        this->mY = y;
        return *this;
    }

    GridTerrian& GridTerrian::noise(float noise) {
        this->mNoise = noise;
        return *this;
    }

    GridTerrian& GridTerrian::noiseWeight(float noiseWeight) {
        this->mNoiseWeight = mNoiseWeight;
        return *this;
    }

    GridTerrian& GridTerrian::size(const float2& size) {
        this->mWidth = size[0]; this->mHeight = size[1];
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
                vertices[current].position = float3(x * mGridSize, 
                                                    mY * mGridSize + h * mNoiseWeight, 
                                                    z * mGridSize);
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
                effect->setFragmentShader(effect->createShader(
                    ukn::Resource::MakeResourcePtr(new ukn::MemoryStream((const uint8*)_color_frag_program, 
                                                                         strlen(_color_frag_program)), 
                                                                         L""),
                                                   ukn::ShaderDesc(ST_FragmentShader,
                                                   "FragmentProgram")));
                effect->setVertexShader(effect->createShader(
                    ukn::Resource::MakeResourcePtr(new ukn::MemoryStream((const uint8*)_color_vert_program, 
                                                                         strlen(_color_vert_program)), 
                                                                         L""),
                                                   ukn::ShaderDesc(ST_VertexShader,
                                                   "VertexProgram")));
                effect->setVertexFormat(ukn::GridTerrian::VertexFormat::Format());

                mRenderBuffer->setEffect(effect);
                mRenderBuffer->setRenderMode(RM_Line);
            } else 
                error = true;
        } else
            error = true;

        mist_free(vertices);
        mist_free(indices);

        return !error;
    }

    void GridTerrian::render() {
        if(!mRenderBuffer)
            return;
        GraphicDevice& gd = Context::Instance().getGraphicFactory().getGraphicDevice();

        gd.bindTexture(TexturePtr());
        gd.renderBuffer(mRenderBuffer);
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

    static const char* _light_vert_program = "\
                                             const uniform float4x4 worldMatrix;         \
                                             const uniform float4x4 viewMatrix;          \
                                             const uniform float4x4 projectionMatrix;    \
                                             struct VertexOutputType {\
                                                float4 position: POSITION;\
                                                float3 normal: TEXCOORD1;\
                                                float2 texCoord: TEXCOORD0; \
                                             };\
                                             VertexOutputType VertexProgram(in float3 position: ATTR0: POSITION,  \
                                                                            in float3 normal: ATTR2: NORMAL, \
                                                                            in float2 texCoord: ATTR8: TEXCOORD0) {      \
                                             VertexOutputType output;\
                                             output.position = float4(position, 1);\
                                             output.position = mul(output.position, worldMatrix);\
                                             output.position = mul(output.position, viewMatrix);\
                                             output.position = mul(output.position, projectionMatrix);\
                                             output.normal = normalize(mul(normal, (float3x3)worldMatrix));\
                                             output.texCoord = texCoord; \
                                             return output;\
                                             }\0";

    static const char* _light_frag_program = "\
                                             const uniform float4 diffuseColor; \
                                             const uniform float4 ambientColor; \
                                             const uniform float4 lightDirection; \
                                             struct VertexOutputType {\
                                             float4 position: POSITION; \
                                             float3 normal: TEXCOORD1; \
                                             float2 texCoord: TEXCOORD0; \
                                             };\
                                             float4 FragmentProgram(VertexOutputType input, \
                                                                    uniform sampler2D tex: TEX0): COLOR {\
                                             float3 nLightDir; \
                                             float lightIntensity; \
                                             float4 color; \
                                             \
                                             color = ambientColor; \
                                             nLightDir = -lightDirection; \
                                             lightIntensity = saturate(dot(input.normal, nLightDir)); \
                                             if(lightIntensity > 0.0f) { \
                                                color += (diffuseColor * lightIntensity); \
                                             } \
                                             return saturate(color) * tex2D(tex, input.texCoord); \
                                             }\0";

    GridTerrianLightening::GridTerrianLightening():
    mTextureRepeat(10) {

    }

    GridTerrianLightening::~GridTerrianLightening() {

    }

    vertex_elements_type GridTerrianLightening::VertexFormat::Format() {
        static vertex_elements_type _format = VertexElementsBuilder()
            .add(VertexElement(VU_Position, EF_Float3, 0))
            .add(VertexElement(VU_Normal, EF_Float3, 0))
            .add(VertexElement(VU_UV, EF_Float2, 0))
            .to_vector();
        return _format;
    }

    GridTerrianLightening& GridTerrianLightening::texture(const TexturePtr& tex) {
        this->mTexture = tex;
        return *this;
    }

    GridTerrianLightening& GridTerrianLightening::textureRepeat(uint32 texRepeat) {
        this->mTextureRepeat = texRepeat;
        return *this;
    }

    bool GridTerrianLightening::build() {
        uint32 vertexSize = this->mWidth * this->mHeight;
        uint32 indexSize = (this->mWidth - 1) * (this->mHeight - 1) * 6;
        VertexFormat* vertices = mist_malloc_t(VertexFormat, vertexSize);
        uint32* indices = mist_malloc_t(uint32, indexSize);
        uint32 index = 0;

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

                float h = 0;
                if(mNoise > 1.f && mNoiseWeight != 0) {
                    h = mist::PerlinNoise::Gen((float)x / mWidth * mNoise, 
                                               (float)z / mHeight * mNoise, 
                                               0);
                }
                vertices[current].position = float3(x * mGridSize, 
                                                    mY * mGridSize + h * mNoiseWeight, 
                                                    z * mGridSize);
                
                vertices[current].uv = float2(tu, 
                                              tv);
               

                if(z < this->mHeight - 1 &&
                    x < this->mWidth - 1) {
                        uint32 r = z * this->mWidth + x + 1;
                        uint32 br = (z + 1) * this->mWidth + x + 1;
                        uint32 bl = (z + 1) * this->mWidth + x;

                        indices[index++] = current; indices[index++] = r;
                        indices[index++] = br; indices[index++] = br;
                        indices[index++] = bl; indices[index++] = current;
                }
                 
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
        
        // calculate normals
        index = 0;
        float3* normals = mist_malloc_t(float3, (this->mWidth - 1) * (this->mHeight - 1));
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
                sum /= c;

                vertices[z * mWidth + x].normal = sum.normalize();
            }
        }
        mist_free(normals);

        uint32 faceSize = (this->mWidth - 1) * (this->mHeight - 1) * 6;
        VertexFormat* faces = mist_malloc_t(VertexFormat, faceSize);
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

        GraphicFactory& gf = Context::Instance().getGraphicFactory();
        mRenderBuffer = gf.createRenderBuffer();
        mVertexBuffer = gf.createVertexBuffer(GraphicBuffer::None,
            GraphicBuffer::Static,
            faceSize,
            faces,
            VertexFormat::Format());
        mIndexBuffer = gf.createIndexBuffer(GraphicBuffer::None,
            GraphicBuffer::Static,
            indexSize,
            indices);

        bool error = false;
        if(mRenderBuffer && mIndexBuffer && mVertexBuffer) {
            mRenderBuffer->bindVertexStream(mVertexBuffer, VertexFormat::Format());
            mRenderBuffer->bindIndexStream(mIndexBuffer);
            mRenderBuffer->useIndexStream(false);

            EffectPtr effect = ukn::Context::Instance().getGraphicFactory().createEffect();
            if(effect) {
                effect->setFragmentShader(effect->createShader(
                    ukn::Resource::MakeResourcePtr(new ukn::MemoryStream((const uint8*)_light_frag_program, 
                                                                         strlen(_light_frag_program)), 
                                                                         L""),
                                                   ukn::ShaderDesc(ST_FragmentShader,
                                                   "FragmentProgram")));
                effect->setVertexShader(effect->createShader(
                    ukn::Resource::MakeResourcePtr(new ukn::MemoryStream((const uint8*)_light_vert_program, 
                                                                         strlen(_light_vert_program)), 
                                                                         L""),
                                                   ukn::ShaderDesc(ST_VertexShader,
                                                   "VertexProgram")));
                effect->setVertexFormat(ukn::GridTerrianLightening::VertexFormat::Format());

                ShaderPtr fragmentShader = effect->getFragmentShader();
                if(fragmentShader) {
                    fragmentShader->setFloatVectorVariable("ambientColor", float4(0.6f, 0.6f, 0.6f, 1.0f));
                    fragmentShader->setFloatVectorVariable("diffuseColor", float4(0.9f, 0.8f, 0.6f, 1.0f));
                    fragmentShader->setFloatVectorVariable("lightDirection", float4(0, 0, -0.5f, 1));
                }

                mRenderBuffer->setEffect(effect);
                mRenderBuffer->setRenderMode(RM_Triangle);
            } else 
                error = true;
        } else
            error = true;

        mist_free(vertices);
        mist_free(indices);

        return !error;
    }

    void GridTerrianLightening::render() {
        if(!mRenderBuffer)
            return;
        GraphicDevice& gd = Context::Instance().getGraphicFactory().getGraphicDevice();

        gd.bindTexture(mTexture);
        gd.renderBuffer(mRenderBuffer);
    }

}