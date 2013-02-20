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
#include "UKN/RenderTarget.h"
#include "UKN/CameraController.h"
#include "UKN/Skybox.h"
#include "UKN/Terrian.h"

#include "UKN/tmx/TMXTiledMap.h"

#include "mist/SysUtil.h"
#include "mist/TimeUtil.h"
#include "mist/RandomUtil.h"
#include "mist/Convert.h"
#include "mist/TimeUtil.h"

#include <vector>
#include <map>

#include "mist/Query.h"

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

    ukn::RenderBufferPtr renderBuffer;
    ukn::TexturePtr texture;
    ukn::TexturePtr texture2;
    ukn::Matrix4 worldMat;
    ukn::EffectPtr effect;
    ukn::EffectPtr deferredEffect;
    ukn::CameraController* camController;
    ukn::FontPtr font;
    ukn::CompositeRenderTarget* renderTarget;
    ukn::CompositeRenderTarget* renderTargetLightMap;
    ukn::SkyboxPtr skybox;
    ukn::TerrianPtr terrian;

    ukn::RenderBufferPtr renderBufferDeferred;
    ukn::ShaderPtr deferredFragmentShader;

    ukn::Vector3 positions[5];
    
    ukn::float4 colors[5];
    for(int i=0; i<5; ++i) {
        positions[i] =  ukn::Vector3(mist::Random::RandomFloat(-10, 10),
                                   mist::Random::RandomFloat(5, 20),
                                   mist::Random::RandomFloat(-5, 5));
               
        colors[i] =ukn::float4(ukn::Random::RandomFloat(0, 1), 
                                ukn::Random::RandomFloat(0, 1), 
                                ukn::Random::RandomFloat(0, 1), 1.f);
    }

    
#ifndef MIST_OS_WINDOWS
    ukn::GraphicFactoryPtr factory;
    ukn::CreateGraphicFactory(factory);
    ukn::Context::Instance().registerGraphicFactory(factory);
#endif

    float r = 0;
     ukn::AppLauncher(L"Windows Test")
        .create(
                ukn::ContextCfg::Default()
                  .width(800)
                  .height(600)
                  .sampleCount(1)
                  .showMouse(true)
                  .isFullScreen(false)
                  .graphicFactoryName(L"D3D11Plugin.dll")
                  .fps(60)
               )
        .connectUpdate([](ukn::Window* ) {

        })
        .connectKey([&](ukn::Window* window, ukn::input::KeyEventArgs& e) {
            if(e.key == ukn::input::Escape)
                ukn::Context::Instance().getApp().terminate();

            if(e.key == ukn::input::F1 &&
                e.state == ukn::input::Release) {
                if(camController->getAttachedCamera()) {
                    camController->detachCamera();
                } else {
                    ukn::Viewport& vp = ukn::Context::Instance()
                                            .getGraphicFactory()
                                            .getGraphicDevice()
                                            .getCurrFrameBuffer()->getViewport();
                    camController->attachCamera(vp.camera);
                }
            }
        })
        .connectRender([&](ukn::Window* wnd) {
            ukn::GraphicDevice& gd = ukn::Context::Instance().getGraphicFactory().getGraphicDevice();
            renderTarget->attachToRender();
            
            ukn::Viewport& vp = gd.getCurrFrameBuffer()->getViewport();
            const ukn::Frustum& frustum = vp.camera->getViewFrustum();
            int renderCount = 0;

            gd.clear(ukn::CM_Color | ukn::CM_Depth, mist::color::Black, 1.f, 0);
            gd.enableDepth(true);

            effect->getPass(0)->getVertexShader()->setMatrixVariable("viewMatrix", vp.camera->getViewMatrix());
            effect->getPass(0)->getVertexShader()->setMatrixVariable("projectionMatrix", vp.camera->getProjMatrix());
            effect->getPass(0)->getVertexShader()->setMatrixVariable("worldMatrix", ukn::Matrix4());
            
            deferredEffect->getPass(0)->getFragmentShader()->setMatrixVariable("viewMatrix", vp.camera->getViewMatrix());
            deferredEffect->getPass(0)->getFragmentShader()->setMatrixVariable("projectionMatrix", vp.camera->getProjMatrix());

            if(terrian) {
                effect->getPass(0)->getFragmentShader()->setTextureVariable("tex", texture);
                effect->getPass(0)->getFragmentShader()->setTextureVariable("tex", texture);
                       
                effect->getPass(0)->begin();
                terrian->render();            
                effect->getPass(0)->end();
            }
            
            gd.bindTexture(texture2);
            for(int i=0; i<5; ++i) {
                if(frustum.isSphereVisible(mist::Sphere(positions[i], 2.5)) != ukn::Frustum::No) {
                    effect->getPass(0)->getVertexShader()->setMatrixVariable("worldMatrix", ukn::Matrix4::TransMat(positions[i].x(), positions[i].y(), positions[i].z()));
                    effect->getPass(0)->getFragmentShader()->setTextureVariable("tex", texture2);
                 
                    effect->getPass(0)->begin();

                    if(renderBuffer)
                        gd.renderBuffer(renderBuffer);
                    
                    effect->getPass(0)->end();

                    renderCount++;
                }
            }    

            effect->getPass(0)->end();

            renderTarget->detachFromRender();

       //     if(skybox) {
       //         skybox->render();
       //     }

            r += 0.01;
           
            font->draw(gd.description().c_str(), 0, 30, ukn::FA_Left, ukn::color::Lightgreen);
            font->draw(ukn::SystemInformation::GetOSVersion().c_str(), 0, 60, ukn::FA_Left, ukn::color::Lightgreen);
            font->draw((L"Fps: " + mist::Convert::ToString(mist::FrameCounter::Instance().getCurrentFps())).c_str(), 
                        0, 
                        0, 
                        ukn::FA_Left,
                        ukn::color::Black);

            font->draw(ukn::Convert::ToString(renderCount).c_str(), 0, 90, ukn::FA_Left, ukn::color::Red);
            font->draw(ukn::Convert::ToString(terrian->getDrawCount()).c_str(), 0, 120, ukn::FA_Left, ukn::color::Red);
              
           // font->render();
            
            renderTargetLightMap->attachToRender();
            gd.clear(ukn::CM_Color, mist::color::Blue, 1.f, 0);
            
            
            deferredFragmentShader->setMatrixVariable("viewMatrix", vp.camera->getViewMatrix());
            deferredFragmentShader->setMatrixVariable("projectionMatrix", vp.camera->getProjMatrix());
            deferredFragmentShader->setTextureVariable("colorMap", 
                                                         renderTarget->getTargetTexture(ukn::ATT_Color0));
            deferredFragmentShader->setTextureVariable("normalMap", 
                                                         renderTarget->getTargetTexture(ukn::ATT_Color1));
            deferredFragmentShader->setTextureVariable("depthMap", 
                                                         renderTarget->getTargetTexture(ukn::ATT_Color2));
            deferredFragmentShader->setFloatVectorVariable("cameraPosition", ukn::Vector4(vp.camera->getEyePos()));
            
            deferredFragmentShader->setFloatVectorVariable("lightDirection", ukn::float4(sin(r), cos(r), 0, 1));
            deferredFragmentShader->setFloatVectorVariable("lightColor", ukn::float4(1, 1, 1, 1));
           
            deferredEffect->getPass(0)->begin();
            gd.renderBuffer(renderBufferDeferred);
            deferredEffect->getPass(0)->end();
            
            /*
            deferredFragmentShader->setFloatVectorVariable("lightDirection", ukn::float4(sin(r), 0, cos(r), 1));
            deferredFragmentShader->setFloatVectorVariable("lightColor", ukn::float4(1, 1, 1, 1));
            
            deferredEffect->getPass(0)->begin();
            gd.renderBuffer(renderBufferDeferred);
            deferredEffect->getPass(0)->end();

            deferredFragmentShader->setFloatVectorVariable("lightDirection", ukn::float4(0, cos(r), sin(r), 1));
            deferredFragmentShader->setFloatVectorVariable("lightColor", ukn::float4(1, 1, 1, 1));
            
            deferredEffect->getPass(0)->begin();
            gd.renderBuffer(renderBufferDeferred);
            deferredEffect->getPass(0)->end();
            */
            renderTargetLightMap->detachFromRender();

            gd.clear(ukn::CM_Color | ukn::CM_Depth, mist::color::Black, 1.f, 0);
           
            ukn::SpriteBatch& sb = ukn::SpriteBatch::DefaultObject();
            sb.begin(ukn::SBB_None, ukn::SBS_Deffered, ukn::Matrix4());
            sb.draw(renderTarget->getTargetTexture(ukn::ATT_Color0), 
                    ukn::Rectangle(0, 0, wnd->width() / 2, wnd->height() / 2, true));
            sb.draw(renderTarget->getTargetTexture(ukn::ATT_Color1), 
                    ukn::Rectangle(0, wnd->height() / 2, wnd->width() / 2, wnd->height() / 2, true));
            sb.draw(renderTarget->getTargetTexture(ukn::ATT_Color2), 
                    ukn::Rectangle(wnd->width() / 2, wnd->height() / 2, wnd->width() / 2, wnd->height() / 2, true));
            sb.draw(renderTargetLightMap->getTargetTexture(ukn::ATT_Color0), 
                    ukn::Rectangle(wnd->width() / 2, 0, wnd->width() / 2, wnd->height() / 2, true));
            
            sb.end();
        })
        .connectInit([&](ukn::Window*) {
            ukn::GraphicFactory& gf = ukn::Context::Instance().getGraphicFactory();
     
            renderBuffer = ukn::ModelLoader::BuildFromSphere(mist::Sphere(ukn::Vector3(0, 0, 0), 2.5), 20);

            effect = gf.createEffect();
            ukn::ShaderPtr vertexShader = effect->createShader(ukn::ResourceLoader::Instance().loadResource(L"vertex_deferred.cg"), 
                    ukn::ShaderDesc(ukn::ST_VertexShader, "VertexProgram", ukn::VertexUVNormal::Format()));
            ukn::ShaderPtr fragmentShader = effect->createShader(ukn::ResourceLoader::Instance().loadResource(L"fragment_deferred.cg"), 
                    ukn::ShaderDesc(ukn::ST_FragmentShader, "FragmentProgram"));
            ukn::EffectPassPtr pass0 = effect->appendPass();

            pass0->setFragmentShader(fragmentShader);
            pass0->setVertexShader(vertexShader);
            pass0->setVertexFormat(ukn::VertexUVNormal::Format());
            
            texture = gf.load2DTexture(mist::ResourceLoader::Instance().loadResource(L"dirt01.dds"));
            texture2 = gf.load2DTexture(mist::ResourceLoader::Instance().loadResource(L"grass.dds"));

            camController = new ukn::FpsCameraController();
            ukn::Viewport& vp = gf.getGraphicDevice().getCurrFrameBuffer()->getViewport();
            vp.camera->setViewParams(ukn::Vector3(0, 5, -30), ukn::Vector3(0, 0, 1));

            camController->attachCamera(vp.camera);

            font = ukn::AssetManager::Instance().load<ukn::Font>(L"Consola.ttf");
            if(font)
                font->setStyleProperty(ukn::FSP_Size, 20);

            renderTarget = new ukn::CompositeRenderTarget();
            renderTarget->attach(ukn::ATT_Color0,
                                    new ukn::RenderTarget(800,
                                                          600,
                                                          1,
                                                          ukn::EF_RGBA8));
            renderTarget->attach(ukn::ATT_Color1,
                                    new ukn::RenderTarget(800,
                                                          600,
                                                          1,
                                                          ukn::EF_RGBA8));
            renderTarget->attach(ukn::ATT_Color2,
                                    new ukn::RenderTarget(800,
                                                          600,
                                                          1,
                                                          ukn::EF_Float));
            renderTarget->attachCamera(vp.camera);

            renderTargetLightMap = new ukn::CompositeRenderTarget();
            renderTargetLightMap->attach(ukn::ATT_Color0,
                                    new ukn::RenderTarget(800,
                                                          600,
                                                          1,
                                                          ukn::EF_RGBA8));
            
            renderTargetLightMap->attachCamera(vp.camera);

            skybox = new ukn::Skybox();
            if(!skybox->load(mist::ResourceLoader::Instance().loadResource(L"skyboxsun25degtest.png"))) {
                mist::log_error(L"unable to load skybox");
            }

            ukn::GridTerrianLightening* t = new ukn::GridTerrianLightening();
            (*t).position(ukn::float3(-50, 0, -50))
                .noise(10)
                .noiseWeight(5)
                .size(ukn::float2(100, 100));
            if(t->build()) {
                t->texture(gf.load2DTexture(ukn::ResourceLoader::Instance().loadResource(L"dirt01.dds")));

                terrian = t;
                
            } else {
                ukn::log_error(L"unable to build terrian");
            }

            renderBufferDeferred = gf.createRenderBuffer();

            struct {
                ukn::float3 position;
                ukn::float2 uv;
            } vertices[6];

            vertices[0].position = ukn::float3(-1, 1, 0); vertices[0].uv = ukn::float2(0, 0); 
            vertices[1].position = ukn::float3(1.0, 1.0, 0); vertices[1].uv = ukn::float2(1, 0);
            vertices[2].position = ukn::float3(1.0, -1.0, 0); vertices[2].uv = ukn::float2(1, 1);
            vertices[3].position = ukn::float3(1.0, -1.0, 0); vertices[3].uv = ukn::float2(1, 1);
            vertices[4].position = ukn::float3(-1, -1.0, 0); vertices[4].uv = ukn::float2(0, 1);
            vertices[5].position = ukn::float3(-1, 1.0, 0); vertices[5].uv = ukn::float2(0, 0);

            ukn::vertex_elements_type format = ukn::VertexElementsBuilder()
                                           .add(ukn::VertexElement(ukn::VU_Position,
                                                                   ukn::EF_Float3,
                                                                   0))
                                           .add(ukn::VertexElement(ukn::VU_UV,
                                                                   ukn::EF_Float2,
                                                                   0))
                                           .to_vector();
            ukn::GraphicBufferPtr vertexBuffer = gf.createVertexBuffer(ukn::GraphicBuffer::None,
                                                                       ukn::GraphicBuffer::Static,
                                                                       6,
                                                                       vertices,
                                                                       format);
            renderBufferDeferred->bindVertexStream(vertexBuffer, format);

            deferredEffect = gf.createEffect();
            ukn::ShaderPtr deferredVertexShader = effect->createShader(ukn::ResourceLoader::Instance().loadResource(L"vertex_deferred_composite.cg"), 
                    ukn::ShaderDesc(ukn::ST_VertexShader, "VertexProgram", format));
            deferredFragmentShader = effect->createShader(ukn::ResourceLoader::Instance().loadResource(L"fragment_deferred_composite.cg"), 
                    ukn::ShaderDesc(ukn::ST_FragmentShader, "FragmentProgram"));

            ukn::EffectPassPtr deferredPass0 = deferredEffect->appendPass();
            deferredPass0->setFragmentShader(deferredFragmentShader);
            deferredPass0->setVertexShader(deferredVertexShader);
            deferredPass0->setVertexFormat(format);

        })
        .run();
    
    return 0;
}

