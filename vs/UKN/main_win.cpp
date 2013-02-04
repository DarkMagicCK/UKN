#include "UKN/App.h"
#include "UKN/Context.h"
#include "UKN/Window.h"
#include "UKN/Texture.h"
#include "UKN/GraphicDevice.h"
#include "UKN/GraphicBuffer.h"
#include "UKN/GraphicFactory.h"
#include "UKN/FrameBuffer.h"
#include "UKN/Texture.h"
#include "UKN/SpriteBatch.h"
#include "UKN/RenderBuffer.h"
#include "UKN/Font.h"
#include "UKN/Asset.h"
#include "UKN/Skeletal.h"
#include "UKN/Input.h"
#include "UKN/Shader.h"
#include "UKN/Model.h"
#include "UKN/CameraController.h"

#include "UKN/tmx/TMXTiledMap.h"

#include "mist/SysUtil.h"
#include "mist/TimeUtil.h"
#include "mist/RandomUtil.h"
#include "mist/Convert.h"

#include <vector>
#include <map>

#include "mist/Query.h"

#include <numeric>

template<typename T, int N>
struct NDArray {
    NDArray()  {} 
    NDArray<T, N-1>& operator [](size_t index) { return a[index]; }
    std::vector<NDArray<T, N-1> > a;
};

template<typename T>
struct NDArray<T, 1> {
    NDArray() {} 
    T& operator [](size_t index) { return a[index]; }
    std::vector<T> a;
};

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

    ukn::RenderBufferPtr renderBuffer;
    ukn::TexturePtr texture;
    ukn::TexturePtr texture2;
    ukn::Matrix4 worldMat;
    ukn::EffectPtr effect;
    ukn::CameraController* camController;
    ukn::FontPtr font;

    ukn::Vector3 positions[5];
    for(int i=0; i<5; ++i) {
        positions[i] =  ukn::Vector3(mist::Random::RandomFloat(-1, 1),
                                   mist::Random::RandomFloat(-1, 1),
                                   0);
               
    }
    
#ifndef MIST_OS_WINDOWS
    ukn::GraphicFactoryPtr factory;
    ukn::CreateGraphicFactory(factory);
    ukn::Context::Instance().registerGraphicFactory(factory);
#endif

     ukn::AppLauncher(L"Windows Test")
        .create(
                ukn::ContextCfg::Default()
                  .width(800)
                  .height(600)
                  .graphicFactoryName(L"D3D11Plugin.dll")
                  .fps(60)
               )
        .connectUpdate([](ukn::Window* ) {

        })
        .connectKey([&](ukn::Window* window, ukn::input::KeyEventArgs& e) {
            if(e.key == ukn::input::W) 
                worldMat.translate(0, 0, -0.1f);
            else if(e.key == ukn::input::S)
                worldMat.translate(0, 0, 0.1f);
        })
        .connectRender([&](ukn::Window*) {
            ukn::GraphicDevice& gd = ukn::Context::Instance().getGraphicFactory().getGraphicDevice();
            gd.clear(ukn::CM_Color | ukn::CM_Depth, mist::color::Black, 1.f, 0);
            
            font->draw(L"hello world!", 100, 100, ukn::FA_Left);
            font->render();


            for(int i=0; i <1 ; ++i) {
                gd.setWorldMatrix(worldMat);

                if(renderBuffer)
                    gd.renderBuffer(renderBuffer);
            }
        })
        .connectInit([&](ukn::Window*) {
            ukn::GraphicFactory& gf = ukn::Context::Instance().getGraphicFactory();
     
            renderBuffer = ukn::ModelLoader::BuildFromSphere(mist::Sphere(ukn::Vector3(0, 0, 0), 1.5), 20);

            effect = gf.createEffect();
            ukn::ShaderPtr vertexShader = effect->createShader(ukn::ResourceLoader::Instance().loadResource(L"vertex.cg"), 
                    ukn::ShaderDesc(ukn::ST_VertexShader, "VertexProgram", ukn::VertexUVNormal::Format()));
            ukn::ShaderPtr fragmentShader = effect->createShader(ukn::ResourceLoader::Instance().loadResource(L"fragment.cg"), 
                    ukn::ShaderDesc(ukn::ST_FragmentShader, "FragmentProgram"));

            effect->setFragmentShader(fragmentShader);
            effect->setVertexShader(vertexShader);
            
            fragmentShader->setFloatVectorVariable("diffuseColor", ukn::float4(1.f, 1.f, 1.f, 1.f));
            fragmentShader->setFloatVectorVariable("lightDirection", ukn::float4(0.5f, 1.f, 0.5f, 1.f));
            fragmentShader->setFloatVectorVariable("ambientColor", ukn::float4(0.15f, 0.15f, 0.15f, 1.f));
            fragmentShader->setFloatVectorVariable("specularColor", ukn::float4(0.f, 0.f, 0.f, 0.f));

            texture = gf.load2DTexture(mist::ResourceLoader::Instance().loadResource(L"test.png"));
          //  texture = gf.create2DTexture(800, 600, 1, ukn::EF_RGBA8, 0);
            texture2 = gf.load2DTexture(mist::ResourceLoader::Instance().loadResource(L"pic0053.png"));

            effect->getFragmentShader()->setTextureVariable("lightmap", texture2);

            camController = new ukn::FpsCameraController();
            ukn::Viewport& vp = gf.getGraphicDevice().getCurrFrameBuffer()->getViewport();
            vp.camera->setViewParams(ukn::Vector3(0, 0, -3), ukn::Vector3(0, 0, 1));

            vertexShader->setFloatVectorVariable("cameraPosition", ukn::Vector4(vp.camera->getEyePos()));
            renderBuffer->setEffect(effect);

       //     camController->attachCamera(vp.camera);

            font = ukn::AssetManager::Instance().load<ukn::Font>(L"consola.ttf");
            font->setStyleProperty(ukn::FSP_Size, 20);
        })
        .run();
    
    return 0;
}

