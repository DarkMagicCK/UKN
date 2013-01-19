
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

#include "mist/SysUtil.h"

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
	ukn::RenderBufferPtr renderBuffer;
	ukn::TexturePtr texture;
    
     ukn::AppLauncher(L"LeapMotion Test")
        .create(
                ukn::ContextCfg::Default()
                  .width(800)
                  .height(600)
				  .graphicFactoryName(L"D3D10Plugin.dll")
               )
		.connectUpdate([](ukn::Window* ) {

	    })
	    .connectRender([&](ukn::Window*) {
			ukn::GraphicDevice& gd = ukn::Context::Instance().getGraphicFactory().getGraphicDevice();
			gd.clear(ukn::CM_Color | ukn::CM_Depth, mist::color::Skyblue, 1.0f, 0);
			
			if(texture)
				gd.bindTexture(texture);
			if(renderBuffer)
				gd.renderBuffer(renderBuffer);
		})
		.connectInit([&](ukn::Window*) {
			ukn::GraphicFactory& gf = ukn::Context::Instance().getGraphicFactory();
			renderBuffer = gf.createRenderBuffer();

			ukn::Vertex2D vertices[10];
			vertices[0].x = -1; vertices[0].y = -1; vertices[0].z = 0.0f;
			vertices[1].x = 0.0f; vertices[1].y = 1; vertices[1].z = 0.0f;
			vertices[2].x = 1; vertices[2].y = -1; vertices[2].z = 0.0f;

			vertices[0].u = 0.0f; vertices[0].v = 1.0f;
			vertices[1].u = 0.5f; vertices[1].v = 0.0f;
			vertices[2].u = 1.0f; vertices[2].v = 1.0f;

			ukn::GraphicBufferPtr vertexBuffer = gf.createVertexBuffer(ukn::GraphicBuffer::WriteOnly,
																	   ukn::GraphicBuffer::Dynamic,
																	   3,
																	   vertices,
																	   ukn::Vertex2D::Format());


			if(vertexBuffer) {
				renderBuffer->bindVertexStream(vertexBuffer, ukn::Vertex2D::Format());
				renderBuffer->setRenderMode(ukn::RM_Triangle);
			}


			texture = gf.load2DTexture(ukn::ResourceLoader::Instance().loadResource(L"test.png"));
			if(!texture) {
				mist::MessageBox::Show(L"error loading texture", L"Error", mist::MBO_OK);
			}
		})
		.run();
    
    return 0;
}

