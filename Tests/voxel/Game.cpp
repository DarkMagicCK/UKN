#include "voxel/Game.h"

#undef MessageBox
#include "mist/SysUtil.h"

#include "OculusRift/OVRDevice.h"

namespace ukn {

    namespace voxel {


        Game::Game() {

        }

        Game::~Game() {


        }

        void Game::render(ukn::Window* wnd) {
            ukn::GraphicDevice& gd = ukn::Context::Instance().getGraphicFactory().getGraphicDevice();

            gd.clear(ukn::CM_Color, ukn::color::Black, 0.f, 1);

            
            ukn::SceneManager& scene = ukn::Context::Instance().getSceneManager();
            
            {
                scene.getLightManager()->makeShadowMaps(scene);
            }
            {
                mRenderer->renderScene(scene);
            }
            {
            }
            ukn::SpriteBatch& sb = ukn::SpriteBatch::DefaultObject();
            sb.begin(ukn::SBB_None, ukn::SBS_Deffered, ukn::Matrix4());
            sb.draw(mRenderer->getLightMapRT()->getTargetTexture(ukn::ATT_Color0), 
                    ukn::Rectangle(0, wnd->height() / 2, wnd->width() / 2, wnd->height() / 2, true));
            sb.draw(mRenderer->getGBufferRT()->getTargetTexture(ukn::ATT_Color1), 
                    ukn::Rectangle(wnd->width() / 2, wnd->height() / 2, wnd->width() / 2, wnd->height() / 2, true));
            sb.draw(mSSAO->getSSAOTarget()->getTexture(), 
                    ukn::Rectangle(0, 0, wnd->width() / 2, wnd->height() / 2, true));
            sb.draw(mRenderer->getFinalTexture(), 
                    ukn::Rectangle(0, 0, wnd->width() , wnd->height(), true));
            sb.end();
        }

        void Game::update(ukn::Window* wnd) {
            static float v = 0.0f;
            v += 0.001f;
     //       mDirectionalLight->setDirection(ukn::Vector3(sin(v), cos(v), 0));
        }

        void Game::keyDown(ukn::Window* wnd, input::KeyEventArgs& e) {
            if(e.key == ukn::input::Escape)
                ukn::Context::Instance().getApp().terminate();
            if(e.state == ukn::input::Release) {
                if(e.key == ukn::input::F1) {
                    if(mCameraController->getAttachedCamera()) {
                        mCameraController->detachCamera();
                    } else {
                        ukn::Viewport& vp = ukn::Context::Instance()
                                                .getGraphicFactory()
                                                .getGraphicDevice()
                                                .getScreenFrameBuffer()->getViewport();
                        mCameraController->attachCamera(vp.camera);
                    }
                }

                if(e.key == ukn::input::U) {
                    mSSAO->setSampleRadius(mSSAO->getSampleRadius() + 0.01);
                } else if(e.key == ukn::input::I) {
                    mSSAO->setDistanceScale(mSSAO->getDistanceScale() + 0.01);
                }
                else if(e.key == ukn::input::J) {
                    mSSAO->setSampleRadius(mSSAO->getSampleRadius() - 0.01);
                } else if(e.key == ukn::input::K) {
                    mSSAO->setDistanceScale(mSSAO->getDistanceScale() - 0.01);
                }
            }
        }

        void Game::initialize(ukn::Window* wnd) {
            mRenderer = new ukn::DeferredRenderer();
            mRenderer->addPostEffect(L"SSAO");
            mSSAO = checked_cast<SSAO*>(mRenderer->getPostEffect(L"SSAO").get());

            ukn::GraphicFactory& gf = ukn::Context::Instance().getGraphicFactory();
     
            mCameraController = new ukn::FpsCameraController();
            ukn::Viewport& vp = gf.getGraphicDevice().getCurrFrameBuffer()->getViewport();

            mCameraController->attachCamera(vp.camera);
            
            ukn::SceneManager& scene = ukn::Context::Instance().getSceneManager();
           
            vp.camera->setViewParams(ukn::Vector3(0, 5, 0), ukn::Vector3(0, 0, 1));

            mDirectionalLight = ukn::MakeSharedPtr<ukn::DirectionalLight>(ukn::float3(0, -1, 1),
                                                                        ukn::color::Skyblue.c,
                                                                        1.0,
                                                                        false,
                                                                        1024);
            scene.addLight(mDirectionalLight);

            SkyboxPtr skybox = MakeSharedPtr<ukn::Skybox>();
            if(!skybox->load(mist::ResourceLoader::Instance().loadResource(L"skyboxsun25degtest.png"))) {
                mist::log_error(L"unable to load skybox");
            }

            ukn::SceneObjectPtr skyboxObject = ukn::MakeSharedPtr<ukn::SceneObject>(skybox, ukn::SOA_Overlay);
            skyboxObject->setModelMatrix(ukn::Matrix4::ScaleMat(200, 200, 200));
                
            scene.addSceneObject(skyboxObject);
        }

        void Game::start() {
            mApp = new ukn::AppLauncher(L"Voxel Test");

            (*mApp).create(
                ukn::ContextCfg::Default()
                  .width(1024)
                  .height(768)
                  .sampleCount(1)
                  .showMouse(true)
                  .isFullScreen(false)
                  .graphicFactoryName(L"D3D11Plugin.dll")
            )
            .connectInit(ukn::Bind(this, &Game::initialize))
            .connectRender(ukn::Bind(this, &Game::render))
            .connectUpdate(ukn::Bind(this, &Game::update))
            .connectKey(ukn::Bind(this, &Game::keyDown))
            .run();
        }


        CameraController::CameraController() {

        }

        CameraController::~CameraController() {

        }

        void CameraController::onKeyDown(Window* window, input::KeyEventArgs& e) {

        }

        void CameraController::onMouseMove(Window* window, input::MouseEventArgs& e) {

        }
    }


}