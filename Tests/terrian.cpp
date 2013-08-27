
#include "UKN/UKN.h"
#include "mist/SysUtil.h"
#include "mist/Profiler.h"
#include "mist/RandomUtil.h"

#include "Test.h"

#ifndef MIST_OS_WINDOWS

int main (int argc, const char * argv[])
{

#define GRAPHIC_PLUGIN_NAME L"libGLPlugin"
    
#else

#pragma comment(linker, "/NODEFAULTLIB:libcmt.lib")
    
#define GRAPHIC_PLUGIN_NAME L"GLPlugin"
    
int CALLBACK WinMain(__in  HINSTANCE hInstance,
                     __in  HINSTANCE hPrevInstance,
                     __in  LPSTR lpCmdLine,
                     __in  int nCmdSho) {
#endif
    SetupTestMediaResourceDirectories();

        ukn::Matrix4 worldMat;
        ukn::CameraController* camController;
        ukn::FontPtr font;
        ukn::SkyboxPtr skybox;
        ukn::TerrianPtr terrian;
        ukn::LightSourcePtr spotLight;
        ukn::LightSourcePtr directionalLight;
        ukn::SSAO* ssao;
        ukn::ModelPtr dragonModel;
    
        ukn::EffectPtr testEffect;
        ukn::EffectTechniquePtr displayDepthTechnique;
        
        ukn::DeferredRendererPtr deferredRenderer;
        
        ukn::PointLightPtr pointLights[400];
        float rs[400];
        ukn::uint32 pointLightCount = 0;
        
#ifndef MIST_OS_WINDOWS
        ukn::GraphicFactoryPtr factory;
        ukn::CreateGraphicFactory(factory);
        ukn::Context::Instance().registerGraphicFactory(factory);
#endif
        
        mist::ResourceLoader::Instance().addPath(L"./Media");
    
        ukn::uint32 count;
        float r = 0;
        ukn::AppLauncher(L"OpenGL Test")
        .create(
                ukn::ContextCfg::Default()
                .width(1024)
                .height(768)
                .sampleCount(1)
                .showMouse(true)
                .isFullScreen(false)
                .graphicFactoryName(GRAPHIC_PLUGIN_NAME)
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
                } else if (e.key == ukn::input::R) {
                    deferredRenderer->reloadShaders();
                    ssao->reloadShaders();
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
            ukn::SpriteBatch& sb = ukn::SpriteBatch::DefaultObject();
            
            sb.begin();
            sb.draw(deferredRenderer->getFinalTexture(), ukn::Rectangle(),
                        ukn::Rectangle(0, 0, wnd->width()/2, wnd->height()/2, true));
            sb.draw(deferredRenderer->getNormalTarget()->getTexture(), ukn::Rectangle(),
                        ukn::Rectangle(wnd->width()/2, 0, wnd->width()/2, wnd->height()/2, true));
            sb.draw(ssao->getSSAOTarget()->getTexture(), ukn::Rectangle(),
                        ukn::Rectangle(wnd->width()/2, wnd->height()/2,  wnd->width()/2, wnd->height()/2, true));
            sb.draw(ssao->getSSAOBlurTarget()->getTexture(), ukn::Rectangle(),
                        ukn::Rectangle(0, wnd->height()/2, wnd->width()/2, wnd->height()/2, true));
            sb.end();

            
            if(font) {
                mist::ProfileData shadowMapProf = mist::Profiler::Instance().get(L"SHADOW_MAP");
                mist::ProfileData gbufferProf = mist::Profiler::Instance().get(L"DEFERRED_GBUFFER");
                mist::ProfileData lightMapPro = mist::Profiler::Instance().get(L"DEFERRED_LIGHTMAP");
                
                 font->begin();
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
                 
                 font->end();
            }
        })
        .connectInit([&](ukn::Window*) {
            ukn::GraphicFactory& gf = ukn::Context::Instance().getGraphicFactory();
            
            camController = new ukn::FpsCameraController();
            ukn::Viewport& vp = gf.getGraphicDevice().getCurrFrameBuffer()->getViewport();
            vp.camera->setViewParams(ukn::Vector3(0, 5, 0), ukn::Vector3(0, 0, 1));
            
            camController->attachCamera(vp.camera);
            
            font = ukn::Font::Create(L"Arial.ttf", 20);

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
            deferredRenderer->addPostEffect(L"Fog");
            
            ssao = ukn::checked_cast<ukn::SSAO*>(deferredRenderer->getPostEffect(L"SSAO").get());
            //    fog = ukn::checked_cast<ukn::Fog*>(deferredRenderer->getPostEffect(L"Fog").get());
            
            ukn::SceneManager& scene = ukn::Context::Instance().getSceneManager();
            
            dragonModel = ukn::AssetManager::Instance().load<ukn::Model>(L"sponza/sponza.obj");
            if(dragonModel) {
                ukn::SceneObjectPtr dragonObject = ukn::MakeSharedPtr<ukn::SceneObject>(dragonModel, ukn::SOA_Cullable | ukn::SOA_Moveable);
                dragonObject->setModelMatrix(ukn::Matrix4::ScaleMat(5, 5, 5) * ukn::Matrix4::TransMat(0, 5, 0));
               
                scene.addSceneObject(dragonObject);
            }
            
            ukn::SceneObjectPtr skyboxObject = ukn::MakeSharedPtr<ukn::SceneObject>(skybox, ukn::SOA_Overlay);
            skyboxObject->setModelMatrix(ukn::Matrix4::ScaleMat(100, 100, 100));
            
      //      scene.addSceneObject(skyboxObject);
            
            ukn::SceneObjectPtr terrianObject = ukn::MakeSharedPtr<ukn::SceneObject>(terrian, ukn::SOA_Cullable | ukn::SOA_Moveable);
            scene.addSceneObject(terrianObject);
                       
            directionalLight = ukn::MakeSharedPtr<ukn::DirectionalLight>(ukn::float3(0, -1, 0.5),
                                                                         ukn::float4(1, 1, 1, 1),
                                                                         1.0,
                                                                         true,
                                                                         256);
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
    
