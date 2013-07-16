#ifndef UKN_OVR_CAMERA_CONTROLLER_H_
#define UKN_OVR_CAMERA_CONTROLLER_H_

#include "ukn/CameraController.h"

#include "mist/MathUtil.h"

namespace ukn {

    class OVRCameraController: public ukn::CameraController {
    public:
        OVRCameraController();
        virtual ~OVRCameraController();

    public:
        void move(float x, float y, float z);

        void update();

    private:
        void onKeyDown(Window* window, input::KeyEventArgs& e);
        void onMouseMove(Window* window, input::MouseEventArgs& e);
        
        void attachCamera(const CameraPtr& camera) override;

    private:
        float mEyeYaw, mEyePitch, mEyeRoll, mLastSensorYaw;
        mist::Vector3 mLocalMove;
        mist::Vector3 mEyePos;

        Connection mKeyDownConn;
        Connection mMouseMoveConn;
    };

}

#endif 