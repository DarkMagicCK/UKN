#include "UKN/Skybox.h"
#include "UKN/Context.h"

#include "UKN/GraphicFactory.h"
#include "UKN/GraphicDevice.h"

#include "UKN/RenderBuffer.h"
#include "UKN/CgHelper.h"
#include "UKN/FrameBuffer.h"
#include "UKN/Camera.h"
#include "UKN/2DHelper.h"

namespace ukn {

    Skybox::Skybox() {

    }

    Skybox::~Skybox() {

    }

    bool Skybox::load(const ResourcePtr& resource) {
        GraphicFactory& factory = Context::Instance().getGraphicFactory();
        mDiffuseTex = factory.load2DTexture(resource, false);
        if(!mDiffuseTex) {
            log_error(L"Skybox::load: error loading texture");
            return false;
        }

        return this->buildVertices(factory);
    }

    void Skybox::render() {
        if(mRenderBuffer) {
            GraphicDevice& gd = Context::Instance().getGraphicFactory().getGraphicDevice();
            gd.renderBuffer(mRenderBuffer);

        } else {
            log_error(L"Skybox::render: invalid render buffer");
        }
    }

    bool Skybox::buildVertices(const GraphicFactory& factory) {
        VertexUVNormal vertices[6* 6];

        float half4 = 0.25f;
        float half2 = 0.5f;
        float half43 = 0.75f;
        float half3 = 1.0f / 3;
        float half32 = 2.0f / 3;

        float d = 1;
        
        float3 v[8] = {
            
            float3(d, -d, -d), // 0
            float3(-d, -d, -d), // 1
            float3(-d, d, -d), // 2
            float3(d, d, -d), // 3
            
            float3(d, -d, d), // 4
            float3(-d, -d, d), // 5
            float3(-d, d, d), // 6
            float3(d, d, d) // 7

        };

        uint32 idx = 0;
        
        // front, 0123
        vertices[idx++].position = v[0]; vertices[idx++].position = v[1]; vertices[idx++].position = v[2];
        vertices[idx++].position = v[2]; vertices[idx++].position = v[3]; vertices[idx++].position = v[0];

        // left, 1265
        vertices[idx++].position = v[1]; vertices[idx++].position = v[2]; vertices[idx++].position = v[6];
        vertices[idx++].position = v[6]; vertices[idx++].position = v[5]; vertices[idx++].position = v[1];

        // back, 4567
        vertices[idx++].position = v[4]; vertices[idx++].position = v[5]; vertices[idx++].position = v[6];
        vertices[idx++].position = v[6]; vertices[idx++].position = v[7]; vertices[idx++].position = v[4];

        // right, 0374
        vertices[idx++].position = v[0]; vertices[idx++].position = v[3]; vertices[idx++].position = v[7];
        vertices[idx++].position = v[7]; vertices[idx++].position = v[4]; vertices[idx++].position = v[0];

        // top, 2376
        vertices[idx++].position = v[2]; vertices[idx++].position = v[3]; vertices[idx++].position = v[7];
        vertices[idx++].position = v[7]; vertices[idx++].position = v[6]; vertices[idx++].position = v[2];

        // bottom, 0154
        vertices[idx++].position = v[0]; vertices[idx++].position = v[1]; vertices[idx++].position = v[5];
        vertices[idx++].position = v[5]; vertices[idx++].position = v[4]; vertices[idx++].position = v[0];

        idx = 0;

        vertices[idx++].uv.set(half2, half32); vertices[idx++].uv.set(half4, half32); vertices[idx++].uv.set(half4, half3);
        vertices[idx++].uv.set(half4, half3); vertices[idx++].uv.set(half2, half3); vertices[idx++].uv.set(half2, half32);
        
        vertices[idx++].uv.set(half4, half32); vertices[idx++].uv.set(half4, half3); vertices[idx++].uv.set(0, half3);
        vertices[idx++].uv.set(0, half3); vertices[idx++].uv.set(0, half32); vertices[idx++].uv.set(half4, half32);
        
        vertices[idx++].uv.set(half43, half32); vertices[idx++].uv.set(1, half32); vertices[idx++].uv.set(1, half3);
        vertices[idx++].uv.set(1, half3); vertices[idx++].uv.set(half43, half3); vertices[idx++].uv.set(half43, half32);
        
        vertices[idx++].uv.set(half2, half32); vertices[idx++].uv.set(half2, half3); vertices[idx++].uv.set(half43, half3);
        vertices[idx++].uv.set(half43, half3); vertices[idx++].uv.set(half43, half32); vertices[idx++].uv.set(half2, half32);
       
        vertices[idx++].uv.set(half4, half3); vertices[idx++].uv.set(half2, half3); vertices[idx++].uv.set(half2, 0);
        vertices[idx++].uv.set(half2, 0); vertices[idx++].uv.set(half4, 0); vertices[idx++].uv.set(half4, half3);
       
        vertices[idx++].uv.set(half2, half32); vertices[idx++].uv.set(half4, half32); vertices[idx++].uv.set(half4, 1);
        vertices[idx++].uv.set(half4, 1); vertices[idx++].uv.set(half2, 1); vertices[idx++].uv.set(half2, half32);
         
        mRenderBuffer = factory.createRenderBuffer();
        mVertexBuffer = factory.createVertexBuffer(GraphicBuffer::None,
                                                   GraphicBuffer::Static,
                                                   6 * 6,
                                                   vertices,
                                                   VertexUVNormal::Format());
        if(mVertexBuffer) {
            mRenderBuffer->bindVertexStream(mVertexBuffer, VertexUVNormal::Format());

        }
        return mRenderBuffer && mVertexBuffer;
    }

    
    const UknString& Skybox::getName() const {
        static UknString name(L"SkyBox");
        return name;
    }
        
    Box Skybox::getBound() const {
        return Box();
    }

    RenderBufferPtr Skybox::getRenderBuffer() const {
        return mRenderBuffer;
    }

}