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
        mWidth(0), mHeight(0) {

    }

    GridTerrian::~GridTerrian() {

    }

    bool GridTerrian::build(float y, const float2& size, float grid_size) {
        this->mWidth = size[0]; this->mHeight = size[1];

        uint32 vertexSize = this->mWidth * this->mHeight;
        uint32 indexSize = (this->mWidth - 1) * (this->mHeight - 1) * 8;
        VertexFormat* vertices = mist_malloc_t(VertexFormat, vertexSize);
        uint32* indices = mist_malloc_t(uint32, indexSize);
        uint32 index = 0;
        for(uint32 z = 0; z < this->mHeight; ++z) {
            for(uint32 x = 0; x < this->mWidth; ++x) {
                uint32 current = z * this->mWidth + x;

                float h = mist::PerlinNoise::Gen((float)x / mWidth * 10, 
                                                 (float)z / mHeight * 10, 
                                                 0);
                vertices[current].position = float3(x * grid_size, 
                                                    y * grid_size + h * 2, 
                                                    z * grid_size);
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
                effect->setVertexFormat(ukn::Vertex2D::Format());

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
}