
#include "../Plugins/GL/GLGraphicFactory.h"
#include "../Plugins/D3D10/D3D10GraphicFactory.h"

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

int CALLBACK WinMain(
  __in  HINSTANCE hInstance,
  __in  HINSTANCE hPrevInstance,
  __in  LPSTR lpCmdLine,
  __in  int nCmdShow
) {
#endif
    
    mist::query::_TestQuery();
    
    mist::thread::Future<double> f = mist::thread::RunAsync<double>([]()->double {
        double result = 0.f;
        for(int i=0; i<100; ++i) {
            result += (i * 100);
            printf("%f\n", result);
        }
        return result;
    });
    
    f.wait();
    printf("%f\n", f.getResult());
    // register plugins manually for test
    ukn::GraphicFactoryPtr gl_factory;
    ukn::CreateGraphicFactoryD3D(gl_factory);

    ukn::Context::Instance().registerGraphicFactory(gl_factory);
   
    
     ukn::AppInstance(L"LeapMotion Test")
        .create(
                ukn::ContextCfg::Default()
                  .width(800)
                  .height(600)
               )
		.updateFunc([](void* ) {

	    })
	    .renderFunc([](void*) {
			ukn::GraphicDevice& gd = ukn::Context::Instance().getGraphicFactory().getGraphicDevice();
			gd.clear(ukn::CM_Color, mist::color::Skyblue, 0, 0);
			
		})
        .run();
    
    return 0;
}

