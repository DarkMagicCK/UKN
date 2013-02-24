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
#include "UKN/GraphicContext.h"

#include "UKN/tmx/TMXTiledMap.h"

#include "mist/SysUtil.h"
#include "mist/TimeUtil.h"
#include "mist/RandomUtil.h"
#include "mist/Convert.h"
#include "mist/TimeUtil.h"
#include "mist/Query.h"

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

    ukn::RenderBufferPtr renderBuffer;
    ukn::TexturePtr texture;
    ukn::TexturePtr texture2;
    ukn::Matrix4 worldMat;
    ukn::EffectPtr effect;
    ukn::CameraController* camController;
    ukn::FontPtr font;
    ukn::CompositeRenderTarget* renderTarget;
    ukn::CompositeRenderTarget* renderTargetLightMap;
    ukn::SkyboxPtr skybox;
    ukn::TerrianPtr terrian;
    ukn::GraphicContextPtr context;

    ukn::RenderBufferPtr dragonBuffer;

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

    ukn::uint32 count;
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
            
            effect->getPass(1)->getFragmentShader()->setMatrixVariable("viewMatrix", vp.camera->getViewMatrix());
            effect->getPass(1)->getFragmentShader()->setMatrixVariable("projectionMatrix", vp.camera->getProjMatrix());

            if(terrian) {
                effect->getPass(0)->getFragmentShader()->setTextureVariable("tex", texture);
                       
                effect->getPass(0)->begin();
      //          terrian->render();            
                effect->getPass(0)->end();
            }
            
           /* gd.bindTexture(texture2);
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
            */
            gd.bindTexture(texture2);
            effect->getPass(0)->getVertexShader()->setMatrixVariable("worldMatrix", ukn::Matrix4::ScaleMat(100, 100, 100));
                    
            effect->getPass(0)->begin();
            gd.renderBuffer(dragonBuffer);
            effect->getPass(0)->end();

            renderTarget->detachFromRender();

       //     if(skybox) {
       //         skybox->render();
       //     }

            r += 0.01;
           
            
            renderTargetLightMap->attachToRender();
            gd.clear(ukn::CM_Color, mist::color::Blue, 1.f, 0);
            
            ukn::ShaderPtr deferredFragmentShader = effect->getPass(1)->getFragmentShader();
            
            deferredFragmentShader->setMatrixVariable("viewMatrix", vp.camera->getViewMatrix());
            deferredFragmentShader->setMatrixVariable("projectionMatrix", vp.camera->getProjMatrix());
            deferredFragmentShader->setTextureVariable("colorMap", 
                                                         renderTarget->getTargetTexture(ukn::ATT_Color0));
            deferredFragmentShader->setTextureVariable("normalMap", 
                                                         renderTarget->getTargetTexture(ukn::ATT_Color1));
            deferredFragmentShader->setTextureVariable("depthMap", 
                                                         renderTarget->getTargetTexture(ukn::ATT_Color2));
            deferredFragmentShader->setFloatVectorVariable("cameraPosition", ukn::Vector4(vp.camera->getEyePos()));
            
            deferredFragmentShader->setFloatVectorVariable("lightDirection", ukn::float4(sin(r), -1, 0, 1));
            deferredFragmentShader->setFloatVectorVariable("lightColor", ukn::float4(1, 1, 1, 1));
           
            effect->getPass(1)->begin();
            ukn::SpriteBatch::DefaultObject().drawQuad(ukn::Vector2(-1, 1), ukn::Vector2(1, -1));
            effect->getPass(1)->end();
            
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
         //   sb.draw(font->getTexturePlacement(0)->texture, ukn::Vector2(0, 0));
            sb.end();

            if(font) {
                font->begin();
                font->draw(gd.description().c_str(), 0, 30, ukn::FA_Left, ukn::color::Lightgreen);
                font->draw(ukn::SystemInformation::GetOSVersion().c_str(), 0, 60, ukn::FA_Left, ukn::color::Lightgreen);
                font->draw((L"Fps: " + mist::Convert::ToString(mist::FrameCounter::Instance().getCurrentFps())).c_str(), 
                            0, 
                            0, 
                            ukn::FA_Left,
                            ukn::color::Black);

                font->draw(ukn::Convert::ToString(renderCount).c_str(), 0, 90, ukn::FA_Left, ukn::color::Red);
                font->draw(ukn::Convert::ToString(count).c_str(), 0, 120, ukn::FA_Left, ukn::color::Red);
              
                font->end();
            }
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

            font = ukn::Font::Create(L"Consola.ttf", 20);

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
            renderTarget->attach(ukn::ATT_DepthStencil,
                                    new ukn::RenderTarget(800,
                                                          600,
                                                          ukn::EF_D16));
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

            context = new ukn::GraphicContext();
            ukn::ShaderPtr deferredVertexShader = effect->createShader(ukn::ResourceLoader::Instance().loadResource(L"vertex_deferred_composite.cg"), 
                    ukn::ShaderDesc(ukn::ST_VertexShader, "VertexProgram", ukn::Vertex2D::Format()));
            ukn::ShaderPtr deferredFragmentShader = effect->createShader(ukn::ResourceLoader::Instance().loadResource(L"fragment_deferred_composite.cg"), 
                    ukn::ShaderDesc(ukn::ST_FragmentShader, "FragmentProgram"));

            ukn::EffectPassPtr deferredPass = effect->appendPass();
            deferredPass->setFragmentShader(deferredFragmentShader);
            deferredPass->setVertexShader(deferredVertexShader);
            deferredPass->setVertexFormat(ukn::Vertex2D::Format());

            ukn::ModelLoader::ModelDataPtr dragonModel = ukn::ModelLoader::LoadFromPly(
                ukn::ResourceLoader::Instance().loadResource(L"dragon_recon/dragon_vrip_res4.ply"));

            if(dragonModel) {
                ukn::ModelLoader::ModelData* pdragonModel = dragonModel.get();
                ukn::VertexUVNormal* vertices = mist_malloc_t(ukn::VertexUVNormal, pdragonModel->indices.size());
                for(size_t i=0; i<dragonModel->indices.size(); ++i) {
                    vertices[i].position = pdragonModel->position[pdragonModel->indices[i]];
                    if(dragonModel->uv.size() > 0)
                        vertices[i].uv = pdragonModel->uv[pdragonModel->indices[i]];
                    else
                        vertices[i].uv = ukn::float2(0, 0);
                }
                // calculate normals
                ukn::uint32 index = 0;
                for(size_t i=0; i<dragonModel->indices.size() / 3; ++i) {
                    ukn::VertexUVNormal& p1 = vertices[index];
                    ukn::VertexUVNormal& p2 = vertices[index+1];
                    ukn::VertexUVNormal& p3 = vertices[index+2];

                    ukn::Vector3 u = p2.position - p1.position;
                    ukn::Vector3 v = p3.position - p1.position;
                    ukn::float3 normal = u.cross(v);

                    p1.normal = p2.normal = p3.normal = normal;
                    index += 3;
                }
                count = dragonModel->indices.size();
                ukn::GraphicBufferPtr vertexBuffer = gf.createVertexBuffer(ukn::GraphicBuffer::None,
                                                                           ukn::GraphicBuffer::Static,
                                                                           dragonModel->indices.size(),
                                                                           vertices,
                                                                           ukn::VertexUVNormal::Format());
                ukn::GraphicBufferPtr indexBuffer = gf.createIndexBuffer(ukn::GraphicBuffer::None,
                                                                         ukn::GraphicBuffer::Static,
                                                                         dragonModel->indices.size(),
                                                                         &dragonModel->indices[0]);
                dragonBuffer = gf.createRenderBuffer();
                dragonBuffer->bindVertexStream(vertexBuffer, ukn::VertexUVNormal::Format());
                dragonBuffer->bindIndexStream(indexBuffer);
                dragonBuffer->setRenderMode(ukn::RM_Triangle);

                mist::mist_free(vertices);
            }

        })
        .run();
    
    return 0;
}

