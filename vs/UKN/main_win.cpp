#include "UKN/App.h"
#include "UKN/Context.h"
#include "UKN/Window.h"
#include "UKN/Texture.h"
#include "UKN/GraphicDevice.h"
#include "UKN/GraphicBuffer.h"
#include "UKN/GraphicFactory.h"
#include "UKN/FrameBuffer.h"
#include "UKN/Texture.h"
#include "UKN/SpriteBatch.h"
#include "UKN/RenderBuffer.h"
#include "UKN/Font.h"
#include "UKN/Asset.h"
#include "UKN/Skeletal.h"
#include "UKN/Input.h"
#include "UKN/Shader.h"
#include "UKN/Model.h"

#include "UKN/tmx/TMXTiledMap.h"

#include "mist/SysUtil.h"
#include "mist/TimeUtil.h"

#include <vector>
#include <map>

#include "mist/Query.h"

#include <numeric>

#ifndef MIST_OS_WINDOWS
int main (int argc, const char * argv[])
{
#else
#pragma comment(linker, "/NODEFAULTLIB:libcmt.lib")

#pragma comment(lib, "mist.lib")
#pragma comment(lib, "ukn_dll.lib")

int CALLBACK WinMain(
  __in  HINSTANCE hInstance,
  __in  HINSTANCE hPrevInstance,
  __in  LPSTR lpCmdLine,
  __in  int nCmdShow
) {
#endif

    ukn::RenderBufferPtr renderBuffer;
    ukn::TexturePtr texture;
    ukn::Matrix4 worldMat;
    ukn::EffectPtr effect;

     ukn::AppLauncher(L"Windows Test")
        .create(
                ukn::ContextCfg::Default()
                  .width(800)
                  .height(600)
                  .graphicFactoryName(L"D3D10Plugin.dll")
                  .fps(60)
               )
        .connectUpdate([](ukn::Window* ) {

        })
        .connectRender([&](ukn::Window*) {
            ukn::GraphicDevice& gd = ukn::Context::Instance().getGraphicFactory().getGraphicDevice();
            gd.clear(ukn::CM_Color | ukn::CM_Depth, mist::color::Black, 1.f, 0);

            ukn::Viewport& vp = gd.getCurrFrameBuffer()->getViewport();
            vp.camera->setViewParams(ukn::Vector3(0, 0, -10), ukn::Vector3(0, 0, 1));
            
            
            float diffuseColor[4] = { 1.f, 0.f, 0.f, 0.3f};
            float lightDirection[3] = {0.f, 0.f, 1.f};

            effect->setFloatVectorVariable("diffuseColor", diffuseColor);
            effect->setFloatVectorVariable("lightDirection", lightDirection);
            


            worldMat.rotate(0, 0.01 * ukn::d_pi, 0);
            gd.setWorldMatrix(worldMat);
            if(texture)
                gd.bindTexture(texture);
            if(renderBuffer)
                gd.renderBuffer(renderBuffer);
        })
        .connectInit([&](ukn::Window*) {
            ukn::GraphicFactory& gf = ukn::Context::Instance().getGraphicFactory();
            renderBuffer = gf.createRenderBuffer();

            ukn::VertexUVNormal vertices[6];
            vertices[0].x = -1; vertices[0].y = -1; vertices[0].z = 0.0f;
            vertices[1].x = 0.f; vertices[1].y = 1; vertices[1].z = 0.0f;
            vertices[2].x = 1; vertices[2].y = -1; vertices[2].z = 0.0f;
            vertices[0].setNormal(0.f, 0.f, -1.f);
            vertices[1].setNormal(0.f, 0.f, -1.f);
            vertices[2].setNormal(0.f, 0.f, -1.f);

            vertices[0].u = 0.0f; vertices[0].v = 0.0f;
            vertices[1].u = 0.5f; vertices[1].v = 0.5f;
            vertices[2].u = 1.0f; vertices[2].v = 1.0f;

            ukn::SharedPtr<ukn::Array<ukn::VertexUVNormal> > sphereVertices =
                    ukn::ModelLoader::BuildFromSphere(mist::Sphere(ukn::Vector3(0, 0, 0), 5.5), 20);

            
            ukn::GraphicBufferPtr vertexBuffer = gf.createVertexBuffer(ukn::GraphicBuffer::WriteOnly,
                                                                       ukn::GraphicBuffer::Dynamic,
                                                                       sphereVertices->size(),
                                                                       0,
                                                                       ukn::VertexUVNormal::Format());

            if(vertexBuffer) {
                renderBuffer->bindVertexStream(vertexBuffer, ukn::VertexUVNormal::Format());
                renderBuffer->setRenderMode(ukn::RM_Triangle);
            }
            effect = gf.loadEffect(ukn::ResourceLoader::Instance().loadResource(L"color.fx"));
            renderBuffer->setEffect(effect);

            ukn::VertexUVNormal* pptest = (ukn::VertexUVNormal*)renderBuffer->getVertexStream()->map();
            if(pptest) {
                for(int i=0; i<sphereVertices->size();++i) {
                   pptest[i] = sphereVertices->operator[](i); 

                }
            }
            renderBuffer->getVertexStream()->unmap();

            texture = gf.load2DTexture(ukn::ResourceLoader::Instance().loadResource(L"test.png"));
            if(!texture) {
                mist::MessageBox::Show(L"error loading texture", L"Error", mist::MBO_OK);
            }
        })
        .run();
    
    return 0;
}

