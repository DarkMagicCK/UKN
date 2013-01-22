#include "UKN/CameraController.h"
#include "UKN/Context.h"
#include "UKN/Window.h"

namespace ukn {

    CameraController::CameraController():
    mMoveScaler(1.f),
    mRotScaler(1.f) {
        
    }

    CameraController::~CameraController() {

    }

    void CameraController::attachCamera(const CameraPtr& cam) {
        mCamera = cam;
    }

    void CameraController::detachCamera() {
        mCamera = CameraPtr();
    }

    CameraPtr CameraController::getAttachedCamera() const {
        return mCamera;
    }

    void CameraController::setScalers(float rot, float move) {
        mRotScaler = rot;
        mMoveScaler = move;
    }

    float CameraController::getRotationScaler() const {
        return mRotScaler;
    }

    float CameraController::getMoveScaler() const {
        return mMoveScaler;
    }

    FpsCameraController::FpsCameraController() {
        Window& wnd = ukn::Context::Instance().getApp().getWindow();
        wnd.connectMouse(Bind(this, &FpsCameraController::onMouseMove));
        wnd.connectKey(Bind(this, &FpsCameraController::onKeyDown));

        mPrevX = wnd.getMousePos()[0];
        mPrevY = wnd.getMousePos()[1];
    }

    FpsCameraController::~FpsCameraController() {

    }

    void FpsCameraController::attachCamera(const CameraPtr& camera)  {
        Vector3 scale, trans, rot;
        camera->getViewMatrix().decompose(trans, rot, scale);

        Quaternion rotQ = Quaternion(rot.x(), rot.y(), rot.z());
        float sqx = rotQ.x * rotQ.x;
        float sqy = rotQ.y * rotQ.y;
        float sqz = rotQ.z * rotQ.z;
        float sqw = rotQ.w * rotQ.w;
        float unit = sqx + sqy + sqz + sqw;
        float test = rot.x() * rot.y() * rot.z();
        float yaw, pitch, roll;
        if(test > 0.499f * unit) {
            yaw = 2 * atan2(rotQ.z, rotQ.w);
            pitch = d_pi / 2;
            roll = 0;
        } else {
            if(test < -0.499f * unit) {
                yaw = -2 * atan2(rotQ.z, rotQ.w);
                pitch = -d_pi / 2;
                roll = 0;
            } else {
                yaw = atan2(2 * (rotQ.y - rotQ.x * rotQ.z), -sqx - sqy + sqz + sqw);
                pitch = asin(2 * test / unit);
                roll = atan2(2 * (rotQ.z - rotQ.x * rotQ.y), -sqx + sqy - sqz + sqw);
            }
        }

        mRotX[0] = sin(pitch / 2); mRotX[1] = cos(pitch / 2);
        mRotY[0] = sin(yaw / 2); mRotY[1] = cos(yaw / 2);
        mRotZ[0] = sin(roll / 2); mRotZ[1] = cos(roll / 2);

        mInvRot = mist::math::inverse_quaternion(rotQ);
        
        CameraController::attachCamera(camera);
    }

    void FpsCameraController::move(float x, float y, float z) {
        if(mCamera) {
            Vector3 move(x, y, z);
            move = move * getMoveScaler();

            Vector3 newEye = mCamera->getEyePos() + mist::math::transform_quaternion(move, mInvRot);
            mCamera->setViewParams(newEye, newEye + mCamera->getViewVec(), mCamera->getUpVec());
        }
    }

    void FpsCameraController::rotate(float yaw, float pitch, float roll) {
        if(mCamera) {
            pitch *= -mRotScaler / 2;
            yaw *= -mRotScaler / 2;
            roll *= -mRotScaler / 2;

            float2 delta_x, delta_y, delta_z;
            delta_x[0] = sin(pitch); delta_x[1] = cos(pitch);
            delta_y[0] = sin(yaw); delta_y[1] = cos(yaw);
            delta_z[0] = sin(roll); delta_z[1] = cos(roll);

            Quaternion quat_x(mRotX[0] * delta_x[1] + mRotX[1] * delta_x[0], 0, 0, mRotX[1] * delta_x[1] - mRotX[1] * delta_x[0]);
            Quaternion quat_y(0, mRotY[0] * delta_y[1] + mRotY[1] * delta_y[0], 0, mRotY[1] * delta_y[1] - mRotY[1] * delta_y[0]);
            Quaternion quat_z(0, 0, mRotZ[0] * delta_z[1] + mRotZ[1] * delta_z[0], mRotZ[1] * delta_z[1] - mRotZ[1] * delta_z[0]);

            mRotX = float2(quat_x.x, quat_x.w);
            mRotY = float2(quat_y.y, quat_y.w);
            mRotZ = float2(quat_z.z, quat_z.w);

            mInvRot = mist::math::inverse_quaternion(quat_y * quat_x * quat_z);
            Vector3 view_vec = mist::math::transform_quaternion(Vector3(0, 0, 1), mInvRot);
            Vector3 up_vec = mist::math::transform_quaternion(Vector3(0, 1, 0), mInvRot);

            mCamera->setViewParams(mCamera->getEyePos(), mCamera->getEyePos() + view_vec, up_vec);
        }
    }

    void FpsCameraController::onMouseMove(Window* window, input::MouseEventArgs& e) {
        if(e.state == input::Move) {
            float delta_x = e.x - mPrevX;
            float delta_y = e.y - mPrevY;

            mPrevX = e.x;
            mPrevY = e.y;

            this->rotate(delta_x < 0 ? 0.1 : 0-.1, 0, 0);
            this->rotate(0, delta_y < 0 ? 0.1 : -0.1, 0);
        }
    }

    void FpsCameraController::onKeyDown(Window* window, input::KeyEventArgs& e) {
        switch(e.key) {
        case input::W:
        case input::Up:
            this->move(0, 0, 0.1);
            break;

        case input::S:
        case input::Down:
            this->move(0, 0, -0.1);
            break;

        case input::A:
        case input::Left:
            this->move(-0.1, 0, 0);
            break;

        case input::D:
        case input::Right:
            this->move(0.1, 0, 0);
            break;
        }
    }
}