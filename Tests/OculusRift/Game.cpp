#include "OculusRift/Game.h"

#include "mist/SysUtil.h"

#include "OculusRift/OVRDevice.h"
#include "OculusRift/OVRPostEffect.h"
#include "OculusRift/OVRCameraController.h"

namespace ukn {

#pragma comment(lib, "OculusRift.lib")

    namespace ovr {
        
        OvrGame::OvrGame() {
            mLastSensorYaw = 0;
            mEyePitch = mEyeYaw = mEyeRoll = 0;
        }

        OvrGame::~OvrGame() {


        }

        void OvrGame::render(ukn::Window* wnd) {
            ukn::GraphicDevice& gd = ukn::Context::Instance().getGraphicFactory().getGraphicDevice();

            gd.clear(ukn::CM_Color, ukn::color::Black, 0.f, 1);

            ukn::SceneManager& scene = ukn::Context::Instance().getSceneManager();
            ukn::Viewport& vp = gd.getCurrFrameBuffer()->getViewport();

            OVRDevice& ovr = OVRDevice::Instance();
            if(ovr.hasDevice()) {
                OVR::Quatf hmdOrient = ovr.getSensorFusion().GetOrientation();
                float yaw = 0.f;
                hmdOrient.GetEulerAngles<OVR::Axis_Y, OVR::Axis_X, OVR::Axis_Z>(&yaw, &mEyePitch, &mEyeRoll);
                mEyeYaw += (yaw - mLastSensorYaw);
                mLastSensorYaw = yaw;

                ((ukn::OVRCameraController*)(mCameraController.get()))->update();
            }

            OVR::Util::Render::StereoEyeParams left = ukn::OVRDevice::Instance().getStereoConfig().GetEyeRenderParams(OVR::Util::Render::StereoEye_Left);
            OVR::Util::Render::StereoEyeParams right = ukn::OVRDevice::Instance().getStereoConfig().GetEyeRenderParams(OVR::Util::Render::StereoEye_Right);

            {
               
            }
            {
                scene.getLightManager()->makeShadowMaps(scene);
            }
            Matrix4 viewMat = vp.camera->getViewMatrix();
            {
                gd.setViewport(ukn::Viewport(0, 0, left.VP.w, left.VP.h));
                vp.camera->setProjMat(OVRUtils::OVRMatToMat(left.Projection));
                
                Matrix4 viewMatAdjust = OVRUtils::OVRMatToMat(left.ViewAdjust);
                vp.camera->setViewMat(viewMatAdjust *viewMat);

                mRenderer->renderScene(scene);
                
                ukn::SpriteBatch& sb = ukn::SpriteBatch::DefaultObject();
                sb.begin(ukn::SBB_None, ukn::SBS_Deffered, ukn::Matrix4());
                
                sb.draw(mRenderer->getFinalTexture(), 
                        ukn::Rectangle(0, 0, wnd->width() / 2, wnd->height(), true));
                sb.end();
            }

            {
                gd.setViewport(ukn::Viewport(0, 0, right.VP.w, right.VP.h));
                vp.camera->setProjMat(OVRUtils::OVRMatToMat(right.Projection));
                
                Matrix4 viewMatAdjust = OVRUtils::OVRMatToMat(right.ViewAdjust);
                vp.camera->setViewMat(viewMatAdjust * viewMat);

                mRenderer->renderScene(scene);
                
                ukn::SpriteBatch& sb = ukn::SpriteBatch::DefaultObject();
                sb.begin(ukn::SBB_None, ukn::SBS_Deffered, ukn::Matrix4());
                sb.draw(mRenderer->getFinalTexture(), 
                        ukn::Rectangle(wnd->width() / 2, 0, wnd->width() / 2, wnd->height(), true));
                sb.end();
            }
            mFont->begin();
            mFont->draw(gd.description().c_str(), 0, 20, ukn::FA_Left, ukn::color::Skyblue);
            mFont->draw((ukn::FormatString(L"DeviceOrient: {0}"), mEyeYaw),
                            0, 
                            0, 
                            ukn::FA_Left,
                            ukn::color::Skyblue);
            mFont->end();


            
        }

        void OvrGame::update(ukn::Window* wnd) {
            static float v = 0.0f;
            v += 0.001f;
        //    mDirectionalLight->setDirection(ukn::Vector3(sin(v), cos(v), 0));
        }

        void OvrGame::keyDown(ukn::Window* wnd, input::KeyEventArgs& e) {
            if(e.key == ukn::input::Escape)
                ukn::Context::Instance().getApp().terminate();
        }

        void OvrGame::initialize(ukn::Window* wnd) {
            mist::ResourceLoader::Instance().addPath(L"../Release/");

            ukn::OVRDevice::Instance().checkDevice();

            mRenderer = new ukn::DeferredRenderer(
                float2(wnd->width(), wnd->height()) * ukn::OVRDevice::Instance().getStereoConfig().GetDistortionScale()    
            );
            mRenderer->addPostEffect(L"SSAO");
            mSSAO = checked_cast<SSAO*>(mRenderer->getPostEffect(L"SSAO").get());

            mRenderer->registerPostEffect<OVRPostEffect>(L"OVR");
            mRenderer->addPostEffect(L"OVR");

            ukn::GraphicFactory& gf = ukn::Context::Instance().getGraphicFactory();
     
            mCameraController = new ukn::OVRCameraController();
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
            
           ukn::ModelPtr sponza = ukn::AssetManager::Instance().load<ukn::Model>(L"sponza/sponza.obj");
            if(sponza) {
                ukn::SceneObjectPtr sponzaObject = ukn::MakeSharedPtr<ukn::SceneObject>(sponza, ukn::SOA_Cullable | ukn::SOA_Moveable);
                sponzaObject->setModelMatrix(ukn::Matrix4::ScaleMat(1, 1, 1));
                scene.addSceneObject(sponzaObject);
            }

            mFont =  ukn::Font::Create(L"Consola.ttf", 20);

        }

        void OvrGame::start() {
            mApp = new ukn::AppLauncher(L"Voxel Test");

            (*mApp).create(
                ukn::ContextCfg::Default()
                  .width(1280)
                  .height(800)
                  .sampleCount(1)
                  .showMouse(true)
                  .isFullScreen(true)
                  .graphicFactoryName(L"D3D11Plugin.dll")
            )
            .connectInit(ukn::Bind(this, &OvrGame::initialize))
            .connectRender(ukn::Bind(this, &OvrGame::render))
            .connectUpdate(ukn::Bind(this, &OvrGame::update))
            .connectKey(ukn::Bind(this, &OvrGame::keyDown))
            .run();
        }

    }


}