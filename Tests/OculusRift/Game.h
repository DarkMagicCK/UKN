#ifndef UKN_TEST_OVR_APP_H_
#define UKN_TEST_OVR_APP_H_

#include "UKN/UKN.h"

namespace ukn {

    namespace ovr {

        class OvrGame {
        public:
            OvrGame();
            ~OvrGame();

            void start();

            void render(ukn::Window* wnd);
            void update(ukn::Window* wnd);
            void initialize(ukn::Window* wnd);
            void keyDown(ukn::Window* wnd, input::KeyEventArgs& e);

        private:
            ukn::DeferredRendererPtr mRenderer;
            ukn::SSAO* mSSAO;
            ukn::AppLauncher* mApp;
            ukn::CameraControllerPtr mCameraController;
            ukn::LightSourcePtr mDirectionalLight;
            ukn::TerrianPtr mTerrian;
            ukn::FontPtr mFont;

            float mEyeYaw, mEyePitch, mEyeRoll;
            float mLastSensorYaw;
        };

    }

}

#endif // UKN_TEST_OVR_APP_H_