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
#include "UKN/SSAO.h"
#include "UKN/Fog.h"
#include "UKN/tmx/TMXTiledMap.h"

#include "mist/SysUtil.h"
#include "mist/TimeUtil.h"
#include "mist/RandomUtil.h"
#include "mist/Convert.h"
#include "mist/TimeUtil.h"
#include "mist/Query.h"
#include "mist/Profiler.h"

#include <vector>
#include <map>
#include <numeric>

#include "../../Tests/voxel/Game.h"

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

     if(0) {
          ukn::voxel::Game game;
          game.start();
          return 0;
      }
      

    ukn::Matrix4 worldMat;
    ukn::CameraController* camController;
    ukn::FontPtr font;
    ukn::SkyboxPtr skybox;
    ukn::TerrianPtr terrian;
    ukn::RenderBufferPtr dragonBuffer;
    ukn::LightSourcePtr spotLight;
    ukn::LightSourcePtr directionalLight;
    ukn::SSAO* ssao;
    ukn::Fog* fog;

    ukn::EffectPtr testEffect;
    ukn::EffectTechniquePtr displayDepthTechnique;

    ukn::DeferredRendererPtr deferredRenderer;

    ukn::PointLightPtr pointLights[400];
    float rs[400];
    ukn::uint32 pointLightCount = 0;

    enum Mode {
        LightMap,
        Scene,
        Depth,
        Color,
        SSAOAll,
        All,
    };

    Mode mode = Scene;
    
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
                  .width(1024)
                  .height(768)
                  .sampleCount(1)
                  .showMouse(true)
                  .isFullScreen(false)
                  .graphicFactoryName(L"D3D11Plugin.dll")
               )
        .connectUpdate([&](ukn::Window* ) {
            for(int i=0; i<pointLightCount; ++i) {
                pointLights[i]->setPosition(ukn::float3(
                                            sinf(r + rs[i]) * (i * 2 % 50) * 5,
                                            (i * 5) % 30,
                                            cosf(r + rs[i]) * (i * 2 % 50) * 5));
            }
            
            r += (float)ukn::d_pi / 180.f;
        })
        .connectKey([&](ukn::Window* window, ukn::input::KeyEventArgs& e) {
            if(e.key == ukn::input::Escape)
                ukn::Context::Instance().getApp().terminate();

            if(e.state == ukn::input::Release) {
                if(e.key == ukn::input::F1) {
                    if(camController->getAttachedCamera()) {
                        camController->detachCamera();
                    } else {
                        ukn::Viewport& vp = ukn::Context::Instance()
                                                .getGraphicFactory()
                                                .getGraphicDevice()
                                                .getScreenFrameBuffer()->getViewport();
                        camController->attachCamera(vp.camera);
                    }
                } else if(e.key == ukn::input::F2) {
                    directionalLight->setEnabled(!directionalLight->getEnabled());
                } else if(e.key == ukn::input::Num2) {
                    mode = LightMap;
                } else if(e.key == ukn::input::Num1) {
                    mode = Scene;
                } else if(e.key == ukn::input::Num3) {
                    mode = All;
                } else if(e.key == ukn::input::Num4) {
                    mode = SSAOAll;

                }  else if(e.key == ukn::input::Num5) {
                    mode = Depth;
                }  else if(e.key == ukn::input::Num6) {
                    mode = Color;
                } else if(e.key == ukn::input::Equals) {
                   
                    ukn::SceneManager& scene = ukn::Context::Instance().getSceneManager();
           
                    pointLights[pointLightCount]  = ukn::MakeSharedPtr<ukn::PointLight>(ukn::float3(pointLightCount + 30,
                                                                               0,
                                                                               0),
                                                                   20.f,
                                                                   ukn::float4(ukn::Random::RandomFloat(0, 1), 
                                                                               ukn::Random::RandomFloat(0, 1), 
                                                                               ukn::Random::RandomFloat(0, 1), 1),
                                                                   1.0f,
                                                                   false,
                                                                   256);
                     rs[pointLightCount] = ukn::Random::RandomFloat(0, ukn::d_pi * 2);
                     scene.addLight(pointLights[pointLightCount]);
                     pointLightCount ++;
                } else if(e.key == ukn::input::Num4) {
                    
                    ukn::SceneManager& scene = ukn::Context::Instance().getSceneManager();
           
                    pointLightCount --;
                    scene.removeLight(pointLights[pointLightCount]);
                }
            }

            
            if(e.key == ukn::input::U) {
                ssao->setSampleRadius(ssao->getSampleRadius() + 0.01);
            } else if(e.key == ukn::input::I) {
                ssao->setDistanceScale(ssao->getDistanceScale() + 0.01);
            }
            else if(e.key == ukn::input::J) {
                ssao->setSampleRadius(ssao->getSampleRadius() - 0.01);
            } else if(e.key == ukn::input::K) {
                ssao->setDistanceScale(ssao->getDistanceScale() - 0.01);
            }
            else if(e.key == ukn::input::O) {
                fog->setDensity(fog->getDensity() + 0.01);
            } else if(e.key == ukn::input::L) {
                fog->setDensity(fog->getDensity() - 0.01);
            }
        })
        .connectRender([&](ukn::Window* wnd) {
            ukn::GraphicDevice& gd = ukn::Context::Instance().getGraphicFactory().getGraphicDevice();
            
            ukn::SceneManager& scene = ukn::Context::Instance().getSceneManager();
            
            {
                scene.getLightManager()->makeShadowMaps(scene);
            }
            {
                deferredRenderer->renderScene(scene);
            }
            {
            }
            // to do with rendering shader

            gd.clear(ukn::CM_Color | ukn::CM_Depth, mist::color::Black, 1.f, 0);
           
            if(mode != Depth) {
            ukn::SpriteBatch& sb = ukn::SpriteBatch::DefaultObject();
            sb.begin(ukn::SBB_None, ukn::SBS_Deffered, ukn::Matrix4());

            const ukn::CompositeRenderTargetPtr& gbuffer = deferredRenderer->getGBufferRT();

            if(mode == SSAOAll) {
         //       sb.draw(deferredRenderer->getLightMapRT()->getTargetTexture(ukn::ATT_Color0), 
         //               ukn::Rectangle(0, wnd->height() / 2, wnd->width() / 2, wnd->height() / 2, true));
        //        sb.draw(deferredRenderer->getGBufferRT()->getTargetTexture(ukn::ATT_Color1), 
         //               ukn::Rectangle(wnd->width() / 2, wnd->height() / 2, wnd->width() / 2, wnd->height() / 2, true));
                sb.draw(ssao->getSSAOTarget()->getTexture(), 
                        ukn::Rectangle(0, 0, wnd->width(), wnd->height(), true));
            }  else if(mode == All) {
             //   sb.draw(ssao->getSSAOTarget()->getTexture(), 
              //          ukn::Rectangle(0, 0, wnd->width() / 2, wnd->height() / 2, true));
         //       sb.draw(deferredRenderer->getLightMapRT()->getTargetTexture(ukn::ATT_Color0), 
          //              ukn::Rectangle(0, 0, wnd->width(), wnd->height(), true));
                sb.draw(deferredRenderer->getGBufferRT()->getTargetTexture(ukn::ATT_Color1), 
                        ukn::Rectangle(0, 0, wnd->width(), wnd->height(), true));
         //       sb.draw(deferredRenderer->getCompositeRT()->getTargetTexture(ukn::ATT_Color0), 
         //               ukn::Rectangle(wnd->width() / 2, 0, wnd->width() / 2, wnd->height() / 2, true));
            } else if(mode == LightMap) {
                sb.draw(deferredRenderer->getLightMapRT()->getTargetTexture(ukn::ATT_Color0), 
                        ukn::Rectangle(0, 0, wnd->width() , wnd->height(), true));

            } else if(mode == Scene) {
                sb.draw(deferredRenderer->getFinalTexture(), 
                        ukn::Rectangle(0, 0, wnd->width() , wnd->height(), true));

            } else if(mode == Color) {
                sb.draw(deferredRenderer->getGBufferRT()->getTargetTexture(ukn::ATT_Color0), 
                        ukn::Rectangle(0, 0, wnd->width() , wnd->height(), true));

            }
            sb.end();
        }
            
            if(mode == Depth) {
                displayDepthTechnique->getPass(0)->getFragmentShader()->setTextureVariable("depthMap", 
                   deferredRenderer->getDepthTarget()->getTexture());
                
                ukn::SpriteBatch::DefaultObject().drawQuad(displayDepthTechnique, 
                    ukn::Vector2(-1, 1), 
                    ukn::Vector2(1, -1));

            }    
            if(font) {
                mist::ProfileData shadowMapProf = mist::Profiler::Instance().get(L"SHADOW_MAP");
                mist::ProfileData gbufferProf = mist::Profiler::Instance().get(L"DEFERRED_GBUFFER");
                mist::ProfileData lightMapPro = mist::Profiler::Instance().get(L"DEFERRED_LIGHTMAP");
            
             /*   font->begin();
                font->draw(gd.description().c_str(), 0, 20, ukn::FA_Left, ukn::color::Skyblue);
                font->draw((ukn::FormatString(L"FPS: {0}"), mist::FrameCounter::Instance().getCurrentFps()),
                            0, 
                            0, 
                            ukn::FA_Left,
                            ukn::color::Skyblue);

                font->draw((ukn::FormatString(L"Vertices Rendered: {0}\nSample Radius: {1}\nDistance Scale: {2}"), 
                                scene.numVerticesRendered(),
                                ssao->getSampleRadius(),
                                ssao->getDistanceScale()),
                            0, 
                            40, 
                            ukn::FA_Left,
                            ukn::color::Skyblue);

                font->end();*/
            }
        })
        .connectInit([&](ukn::Window*) {
            ukn::GraphicFactory& gf = ukn::Context::Instance().getGraphicFactory();
     
            camController = new ukn::FpsCameraController();
            ukn::Viewport& vp = gf.getGraphicDevice().getCurrFrameBuffer()->getViewport();
            vp.camera->setViewParams(ukn::Vector3(0, 5, 0), ukn::Vector3(0, 0, 1));

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
                .size(ukn::float2(100, 100))
                .grid_size(1);
            if(t->build()) {
                t->texture(gf.load2DTexture(ukn::ResourceLoader::Instance().loadResource(L"dirt01.dds")));

                terrian = t;
                
            } else {
                ukn::log_error(L"unable to build terrian");
            }

            deferredRenderer = new ukn::DeferredRenderer();
            deferredRenderer->addPostEffect(L"SSAO");
        //    deferredRenderer->addPostEffect(L"Fog");

            ssao = ukn::checked_cast<ukn::SSAO*>(deferredRenderer->getPostEffect(L"SSAO").get());
        //    fog = ukn::checked_cast<ukn::Fog*>(deferredRenderer->getPostEffect(L"Fog").get());

            ukn::SceneManager& scene = ukn::Context::Instance().getSceneManager();
           
            ukn::ModelPtr dragonModel = ukn::AssetManager::Instance().load<ukn::Model>(L"dragon_recon/dragon_vrip_res4.ply");
            if(dragonModel) {
                ukn::SceneObjectPtr dragonObject = ukn::MakeSharedPtr<ukn::SceneObject>(dragonModel, ukn::SOA_Cullable | ukn::SOA_Moveable);
                dragonObject->setModelMatrix(ukn::Matrix4::ScaleMat(30, 30, 30));
                scene.addSceneObject(dragonObject);
            }
            
            ukn::SceneObjectPtr skyboxObject = ukn::MakeSharedPtr<ukn::SceneObject>(skybox, ukn::SOA_Overlay);
            skyboxObject->setModelMatrix(ukn::Matrix4::ScaleMat(100, 100, 100));
                
            scene.addSceneObject(skyboxObject);

     //       ukn::SceneObjectPtr terrianObject = ukn::MakeSharedPtr<ukn::SceneObject>(terrian, ukn::SOA_Cullable | ukn::SOA_Moveable);
     //      scene.addSceneObject(terrianObject);
            
            ukn::ModelPtr sponza = ukn::AssetManager::Instance().load<ukn::Model>(L"sponza/sponza.obj");
            if(sponza) {
                ukn::SceneObjectPtr sponzaObject = ukn::MakeSharedPtr<ukn::SceneObject>(sponza, ukn::SOA_Cullable | ukn::SOA_Moveable);
                sponzaObject->setModelMatrix(ukn::Matrix4::ScaleMat(1, 1, 1));
                scene.addSceneObject(sponzaObject);
           }
      
            directionalLight = ukn::MakeSharedPtr<ukn::DirectionalLight>(ukn::float3(0, -1, 0.5),
                                                                        ukn::float4(1, 1, 1, 1),
                                                                        1.0,
                                                                        true,
                                                                        1024);
            scene.addLight(directionalLight);
            directionalLight = ukn::MakeSharedPtr<ukn::DirectionalLight>(ukn::float3(1, 0, 0.5),
                                                                        ukn::float4(1, 1, 1, 1),
                                                                        0.5,
                                                                        true,
                                                                        1024);

      /*      spotLight = ukn::MakeSharedPtr<ukn::SpotLight>(ukn::float3(0, 10, 0),
                                                           ukn::float3(0, -1, 0),
                                                           ukn::float4(1, 1, 1, 1),
                                                           1.0,
                                                           gf.load2DTexture(ukn::ResourceLoader::Instance().loadResource(L"SpotCookie.dds")),
                                                           false, 
                                                           1024);
           */
            for(int i=0; i<0; ++i) {
                
            }
            
   //         scene.addLight(spotLight);
            scene.addLight(directionalLight);

            testEffect = gf.createEffect();
            displayDepthTechnique = testEffect->appendTechnique(
                testEffect->createShader(
                ukn::ResourceLoader::Instance().loadResource(L"deferred/display_depth_frag.cg"),
                FRAGMENT_SHADER_DESC("FragmentProgram")),
                testEffect->createShader(
                ukn::ResourceLoader::Instance().loadResource(L"deferred/display_depth_vert.cg"),
                VERTEX_SHADER_DESC("VertexProgram")),
                ukn::ShaderPtr());

        })
        .run();
    
    return 0;
}

