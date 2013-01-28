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

    ukn::RenderBufferPtr renderBuffer;
    ukn::TexturePtr texture;
    ukn::Matrix4 worldMat;
    ukn::EffectPtr effect;
    ukn::CameraController* camController;

    ukn::Vector3 positions[5];
    for(int i=0; i<5; ++i) {
        positions[i] =  ukn::Vector3(mist::Random::RandomFloat(-1, 1),
                                   mist::Random::RandomFloat(-1, 1),
                                   0);
               
    }

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
        .connectRender([&](ukn::Window*) {
            ukn::GraphicDevice& gd = ukn::Context::Instance().getGraphicFactory().getGraphicDevice();
            gd.clear(ukn::CM_Color | ukn::CM_Depth, mist::color::Black, 1.f, 0);

            worldMat.rotate(0, 0.01 * ukn::d_pi, 0);
            if(texture)
                gd.bindTexture(texture);
            for(int i=0; i <1 ; ++i) {
                worldMat = ukn::Matrix4::TransMat(0,
                                                  0,
                                                  0); 
                gd.setWorldMatrix(worldMat);

                if(renderBuffer)
                    gd.renderBuffer(renderBuffer);
            }
        })
        .connectInit([&](ukn::Window*) {
            ukn::GraphicFactory& gf = ukn::Context::Instance().getGraphicFactory();
     
            renderBuffer = ukn::ModelLoader::BuildFromSphere(mist::Sphere(ukn::Vector3(0, 0, 0), 0.5), 20);

            effect = gf.createEffect();
            ukn::ShaderPtr vertexShader = effect->createShader(ukn::ResourceLoader::Instance().loadResource(L"vertex.cg"), 
                    ukn::ShaderDesc(ukn::ST_VertexShader, "VertexProgram", "", ukn::VertexUVNormal::Format()));
            ukn::ShaderPtr fragmentShader = effect->createShader(ukn::ResourceLoader::Instance().loadResource(L"fragment.cg"), 
                    ukn::ShaderDesc(ukn::ST_FragmentShader, "FragmentProgram", ""));

            effect->setFragmentShader(fragmentShader);
            effect->setVertexShader(vertexShader);
            
            fragmentShader->setFloatVectorVariable("diffuseColor", ukn::float4(1.f, 1.f, 1.f, 1.f));
            fragmentShader->setFloatVectorVariable("lightDirection", ukn::float4(0.5f, 1.f, 0.5f, 1.f));
            fragmentShader->setFloatVectorVariable("ambientColor", ukn::float4(0.15f, 0.15f, 0.15f, 1.f));
            fragmentShader->setFloatVectorVariable("specularColor", ukn::float4(0.f, 0.f, 0.f, 0.f));

            texture = gf.create2DTexture(800, 600, 0, ukn::EF_RGBA8, 0);
            if(!texture) {
                mist::MessageBox::Show(L"error loading texture", L"Error", mist::MBO_OK);
            } else {
                ukn::GraphicDevice& gd = gf.getGraphicDevice();
                
            }

            camController = new ukn::FpsCameraController();
            ukn::Viewport& vp = gf.getGraphicDevice().getCurrFrameBuffer()->getViewport();
            vp.camera->setViewParams(ukn::Vector3(0, 0, -10), ukn::Vector3(0, 0, 1));

            vertexShader->setFloatVectorVariable("cameraPosition", ukn::Vector4(vp.camera->getEyePos()));
            
            renderBuffer->setEffect(effect);

            camController->attachCamera(vp.camera);
        })
        .run();
    
    return 0;
}

