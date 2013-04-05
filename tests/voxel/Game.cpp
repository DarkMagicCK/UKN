#include "Game.h"

namespace ukn {

    namespace voxel {

        Game::Game() {

        }

        Game::~Game() {


        }

        void Game::render(ukn::Window* wnd) {
            ukn::GraphicDevice& gd = ukn::Context::Instance().getGraphicFactory().getGraphicDevice();

            gd.clear(ukn::CM_Color, ukn::color::Black, 0.f, 1);
        }

        void Game::update(ukn::Window* wnd) {

        }

        void Game::initialize(ukn::Window* wnd) {

        }

        void Game::start() {
            mApp = new ukn::AppLauncher(L"Voxel Test");

            (*mApp).create(
                ukn::ContextCfg::Default()
                  .width(800)
                  .height(600)
                  .sampleCount(1)
                  .showMouse(true)
                  .isFullScreen(false)
                  .graphicFactoryName(L"D3D11Plugin.dll")
            )
            .connectInit(ukn::Bind(this, &Game::initialize))
            .connectRender(ukn::Bind(this, &Game::render))
            .connectUpdate(ukn::Bind(this, &Game::update))
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