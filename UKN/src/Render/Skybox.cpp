#include "UKN/Skybox.h"
#include "UKN/Context.h"

#include "UKN/GraphicFactory.h"
#include "UKN/GraphicDevice.h"

#include "UKN/RenderBuffer.h"
#include "UKN/CgHelper.h"
#include "UKN/FrameBuffer.h"
#include "UKN/Camera.h"

namespace ukn {

    Skybox::Skybox() {

    }

    Skybox::~Skybox() {

    }

    bool Skybox::load(const ResourcePtr& resource) {
        GraphicFactory& factory = Context::Instance().getGraphicFactory();
        mTexture = factory.load2DTexture(resource, false);
        if(!mTexture) {
            log_error(L"Skybox::load: error loading texture");
            return false;
        }

        return this->buildVertices(factory);
    }

    void Skybox::render() {
        if(mRenderBuffer) {
            GraphicDevice& gd = Context::Instance().getGraphicFactory().getGraphicDevice();

            // reset world matrix
            FrameBufferPtr fb = gd.getCurrFrameBuffer();
            CameraPtr cam = fb->getViewport().camera;
            
            Matrix4 mat = Matrix4::ScaleMat(100, 100, 100);
            if(cam) {
                Vector3 eye = cam->getEyePos();
               // mat.translate(eye.x(), eye.y(), eye.z());
            }
            gd.setWorldMatrix(mat);
            gd.bindTexture(mTexture);
            gd.renderBuffer(mRenderBuffer);
            gd.setWorldMatrix(Matrix4());

        } else {
            log_error(L"Skybox::render: invalid render buffer");
        }
    }

    bool Skybox::buildVertices(const GraphicFactory& factory) {
        Vertex2D vertices[6* 6];

        double half4 = 0.25;
        double half2 = 0.5;
        double half43 = 0.75;
        double half3 = 1024.0 / 3072.0;
        double half32 = 2048.0 / 3072.0;

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
        vertices[idx++].xyz = v[0]; vertices[idx++].xyz = v[1]; vertices[idx++].xyz = v[2];
        vertices[idx++].xyz = v[2]; vertices[idx++].xyz = v[3]; vertices[idx++].xyz = v[0];

        // left, 1265
        vertices[idx++].xyz = v[1]; vertices[idx++].xyz = v[2]; vertices[idx++].xyz = v[6];
        vertices[idx++].xyz = v[6]; vertices[idx++].xyz = v[5]; vertices[idx++].xyz = v[1];

        // back, 4567
        vertices[idx++].xyz = v[4]; vertices[idx++].xyz = v[5]; vertices[idx++].xyz = v[6];
        vertices[idx++].xyz = v[6]; vertices[idx++].xyz = v[7]; vertices[idx++].xyz = v[4];

        // right, 0374
        vertices[idx++].xyz = v[0]; vertices[idx++].xyz = v[3]; vertices[idx++].xyz = v[7];
        vertices[idx++].xyz = v[7]; vertices[idx++].xyz = v[4]; vertices[idx++].xyz = v[0];

        // top, 2376
        vertices[idx++].xyz = v[2]; vertices[idx++].xyz = v[3]; vertices[idx++].xyz = v[7];
        vertices[idx++].xyz = v[7]; vertices[idx++].xyz = v[6]; vertices[idx++].xyz = v[2];

        // bottom, 0154
        vertices[idx++].xyz = v[0]; vertices[idx++].xyz = v[1]; vertices[idx++].xyz = v[5];
        vertices[idx++].xyz = v[5]; vertices[idx++].xyz = v[4]; vertices[idx++].xyz = v[0];

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
                                                   Vertex2D::Format());
        if(mVertexBuffer) {
            mRenderBuffer->bindVertexStream(mVertexBuffer, Vertex2D::Format());
            mRenderBuffer->setEffect(ukn::CreateCgEffet2D());
        }
        return mRenderBuffer && mVertexBuffer;
    }

}