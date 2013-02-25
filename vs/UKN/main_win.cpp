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
#include "UKN/RasterizerStateObject.h"
#include "UKN/BlendStateObject.h"
#include "UKN/SamplerStateObject.h"

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
    ukn::CompositeRenderTarget* renderTargetFinal;
    ukn::SkyboxPtr skybox;
    ukn::TerrianPtr terrian;
    ukn::GraphicContextPtr context;
    ukn::RasterizerStatePtr previousRasterizerState;
    ukn::RasterizerStatePtr wireframeRasterizerState;
    ukn::BlendStatePtr lightingBlendState;
    ukn::BlendStatePtr previousBlendState;
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

            if(
                e.state == ukn::input::Release) {
                if(e.key == ukn::input::F1) {
                    if(camController->getAttachedCamera()) {
                        camController->detachCamera();
                    } else {
                        ukn::Viewport& vp = ukn::Context::Instance()
                                                .getGraphicFactory()
                                                .getGraphicDevice()
                                                .getCurrFrameBuffer()->getViewport();
                        camController->attachCamera(vp.camera);
                    }
                } else if(e.key == ukn::input::F2) {
                    static bool wireframe = false;
                    if(!wireframe) {
                        ukn::Context::Instance().getGraphicFactory().getGraphicDevice().setRasterizerState(wireframeRasterizerState);
                        wireframe = true;
                    } else {
                        ukn::Context::Instance().getGraphicFactory().getGraphicDevice().setRasterizerState(previousRasterizerState);
                        wireframe = false;
                    }
                }
            }
        })
        .connectRender([&](ukn::Window* wnd) {
            ukn::GraphicDevice& gd = ukn::Context::Instance().getGraphicFactory().getGraphicDevice();
            

            renderTarget->attachToRender();

            effect->getPass(4)->begin();
            ukn::SpriteBatch::DefaultObject().drawQuad(ukn::Vector2(-1, 1), ukn::Vector2(1, -1));
            effect->getPass(4)->end();

            
            ukn::Viewport& vp = gd.getCurrFrameBuffer()->getViewport();
            const ukn::Frustum& frustum = vp.camera->getViewFrustum();
            int renderCount = 0;

            gd.clear(ukn::CM_Depth, mist::color::Transparent, 1.f, 0);
            gd.enableDepth(true);
            
            gd.setBlendState(previousBlendState);

            effect->getPass(0)->getVertexShader()->setMatrixVariable("viewMatrix", vp.camera->getViewMatrix());
            effect->getPass(0)->getVertexShader()->setMatrixVariable("projectionMatrix", vp.camera->getProjMatrix());
            effect->getPass(0)->getVertexShader()->setMatrixVariable("worldMatrix", ukn::Matrix4());
            
            effect->getPass(1)->getFragmentShader()->setMatrixVariable("viewMatrix", vp.camera->getViewMatrix());
            effect->getPass(1)->getFragmentShader()->setMatrixVariable("projectionMatrix", vp.camera->getProjMatrix());

            if(terrian) {
                effect->getPass(0)->getFragmentShader()->setTextureVariable("tex", texture);
                       
                effect->getPass(0)->begin();
                terrian->render();            
                effect->getPass(0)->end();
            }
            
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
            gd.clear(ukn::CM_Color, mist::color::Transparent, 1.f, 0);
            
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
            
            
            gd.setBlendState(lightingBlendState);
            {
                deferredFragmentShader->setFloatVectorVariable("lightDirection", ukn::float4(0, -1, 0, 1));
                deferredFragmentShader->setFloatVectorVariable("lightColor", ukn::float4(1, 1, 1, 1));
            
                effect->getPass(1)->begin();
                ukn::SpriteBatch::DefaultObject().drawQuad(ukn::Vector2(-1, 1), ukn::Vector2(1, -1));
                effect->getPass(1)->end();
            /*
                deferredFragmentShader->setFloatVectorVariable("lightDirection", ukn::float4(-1, 0, 0, 1));
                deferredFragmentShader->setFloatVectorVariable("lightColor", ukn::float4(0, 0, 1, 1));
            
                effect->getPass(1)->begin();
                ukn::SpriteBatch::DefaultObject().drawQuad(ukn::Vector2(-1, 1), ukn::Vector2(1, -1));
                effect->getPass(1)->end();

                deferredFragmentShader->setFloatVectorVariable("lightDirection", ukn::float4(1, 0, 0, 1));
                deferredFragmentShader->setFloatVectorVariable("lightColor", ukn::float4(1, 0, 0, 1));
        
                effect->getPass(1)->begin();
                ukn::SpriteBatch::DefaultObject().drawQuad(ukn::Vector2(-1, 1), ukn::Vector2(1, -1));
                effect->getPass(1)->end();*/
            }

            ukn::ShaderPtr deferredVertexShader = effect->getPass(3)->getVertexShader();
            deferredFragmentShader = effect->getPass(3)->getFragmentShader();
            
            deferredFragmentShader->setFloatVectorVariable("cameraPosition", ukn::Vector4(vp.camera->getEyePos()));
            deferredVertexShader->setMatrixVariable("viewMatrix", vp.camera->getViewMatrix());
            deferredVertexShader->setMatrixVariable("projectionMatrix", vp.camera->getProjMatrix());
            
            deferredFragmentShader->setMatrixVariable("viewMatrix", vp.camera->getViewMatrix());
            deferredFragmentShader->setMatrixVariable("projectionMatrix", vp.camera->getProjMatrix());
            
            deferredFragmentShader->setTextureVariable("colorMap", 
                                                         renderTarget->getTargetTexture(ukn::ATT_Color0));
            deferredFragmentShader->setTextureVariable("normalMap", 
                                                         renderTarget->getTargetTexture(ukn::ATT_Color1));
            deferredFragmentShader->setTextureVariable("depthMap", 
                                                         renderTarget->getTargetTexture(ukn::ATT_Color2));
            
            {
                for(int i=1; i<=18; ++i) {
                    float radius = 12;
                    ukn::float4 position = ukn::float4(sin(i * ukn::d_pi / 18 * 2 + r) * 10, 10, cos(i * ukn::d_pi / 18 * 2 + r) * 20, 1);
                    ukn::Color c = ukn::Color(i * sin(r), i * cos(r), i * 0.1, 1);

                    deferredFragmentShader->setFloatVariable("lightIntensity", 1, ukn::float1(2).value);
                    deferredFragmentShader->setFloatVariable("lightRadius", 1, ukn::float1(radius).value);
                    deferredFragmentShader->setFloatVectorVariable("lightPosition", position);
                    deferredFragmentShader->setFloatVectorVariable("lightColor", c.c);
                    
                    ukn::Matrix4 sphereWorld = ukn::Matrix4::ScaleMat(radius, radius, radius) * ukn::Matrix4::TransMat(position[0], position[1], position[2]);
                    
                    deferredVertexShader->setMatrixVariable("worldMatrix", sphereWorld);
                    
                    effect->getPass(3)->begin();
                    gd.renderBuffer(renderBuffer);
                    effect->getPass(3)->end();
                }
            }
            
            renderTargetLightMap->detachFromRender();

            gd.setBlendState(previousBlendState);

            {
            renderTargetFinal->attachToRender();

            effect->getPass(2)->getFragmentShader()->setTextureVariable("colorMap", 
                                                         renderTarget->getTargetTexture(ukn::ATT_Color0));
            effect->getPass(2)->getFragmentShader()->setTextureVariable("lightMap", 
                                                         renderTargetLightMap->getTargetTexture(ukn::ATT_Color0));                                                    

            effect->getPass(2)->begin();
            ukn::SpriteBatch::DefaultObject().drawQuad(ukn::Vector2(-1, 1), ukn::Vector2(1, -1));
            effect->getPass(2)->end();

            renderTargetFinal->detachFromRender();
            }

            gd.clear(ukn::CM_Color | ukn::CM_Depth, mist::color::Black, 1.f, 0);
           
            ukn::SpriteBatch& sb = ukn::SpriteBatch::DefaultObject();
            sb.begin(ukn::SBB_None, ukn::SBS_Deffered, ukn::Matrix4());
            sb.draw(renderTarget->getTargetTexture(ukn::ATT_Color0), 
                    ukn::Rectangle(0, 0, wnd->width() / 2, wnd->height() / 2, true));
            sb.draw(renderTarget->getTargetTexture(ukn::ATT_Color1), 
                    ukn::Rectangle(0, wnd->height() / 2, wnd->width() / 2, wnd->height() / 2, true));
            sb.draw(renderTargetFinal->getTargetTexture(ukn::ATT_Color0), 
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
     
            renderBuffer = ukn::ModelLoader::BuildFromSphere(mist::Sphere(ukn::Vector3(0, 0, 0), 1.0), 10);

            texture = gf.load2DTexture(mist::ResourceLoader::Instance().loadResource(L"dirt01.dds"));
            texture2 = gf.load2DTexture(mist::ResourceLoader::Instance().loadResource(L"grass.dds"));

            camController = new ukn::FpsCameraController();
            ukn::Viewport& vp = gf.getGraphicDevice().getCurrFrameBuffer()->getViewport();
            vp.camera->setViewParams(ukn::Vector3(0, 5, -30), ukn::Vector3(0, 0, 1));

            camController->attachCamera(vp.camera);

            font = ukn::Font::Create(L"Consola.ttf", 20);

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

          
            ukn::ModelLoader::ModelDataPtr dragonModel = ukn::ModelLoader::LoadFromPly(
                ukn::ResourceLoader::Instance().loadResource(L"dragon_recon/dragon_vrip_res2.ply"));
          
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

            ukn::RasterizerStateDesc wireframeDesc;
            wireframeDesc.cullface = ukn::RSP_CullBack;
            wireframeDesc.fillmode = ukn::RSP_FillWireFrame;
            wireframeDesc.frontface = ukn::RSP_FrontFaceClockwise;

            wireframeRasterizerState = gf.createRasterizerStateObject(wireframeDesc);
            previousRasterizerState = gf.getGraphicDevice().getRasterizerState(); 

            ukn::BlendStateDesc blendDesc;
            blendDesc.blend_state.enabled = true;
            blendDesc.blend_state.src = ukn::RSP_BlendFuncOne;
            blendDesc.blend_state.dst = ukn::RSP_BlendFuncOne;
            blendDesc.blend_state.op = ukn::RSP_BlendOpAdd;
            blendDesc.blend_state.src_alpha = ukn::RSP_BlendFuncOne;
            blendDesc.blend_state.dst_alpha = ukn::RSP_BlendFuncOne;
            blendDesc.blend_state.op_alpha = ukn::RSP_BlendOpAdd;

            lightingBlendState = gf.createBlendStateObject(blendDesc);
            previousBlendState = gf.getGraphicDevice().getBlendState();
        })
        .run();
    
    return 0;
}

