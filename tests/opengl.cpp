#include "UKN/UKN.h"

#include "mist/SysUtil.h"
#include "mist/TimeUtil.h"
#include "mist/RandomUtil.h"
#include "mist/Convert.h"
#include "mist/TimeUtil.h"
#include "mist/Query.h"
#include "mist/Profiler.h"

#include "UKN/2DHelper.h"
#include "UKN/CgHelper.h"

#include <vector>
#include <map>
#include <numeric>

#include "../Plugins/gl/GLPreq.h"
#ifndef MIST_OS_WINDOWS

#include "../Plugins/gl/GLGraphicFactory.h"

#include "../Plugins/gl/GLPreq.h"

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
        ukn::TexturePtr texture;

        ukn::RenderBufferPtr renderBuffer;
        ukn::GraphicBufferPtr vtxBuffer;
        
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
                .graphicFactoryName(L"GLPlugin.dll")
                )
        .connectUpdate([&](ukn::Window* ) {
          
        })
        .connectKey([&](ukn::Window* window, ukn::input::KeyEventArgs& e) {
            if(e.key == ukn::input::Escape)
                ukn::Context::Instance().getApp().terminate();
            

        })
        .connectRender([&](ukn::Window* wnd) {
            ukn::GraphicDevice& gd = ukn::Context::Instance().getGraphicFactory().getGraphicDevice();
            
            gd.clear(ukn::CM_Color | ukn::CM_Depth, ukn::color::Blue, 1.f, 0);

            ukn::SpriteBatch& sb = ukn::SpriteBatch::DefaultObject();
            sb.begin(ukn::SBB_None, ukn::SBS_None, ukn::Matrix4());
            sb.draw(texture, ukn::Vector2(0, 0));
            sb.end();

        })
        .connectInit([&](ukn::Window*) {
            ukn::GraphicFactory& gf = ukn::Context::Instance().getGraphicFactory();
            
            camController = ukn::MakeSharedPtr<ukn::FpsCameraController>();
            ukn::Viewport& vp = gf.getGraphicDevice().getCurrFrameBuffer()->getViewport();
            vp.camera->setViewParams(ukn::Vector3(0, 5, 0), ukn::Vector3(0, 0, 1));
            
            camController->attachCamera(vp.camera);
            
            font = ukn::Font::Create(L"Arial.ttf", 20);
            texture = ukn::AssetManager::Instance().load<ukn::Texture>(L"test.jpg");

            vtxBuffer = gf.createVertexBuffer(ukn::GraphicBuffer::Access::WriteOnly,
                                              ukn::GraphicBuffer::Usage::Dynamic,
                                              6,
                                              0,
                                              ukn::Vertex2D::Format());
            renderBuffer = gf.createRenderBuffer();
            renderBuffer->bindVertexStream(vtxBuffer, ukn::Vertex2D::Format());
        })
        .run();
        
        return 0;
    }
    
