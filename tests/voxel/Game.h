#ifndef UKN_TEST_VOXEL_APP_H_
#define UKN_TEST_VOXEL_APP_H_

#include "UKN/UKN.h"

namespace ukn {

    namespace voxel {

        class Game {
        public:
            Game();
            ~Game();

            void start();

            void render(ukn::Window* wnd);
            void update(ukn::Window* wnd);
            void initialize(ukn::Window* wnd);

        private:
            ukn::AppLauncher* mApp;
        };

        class CameraController: public ukn::FpsCameraController {
        public:
            CameraController();
            virtual ~CameraController();

            void onKeyDown(Window* window, input::KeyEventArgs& e);
            void onMouseMove(Window* window, input::MouseEventArgs& e);

        };
    }

}

#endif // UKN_TEST_VOXEL_APP_H_