
#include "../Plugins/GL/GLGraphicFactory.h"

#include "UKN/App.h"
#include "UKN/Context.h"
#include "UKN/Window.h"
#include "UKN/Texture.h"
#include "UKN/GraphicDevice.h"
#include "UKN/FrameBuffer.h"
#include "UKN/Texture.h"
#include "UKN/SpriteBatch.h"
#include "UKN/RenderBuffer.h"
#include "UKN/Font.h"
#include "UKN/Asset.h"
#include "UKN/Skeletal.h"
#include "UKN/Input.h"

#include "UKN/tmx/TMXTiledMap.h"


#include <vector>
#include <map>

#include "mist/Query.h"

#include <numeric>

#ifndef MIST_OS_WINDOWS
int main (int argc, const char * argv[])
{
#else
#pragma comment(linker, "/NODEFAULTLIB:libcmt.lib")

#pragma comment(lib, "mist.lib")
#pragma comment(lib, "ukn_dll.lib")
#pragma comment(lib, "GLPlugin.lib")

int CALLBACK WinMain(
  __in  HINSTANCE hInstance,
  __in  HINSTANCE hPrevInstance,
  __in  LPSTR lpCmdLine,
  __in  int nCmdShow
) {
#endif

    // register plugins manually for test
  //  ukn::GraphicFactoryPtr gl_factory;
    
//	ukn::CreateGraphicFactory(gl_factory);
//    ukn::Context::Instance().registerGraphicFactory(gl_factory);
   

    
     ukn::AppLauncher(L"LeapMotion Test")
        .create(
                ukn::ContextCfg::Default()
                  .width(800)
                  .height(600)
				  .graphicFactoryName(L"D3D10Plugin.dll")
               )
		.connectUpdate([](void* ) {

	    })
	    .connectRender([](void*) {
			ukn::GraphicDevice& gd = ukn::Context::Instance().getGraphicFactory().getGraphicDevice();
			gd.clear(ukn::CM_Color, mist::color::Skyblue, 0, 0);
			
		})
		.run();
    
    return 0;
}

