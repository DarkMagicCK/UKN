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
#include "UKN/SceneObjectWrapper.h"
#include "UKN/SceneManager.h"
#include "UKN/SceneObject.h"

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
            
            deferredRenderer->renderScene();

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
     
            camController = new ukn::FpsCameraController();
            ukn::Viewport& vp = gf.getGraphicDevice().getCurrFrameBuffer()->getViewport();
            vp.camera->setViewParams(ukn::Vector3(0, 5, -10), ukn::Vector3(0, 0, 1));

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

            deferredRenderer = new ukn::DeferredRenderer();

            ukn::SceneManager& scene = ukn::Context::Instance().getSceneManager();
           
            ukn::ModelPtr dragonModel = ukn::AssetManager::Instance().load<ukn::Model>(L"dragon_recon/dragon_vrip_res4.ply");
            if(dragonModel) {
                ukn::SceneObjectPtr dragonObject = ukn::MakeSharedPtr<ukn::SceneObject>(dragonModel, 0);
                dragonObject->setModelMatrix(ukn::Matrix4::ScaleMat(50, 50, 50));
                scene.addSceneObject(dragonObject);

                ukn::SceneObjectPtr terrianObject = ukn::MakeSharedPtr<ukn::SceneObject>(terrian, 0);
                scene.addSceneObject(terrianObject);
            }
            scene.addLight(ukn::MakeSharedPtr<ukn::DirectionalLight>(ukn::Vector3::Down(),
                                                                     ukn::float4(1, 1, 1, 1),
                                                                     1.0));
        })
        .run();
    
    return 0;
}

