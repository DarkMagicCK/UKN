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
#include "UKN/RenderTarget.h"
#include "UKN/CameraController.h"
#include "UKN/Skybox.h"
#include "UKN/Terrian.h"
#include "UKN/GraphicContext.h"
#include "UKN/RasterizerStateObject.h"
#include "UKN/BlendStateObject.h"
#include "UKN/SamplerStateObject.h"
#include "UKN/Deferred.h"
#include "UKN/Light.h"
#include "UKN/LightManager.h"

#include "UKN/tmx/TMXTiledMap.h"

#include "mist/SysUtil.h"
#include "mist/TimeUtil.h"
#include "mist/RandomUtil.h"
#include "mist/Convert.h"
#include "mist/TimeUtil.h"
#include "mist/Query.h"

#include <vector>
#include <map>


#include <numeric>
#ifndef MIST_OS_WINDOWS

#include "../Plugins/gl/GLGraphicFactory.h"

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
  __in  int nCmdSho) {
#endif

    ukn::RenderBufferPtr renderBuffer;
    ukn::TexturePtr texture;
    ukn::TexturePtr texture2;
    ukn::Matrix4 worldMat;
    ukn::CameraController* camController;
    ukn::FontPtr font;
    ukn::SkyboxPtr skybox;
    ukn::TerrianPtr terrian;
    ukn::RenderBufferPtr dragonBuffer;
    ukn::LightManagerPtr lights;

    ukn::DeferredRendererPtr deferredRenderer;
    
#ifndef MIST_OS_WINDOWS
    ukn::GraphicFactoryPtr factory;
    ukn::CreateGraphicFactory(factory);
    ukn::Context::Instance().registerGraphicFactory(factory);
#endif

    ukn::uint32 count;
    float r = 0;
     ukn::AppLauncher(L"Windows Test")
        .create(
                ukn::ContextCfg::Default()
                  .width(800)
                  .height(600)
                  .sampleCount(1)
                  .showMouse(true)
                  .isFullScreen(false)
                  .graphicFactoryName(L"D3D11Plugin.dll")
                  .fps(60)
               )
        .connectUpdate([](ukn::Window* ) {

        })
        .connectKey([&](ukn::Window* window, ukn::input::KeyEventArgs& e) {
            if(e.key == ukn::input::Escape)
                ukn::Context::Instance().getApp().terminate();

            if(
                e.state == ukn::input::Release) {
                if(e.key == ukn::input::F1) {
                    if(camController->getAttachedCamera()) {
                        camController->detachCamera();
                    } else {
                        ukn::Viewport& vp = ukn::Context::Instance()
                                                .getGraphicFactory()
                                                .getGraphicDevice()
                                                .getCurrFrameBuffer()->getViewport();
                        camController->attachCamera(vp.camera);
                    }
                }
            }
        })
        .connectRender([&](ukn::Window* wnd) {
            ukn::GraphicDevice& gd = ukn::Context::Instance().getGraphicFactory().getGraphicDevice();
            
            deferredRenderer->begin(lights);
            deferredRenderer->renderBuffer(dragonBuffer, texture2, ukn::Matrix4::ScaleMat(100, 100, 100));
            if(terrian) {
                deferredRenderer->startRenderBuffer(ukn::Matrix4(), texture);
                terrian->render();
                deferredRenderer->endRenderBuffer();
            }
            deferredRenderer->end();

            gd.clear(ukn::CM_Color | ukn::CM_Depth, mist::color::Black, 1.f, 0);
           
            ukn::SpriteBatch& sb = ukn::SpriteBatch::DefaultObject();
            sb.begin(ukn::SBB_None, ukn::SBS_Deffered, ukn::Matrix4());

            const ukn::CompositeRenderTargetPtr& gbuffer = deferredRenderer->getGBufferRT();

            sb.draw(gbuffer->getTargetTexture(ukn::ATT_Color0), 
                    ukn::Rectangle(0, 0, wnd->width() / 2, wnd->height() / 2, true));
            sb.draw(gbuffer->getTargetTexture(ukn::ATT_Color1), 
                    ukn::Rectangle(0, wnd->height() / 2, wnd->width() / 2, wnd->height() / 2, true));
            sb.draw(deferredRenderer->getLightMapRT()->getTargetTexture(ukn::ATT_Color0), 
                    ukn::Rectangle(wnd->width() / 2, wnd->height() / 2, wnd->width() / 2, wnd->height() / 2, true));
            sb.draw(deferredRenderer->getCompositeRT()->getTargetTexture(ukn::ATT_Color0), 
                    ukn::Rectangle(wnd->width() / 2, 0, wnd->width() / 2, wnd->height() / 2, true));
            sb.end();

            if(font) {
                font->begin();
                font->draw(gd.description().c_str(), 0, 20, ukn::FA_Left, ukn::color::Skyblue);
                font->draw((ukn::FormatString(L"FPS: {0}"), mist::FrameCounter::Instance().getCurrentFps()),
                            0, 
                            0, 
                            ukn::FA_Left,
                            ukn::color::Skyblue);

                font->end();
            }
        })
        .connectInit([&](ukn::Window*) {
            ukn::GraphicFactory& gf = ukn::Context::Instance().getGraphicFactory();
     
            renderBuffer = ukn::ModelLoader::BuildFromSphere(mist::Sphere(ukn::Vector3(0, 0, 0), 1.0), 10);

            texture = gf.load2DTexture(mist::ResourceLoader::Instance().loadResource(L"grass.dds"));
           
            ukn::uint32 c = 0xFFFFFFFF;
            texture2 = gf.create2DTexture(1, 1, 0, ukn::EF_RGBA8, (mist::uint8*)&c);

            camController = new ukn::FpsCameraController();
            ukn::Viewport& vp = gf.getGraphicDevice().getCurrFrameBuffer()->getViewport();
            vp.camera->setViewParams(ukn::Vector3(0, 5, -20), ukn::Vector3(0, 0, 1));

            camController->attachCamera(vp.camera);

            font = ukn::Font::Create(L"Consola.ttf", 20);

            skybox = new ukn::Skybox();
            if(!skybox->load(mist::ResourceLoader::Instance().loadResource(L"skyboxsun25degtest.png"))) {
                mist::log_error(L"unable to load skybox");
            }

            ukn::GridTerrianLightening* t = new ukn::GridTerrianLightening();
            (*t).position(ukn::float3(-50, 0, -50))
                .noise(10)
                .noiseWeight(5)
                .size(ukn::float2(100, 100));
            if(t->build()) {
                t->texture(gf.load2DTexture(ukn::ResourceLoader::Instance().loadResource(L"dirt01.dds")));

                terrian = t;
                
            } else {
                ukn::log_error(L"unable to build terrian");
            }

          
            ukn::ModelLoader::ModelDataPtr dragonModel = ukn::ModelLoader::LoadFromPly(
                ukn::ResourceLoader::Instance().loadResource(L"dragon_recon/dragon_vrip_res4.ply"));
          
            if(dragonModel) {
                ukn::ModelLoader::ModelData* pdragonModel = dragonModel.get();
                ukn::VertexUVNormal* vertices = mist_malloc_t(ukn::VertexUVNormal, pdragonModel->indices.size());
                for(size_t i=0; i<dragonModel->indices.size(); ++i) {
                    vertices[i].position = pdragonModel->position[pdragonModel->indices[i]];
                    if(dragonModel->uv.size() > 0)
                        vertices[i].uv = pdragonModel->uv[pdragonModel->indices[i]];
                    else
                        vertices[i].uv = ukn::float2(0, 0);
                }
                // calculate normals
                ukn::uint32 index = 0;
                for(size_t i=0; i<dragonModel->indices.size() / 3; ++i) {
                    ukn::VertexUVNormal& p1 = vertices[index];
                    ukn::VertexUVNormal& p2 = vertices[index+1];
                    ukn::VertexUVNormal& p3 = vertices[index+2];

                    ukn::Vector3 u = p2.position - p1.position;
                    ukn::Vector3 v = p3.position - p1.position;
                    ukn::float3 normal = u.cross(v);

                    p1.normal = p2.normal = p3.normal = normal;
                    index += 3;
                }
                count = dragonModel->indices.size();
                ukn::GraphicBufferPtr vertexBuffer = gf.createVertexBuffer(ukn::GraphicBuffer::None,
                                                                           ukn::GraphicBuffer::Static,
                                                                           dragonModel->indices.size(),
                                                                           vertices,
                                                                           ukn::VertexUVNormal::Format());
                ukn::GraphicBufferPtr indexBuffer = gf.createIndexBuffer(ukn::GraphicBuffer::None,
                                                                         ukn::GraphicBuffer::Static,
                                                                         dragonModel->indices.size(),
                                                                         &dragonModel->indices[0]);
                dragonBuffer = gf.createRenderBuffer();
                dragonBuffer->bindVertexStream(vertexBuffer, ukn::VertexUVNormal::Format());
                dragonBuffer->bindIndexStream(indexBuffer);
                dragonBuffer->setRenderMode(ukn::RM_Triangle);

                mist::mist_free(vertices);
            }

            deferredRenderer = new ukn::DeferredRenderer();

            lights = new ukn::LightManager();
            lights->addLight(new ukn::DirectionalLight(ukn::float3(0, -1, 0),
                                                       ukn::float4(1, 1, 1, 1),
                                                       1));
        })
        .run();
    
    return 0;
}

