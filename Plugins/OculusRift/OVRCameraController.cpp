#include "OVRCameraController.h"
#include "OVRDevice.h"
#include "UKN/Context.h"

namespace ukn {

    using namespace mist;

    OVRCameraController::OVRCameraController():
    mEyeYaw(3.1415926f),
    mEyePitch(0),
    mEyeRoll(0),
    mLastSensorYaw(0),
    mLocalMove(Vector3(0, 0, 0)){
        
        Window& wnd = ukn::Context::Instance().getApp().getWindow();
        mMouseMoveConn = wnd.connectMouse(Bind(this, &OVRCameraController::onMouseMove));
        mKeyDownConn = wnd.connectKey(Bind(this, &OVRCameraController::onKeyDown));
    }

    OVRCameraController::~OVRCameraController() {

    }

    void OVRCameraController::move(float x, float y, float z) {
        if(mCamera) {
            Vector3 move(x, y, z);
            mLocalMove = move * getMoveScaler();
            mLocalMove.normalize();
            
            Matrix4 yawRotate = Matrix4::RotY(mEyeYaw);
            Vector3 orientationVector = yawRotate * mLocalMove;
            mEyePos += orientationVector;
            
            this->update();
        }
    }

    void OVRCameraController::update() {
        OVRDevice& ovr = OVRDevice::Instance();
        if(ovr.hasDevice()) {
            OVR::Quatf hmdOrient = ovr.getSensorFusion().GetOrientation();
            float yaw = 0.f;
            hmdOrient.GetEulerAngles<OVR::Axis_Y, OVR::Axis_X, OVR::Axis_Z>(&yaw, &mEyePitch, &mEyeRoll);
            mEyeYaw += (yaw - mLastSensorYaw);
            mLastSensorYaw = yaw;

            Matrix4 rollPitchYaw = Matrix4::RotY(mEyeYaw) * Matrix4::RotX(mEyePitch) * Matrix4::RotZ(mEyeRoll);

            Vector3 up = rollPitchYaw * Vector3::Up();
            Vector3 forward = rollPitchYaw * Vector3::Forward();

            float headBaseToEyeHeight = 0.15f;
            float headBaseToEyeProtrusion = 0.09f;

            Vector3 eyeCenterInHeadFrame(0.0f, headBaseToEyeHeight, -headBaseToEyeProtrusion);
            Vector3 shiftedEyePos = mEyePos + rollPitchYaw * eyeCenterInHeadFrame;
            shiftedEyePos.y() -= eyeCenterInHeadFrame.y();

            mCamera->setViewParams(shiftedEyePos, shiftedEyePos + forward, up);
        }

    }

    void OVRCameraController::attachCamera(const CameraPtr& camera) {
        mEyePos = camera->getEyePos();
        CameraController::attachCamera(camera);
    }
    
    void OVRCameraController::onKeyDown(Window* window, input::KeyEventArgs& e) {
        
        switch(e.key) {
        case input::W:
        case input::Up:
            this->move(0, 0, 1.f);
            break;

        case input::S:
        case input::Down:
            this->move(0, 0, -1.f);
            break;

        case input::A:
        case input::Left:
            this->move(-1.f, 0, 0);
            break;

        case input::D:
        case input::Right:
            this->move(1.f, 0, 0);
            break;
        }
     }
     
     void OVRCameraController::onMouseMove(Window* window, input::MouseEventArgs& e) {

     }

}