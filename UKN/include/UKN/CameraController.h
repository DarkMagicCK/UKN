#ifndef PROJECT_UNKNOWN_CAMERACONTROLLER_H_
#define PROJECT_UNKNOWN_CAMERACONTROLLER_H_

#include "UKN/PreDeclare.h"
#include "UKN/Camera.h"
#include "UKN/Input.h"

namespace ukn {

    class UKN_API CameraController {
    public:
        CameraController();
        virtual ~CameraController();

        virtual void attachCamera(const CameraPtr& camera);
        virtual void detachCamera();

        CameraPtr getAttachedCamera() const;

        void setScalers(float rot, float move);
        float getRotationScaler() const;
        float getMoveScaler() const;

        void setInvertUpdown(bool flag);
        bool isInvertUpdown() const;

    protected:
        CameraPtr mCamera;

        float mRotScaler;
        float mMoveScaler;
        bool mInvertUpdown;;
    };

    class UKN_API FpsCameraController: public CameraController {
    public:
        FpsCameraController();
        virtual ~FpsCameraController();

        void attachCamera(const CameraPtr& camera) override;

        void move(float x, float y, float z);
        void rotate(float yaw, float pitch, float roll);

        void onKeyDown(Window* window, input::KeyEventArgs& e);
        void onMouseMove(Window* window, input::MouseEventArgs& e);

    private:
        float2 mRotX;
        float2 mRotY;
        float2 mRotZ;
        Quaternion mInvRot;

        float mPrevX;
        float mPrevY;
    };

}

#endif