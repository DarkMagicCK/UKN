#include "UKN/UKN.h"

#include "mist/SysUtil.h"
#include "mist/TimeUtil.h"
#include "mist/RandomUtil.h"
#include "mist/Convert.h"
#include "mist/TimeUtil.h"
#include "mist/Query.h"
#include "mist/Profiler.h"

#include <vector>
#include <map>
#include <numeric>

#ifndef MIST_OS_WINDOWS

#include "../Plugins/gl/GLGraphicFactory.h"

int main (int argc, const char * argv[])
{
#else
#pragma comment(linker, "/NODEFAULTLIB:libcmt.lib")
    
#pragma comment(lib, "mist.lib")
#pragma comment(lib, "ukn_dll.lib")
    
    int CALLBACK WinMain(
                         __in  HINSTANCE hInstance,
                         __in  HINSTANCE hPrevInstance,
                         __in  LPSTR lpCmdLine,
                         __in  int nCmdSho) {
#endif
        
        ukn::FontPtr font;
        ukn::CameraControllerPtr camController;
        
#ifndef MIST_OS_WINDOWS
        ukn::GraphicFactoryPtr factory;
        ukn::CreateGraphicFactory(factory);
        ukn::Context::Instance().registerGraphicFactory(factory);
#endif
        
        ukn::AppLauncher(L"OpenGL Test")
        .create(
                ukn::ContextCfg::Default()
                .width(1024)
                .height(768)
                .sampleCount(1)
                .showMouse(true)
                .isFullScreen(false)
                .graphicFactoryName(L"D3D11Plugin.dll")
                )
        .connectUpdate([&](ukn::Window* ) {
          
        })
        .connectKey([&](ukn::Window* window, ukn::input::KeyEventArgs& e) {
            if(e.key == ukn::input::Escape)
                ukn::Context::Instance().getApp().terminate();
            

        })
        .connectRender([&](ukn::Window* wnd) {
            ukn::GraphicDevice& gd = ukn::Context::Instance().getGraphicFactory().getGraphicDevice();
            
            gd.clear(ukn::CM_Color, ukn::color::Blue, 1.f, 0);
            
        })
        .connectInit([&](ukn::Window*) {
            ukn::GraphicFactory& gf = ukn::Context::Instance().getGraphicFactory();
            
            camController = ukn::MakeSharedPtr<ukn::FpsCameraController>();
            ukn::Viewport& vp = gf.getGraphicDevice().getCurrFrameBuffer()->getViewport();
            vp.camera->setViewParams(ukn::Vector3(0, 5, 0), ukn::Vector3(0, 0, 1));
            
            camController->attachCamera(vp.camera);
            
            font = ukn::Font::Create(L"Arial.ttf", 20);
            
            
        })
        .run();
        
        return 0;
    }
    
