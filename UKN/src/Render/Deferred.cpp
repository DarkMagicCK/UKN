#include "UKN/Deferred.h"
#include "UKN/RenderTarget.h"
#include "UKN/BlendStateObject.h"
#include "UKN/RasterizerStateObject.h"
#include "UKN/SamplerStateObject.h"
#include "UKN/GraphicDevice.h"
#include "UKN/GraphicFactory.h"
#include "UKN/Context.h"
#include "UKN/App.h"
#include "UKN/Shader.h"
#include "UKN/LightManager.h"
#include "UKN/SpriteBatch.h"
#include "UKN/DepthStencilStateObject.h"
#include "UKN/SceneManager.h"
#include "UKN/SceneObject.h"
#include "UKN/SceneObjectWrapper.h"
#include "UKN/Model.h"

#include "mist/Profiler.h"

namespace ukn {

    DeferredRenderer::DeferredRenderer(const float2& size):
    mSize(size),
    mBegan(false) {
        if(!this->init()) {
            log_error(L"DeferredRenderer::DeferredRenderer: error initializing deferred renderer");
        }
    }

    DeferredRenderer::~DeferredRenderer() {

    }

    const EffectPtr& DeferredRenderer::getEffect() const {
        return mEffect;
    }

    const CompositeRenderTargetPtr& DeferredRenderer::getGBufferRT() const {
        return mGBufferRT;
    }

    const CompositeRenderTargetPtr& DeferredRenderer::getLightMapRT() const {
        return mLightMapRT;
    }

    const CompositeRenderTargetPtr& DeferredRenderer::getCompositeRT() const {
        return mCompositeRT;
    }

    float2 DeferredRenderer::size() const {
        return mSize;
    }

    bool DeferredRenderer::init() {
        GraphicFactory& gf = Context::Instance().getGraphicFactory();

        if(mSize[0] == 0 && mSize[1] == 0) {
            Window& wnd = Context::Instance().getApp().getWindow();
            mSize = float2((float)wnd.width(), (float)wnd.height());
        }
        /* render targets */
        {
            mGBufferRT = new ukn::CompositeRenderTarget();
            mGBufferRT->attach(ukn::ATT_Color0,
                               MakeSharedPtr<ukn::RenderTarget>(mSize[0],
                                                          mSize[1],
                                                          1,
                                                          ukn::EF_RGBA8));
            mGBufferRT->attach(ukn::ATT_Color1,
                               MakeSharedPtr<ukn::RenderTarget>(mSize[0],
                                                          mSize[1],
                                                          1,
                                                          ukn::EF_RGBA8));
            mGBufferRT->attach(ukn::ATT_Color2,
                               MakeSharedPtr<ukn::RenderTarget>(mSize[0],
                                                          mSize[1],
                                                          1,
                                                          ukn::EF_Float));
            mGBufferRT->attach(ukn::ATT_DepthStencil,
                               MakeSharedPtr<ukn::RenderTarget>(mSize[0],
                                                          mSize[1],
                                                          ukn::EF_D16));

            
            mLightMapRT = MakeSharedPtr<ukn::CompositeRenderTarget>();
            mLightMapRT->attach(ukn::ATT_Color0,
                                MakeSharedPtr<ukn::RenderTarget>(800,
                                                          600,
                                                          1,
                                                          ukn::EF_RGBA8));
            
            mCompositeRT = new ukn::CompositeRenderTarget();
            mCompositeRT->attach(ukn::ATT_Color0,
                                 MakeSharedPtr<ukn::RenderTarget>(800,
                                                          600,
                                                          1,
                                                          ukn::EF_RGBA8));
            
        }
        /* shaders */
        // temporary stored as file for test
        {
            ukn::ShaderPtr vertexShader;
            ukn::ShaderPtr fragmentShader;
            
            mEffect = gf.createEffect();

            if(mEffect) {
                vertexShader = mEffect->createShader(MIST_LOAD_RESOURCE(L"deferred/clear_vert.cg"), 
                                                     VERTEX_SHADER_DESC("VertexProgram"));
                fragmentShader = mEffect->createShader(MIST_LOAD_RESOURCE(L"deferred/clear_frag.cg"), 
                                                       FRAGMENT_SHADER_DESC("FragmentProgram"));

                mClearTechnique = mEffect->appendTechnique(fragmentShader, vertexShader, ShaderPtr());

                vertexShader = mEffect->createShader(MIST_LOAD_RESOURCE(L"deferred/gbuffer_vert.cg"), 
                                                     VERTEX_SHADER_DESC("VertexProgram"));
                fragmentShader = mEffect->createShader(MIST_LOAD_RESOURCE(L"deferred/gbuffer_frag.cg"), 
                                                       FRAGMENT_SHADER_DESC("FragmentProgram"));
            
                mGBufferTechnique = mEffect->appendTechnique(fragmentShader, vertexShader, ShaderPtr());

                vertexShader = mEffect->createShader(MIST_LOAD_RESOURCE(L"deferred/directionallight_vert.cg"), 
                                                     VERTEX_SHADER_DESC("VertexProgram"));
                fragmentShader = mEffect->createShader(MIST_LOAD_RESOURCE(L"deferred/directionallight_frag.cg"), 
                                                       FRAGMENT_SHADER_DESC("FragmentProgram"));

                mDirectionalLightTechnique = mEffect->appendTechnique(fragmentShader, vertexShader, ShaderPtr());

                vertexShader = mEffect->createShader(MIST_LOAD_RESOURCE(L"deferred/spotlight_vert.cg"), 
                                                     VERTEX_SHADER_DESC("VertexProgram"));
                fragmentShader = mEffect->createShader(MIST_LOAD_RESOURCE(L"deferred/spotlight_frag.cg"), 
                                                       FRAGMENT_SHADER_DESC("FragmentProgram"));

                mSpotLightTechnique = mEffect->appendTechnique(fragmentShader, vertexShader, ShaderPtr());

                vertexShader = mEffect->createShader(MIST_LOAD_RESOURCE(L"deferred/pointlight_vert.cg"), 
                                                     VERTEX_SHADER_DESC("VertexProgram"));
                fragmentShader = mEffect->createShader(MIST_LOAD_RESOURCE(L"deferred/pointlight_frag.cg"), 
                                                       FRAGMENT_SHADER_DESC("FragmentProgram"));

                mPointLightTechnique = mEffect->appendTechnique(fragmentShader, vertexShader, ShaderPtr());


                vertexShader = mEffect->createShader(MIST_LOAD_RESOURCE(L"deferred/composite_vert.cg"), 
                                                     VERTEX_SHADER_DESC("VertexProgram"));
                fragmentShader = mEffect->createShader(MIST_LOAD_RESOURCE(L"deferred/composite_frag.cg"), 
                                                       FRAGMENT_SHADER_DESC("FragmentProgram"));

                mCompositeTechnique = mEffect->appendTechnique(fragmentShader, vertexShader, ShaderPtr());

            }
        }

        /* blend state */
        {
            BlendStateDesc desc;
            desc.blend_state.enabled = true;
            desc.blend_state.src = RSP_BlendFuncOne;
            desc.blend_state.dst = RSP_BlendFuncOne;
            desc.blend_state.op = RSP_BlendOpAdd;
            desc.blend_state.src_alpha = RSP_BlendFuncOne;
            desc.blend_state.dst_alpha = RSP_BlendFuncOne;
            desc.blend_state.op_alpha = RSP_BlendOpAdd;
            desc.blend_state.write_mask = 0x0f;

            mLightMapBS = gf.createBlendStateObject(desc);
        }
        /* rasterizer state */
        {
            RasterizerStateDesc desc;
            desc.fillmode = RSP_FillWireFrame;
            desc.cullface = RSP_CullBack;
            mWireframeRS = gf.createRasterizerStateObject(desc);
        }
        return true;
    }


    void DeferredRenderer::makeLightMap(SceneManager& scene) {
        MIST_PROFILE(L"DEFERRED_LIGHTMAP");
        mLightMapRT->attachToRender();

        GraphicDevice& gd = Context::Instance().getGraphicFactory().getGraphicDevice();
        gd.clear(ukn::CM_Color, ukn::color::Transparent, 1.f, 0);
       
        const FrameBufferPtr& fb = mLightMapRT->getFrameBuffer();
        const CameraPtr& cam = fb->getViewport().camera;
        const Frustum& frustum = cam->getViewFrustum();
    
        Matrix4 invViewProj = (cam->getViewMatrix() * cam->getProjMatrix()).inverted();

        gd.setBlendState(mLightMapBS);
        gd.setDepthStencilState(DepthStencilStateObject::None());

        const LightManagerPtr& lights = scene.getLightManager();

        // directional lights
        {
            MIST_PROFILE(L"DEFERRED_LIGHTMAP_DIRECTIONAL");

            if(lights->getDirectionalLights().size() > 0) {
                Shader* fragmentShader = mDirectionalLightTechnique->getPass(0)->getFragmentShader().get();

                fragmentShader->setMatrixVariable("inverseViewProj", invViewProj);
                fragmentShader->setFloatVectorVariable("cameraPosition", cam->getEyePos());
                fragmentShader->setFloatVectorVariable("gbufferSize", mSize);
        
                fragmentShader->setTextureVariable("colorMap", 
                                                    mGBufferRT->getTargetTexture(ukn::ATT_Color0));
                fragmentShader->setTextureVariable("normalMap", 
                                                    mGBufferRT->getTargetTexture(ukn::ATT_Color1));
                fragmentShader->setTextureVariable("depthMap", 
                                                    mGBufferRT->getTargetTexture(ukn::ATT_Color2));

                for(const LightSourcePtr& light: lights->getDirectionalLights()) {
                    if(!light->getEnabled())
                        continue;

                    if(light->getCastShadows() && light->getShadowMap()) {
                        fragmentShader->setTextureVariable("shadowMap", light->getShadowMap()->getTexture());
                    }
                
                    fragmentShader->setFloatVectorVariable("lightColor", light->getColor());
                    fragmentShader->setFloatVariable("lightDirection", 3, light->getDirection().value);
                    fragmentShader->setFloatVariable("lightIntensity", light->getIntensity());
                
                    const CameraPtr& lightCam = light->getCamera(0);
                    fragmentShader->setMatrixVariable("lightView", lightCam->getViewMatrix());
                    fragmentShader->setMatrixVariable("lightViewProj", lightCam->getViewMatrix() * lightCam->getProjMatrix());
                    fragmentShader->setIntVariable("hasShadow", light->getCastShadows() ? 1 : 0);
                    fragmentShader->setFloatVariable("shadowMapSize", (float)light->getShadowMapResolution());
                    fragmentShader->setFloatVariable("depthPrecision", lightCam->getFarPlane());
                
                    ukn::SpriteBatch::DefaultObject().drawQuad(mDirectionalLightTechnique, 
                                                               ukn::Vector2(-1, 1), 
                                                               ukn::Vector2(1, -1));

                }
            }
        }
        // spot lights
        {
            MIST_PROFILE(L"DEFERRED_LIGHTMAP_SPOT");

            if(lights->getSpotLights().size() > 0) {
                Shader* fragmentShader = mSpotLightTechnique->getPass(0)->getFragmentShader().get();
                Shader* vertexShader = mSpotLightTechnique->getPass(0)->getVertexShader().get();
            
                vertexShader->setMatrixVariable("viewMatrix", cam->getViewMatrix());
                vertexShader->setMatrixVariable("projectionMatrix", cam->getProjMatrix());

                fragmentShader->setMatrixVariable("inverseViewProj", invViewProj);
                fragmentShader->setFloatVariable("cameraPosition", 3, cam->getEyePos().value);
                fragmentShader->setFloatVariable("gbufferSize", 2, mSize.value);

                fragmentShader->setTextureVariable("colorMap", 
                                                    mGBufferRT->getTargetTexture(ukn::ATT_Color0));
                fragmentShader->setTextureVariable("normalMap", 
                                                    mGBufferRT->getTargetTexture(ukn::ATT_Color1));
                fragmentShader->setTextureVariable("depthMap", 
                                                    mGBufferRT->getTargetTexture(ukn::ATT_Color2));
        
                for(const LightSourcePtr& light: lights->getSpotLights()) {
                    if(!light->getEnabled())
                        continue;

                    if(light->getAttenuationTexture()) {
                        fragmentShader->setTextureVariable("attenuationMap", light->getAttenuationTexture());
                    }
                    if(light->getCastShadows() && light->getShadowMap()) {
                        fragmentShader->setTextureVariable("shadowMap", light->getShadowMap()->getTexture());
                    }
                
                    SpotLight* sl = (SpotLight*)light.get();
                
                    const CameraPtr& lightCam = sl->getCamera(0);

                    vertexShader->setMatrixVariable("worldMatrix", sl->getWorldMatrix());
                    fragmentShader->setMatrixVariable("lightViewProj", lightCam->getViewMatrix() * lightCam->getProjMatrix());
                    fragmentShader->setFloatVectorVariable("lightPosition", sl->getPosition());
                    fragmentShader->setFloatVectorVariable("lightColor", sl->getColor());
                    fragmentShader->setFloatVectorVariable("lightDirection", sl->getDirection());
                    fragmentShader->setFloatVariable("lightIntensity", sl->getIntensity());

                    fragmentShader->setFloatVariable("lightAngleCos", sl->lightAngleCos());
                    fragmentShader->setFloatVariable("lightHeight", lightCam->getNearPlane());
                    fragmentShader->setIntVariable("hasShadow", sl->getCastShadows() ? 1 : 0);
                    fragmentShader->setFloatVariable("shadowMapSize", (float)sl->getShadowMapResolution());
                    fragmentShader->setFloatVariable("depthPrecision", lightCam->getFarPlane());
                    fragmentShader->setFloatVariable("depthBias", sl->getDepthBias());

                    {
                        float3 L = cam->getEyePos() - sl->getPosition();
                        float SL = abs(L.dot(sl->getDirection()));

                        if(SL < sl->lightAngleCos()) {
                            // within light  
                            gd.setRasterizerState(RasterizerStateObject::CullCounterClockwise());
                        } else {
                            gd.setRasterizerState(RasterizerStateObject::CullCounterClockwise());
                        }

                        // render spot light
                        gd.renderBuffer(mSpotLightTechnique, lights->getSpotLightGeometry());
                    }

                }
            }
        }
        // point lights
        {
            MIST_PROFILE(L"DEFERRED_LIGHTMAP_POINT");

            if(lights->getPointLights().size() > 0) {
                Shader* fragmentShader = mPointLightTechnique->getPass(0)->getFragmentShader().get();
                Shader* vertexShader = mPointLightTechnique->getPass(0)->getVertexShader().get();
            
                vertexShader->setMatrixVariable("viewMatrix", cam->getViewMatrix());
                vertexShader->setMatrixVariable("projectionMatrix", cam->getProjMatrix());

                fragmentShader->setMatrixVariable("inverseViewProj", invViewProj);
                fragmentShader->setFloatVariable("cameraPosition", 3, cam->getEyePos().value);
                fragmentShader->setFloatVariable("gbufferSize", 2, mSize.value);

                fragmentShader->setTextureVariable("colorMap", 
                                                    mGBufferRT->getTargetTexture(ukn::ATT_Color0));
                fragmentShader->setTextureVariable("normalMap", 
                                                    mGBufferRT->getTargetTexture(ukn::ATT_Color1));
                fragmentShader->setTextureVariable("depthMap", 
                                                    mGBufferRT->getTargetTexture(ukn::ATT_Color2));
        
                for(const LightSourcePtr& light: lights->getPointLights()) {
                    if(!light->getEnabled())
                        continue;
                
                    PointLight* sl = (PointLight*)light.get();
                    if(!frustum.isSphereVisible(mist::Sphere(light->getPosition(), sl->getRadius())))
                        continue;

                    if(light->getCastShadows() && light->getShadowMap()) {
                        fragmentShader->setTextureVariable("shadowMap", light->getShadowMap()->getTexture());
                    }
                    const CameraPtr& lightCam = sl->getCamera(0);

                    vertexShader->setMatrixVariable("worldMatrix", sl->getWorldMatrix());
                    fragmentShader->setFloatVectorVariable("lightPosition", sl->getPosition());
                    fragmentShader->setFloatVectorVariable("lightColor", sl->getColor());
                    fragmentShader->setFloatVariable("lightRadius", sl->getRadius());
                    fragmentShader->setFloatVariable("lightIntensity", sl->getIntensity());

                    fragmentShader->setIntVariable("hasShadow", sl->getCastShadows() ? 1 : 0);
                    fragmentShader->setFloatVariable("shadowMapSize", (float)sl->getShadowMapResolution());
                    fragmentShader->setFloatVariable("depthPrecision", lightCam->getFarPlane());
                    fragmentShader->setFloatVariable("depthBias", sl->getDepthBias());
                    {
                        // render spot light
                        gd.renderBuffer(mPointLightTechnique, lights->getPointLightGeometry());
                    }

                }
            }
        }
        mLightMapRT->detachFromRender();
           
        gd.setBlendState(BlendStateObject::Opaque());
        gd.setDepthStencilState(DepthStencilStateObject::Default());
        gd.setRasterizerState(RasterizerStateObject::CullCounterClockwise());
    }

    void DeferredRenderer::makeFinal() {
         mCompositeRT->attachToRender();
        
        GraphicDevice& gd = Context::Instance().getGraphicFactory().getGraphicDevice();
        gd.clear(ukn::CM_Color, ukn::color::Transparent, 1.f, 0);

        const ShaderPtr& fragmentShader = mCompositeTechnique->getPass(0)->getFragmentShader();
        fragmentShader->setTextureVariable("colorMap", 
                                            mGBufferRT->getTargetTexture(ATT_Color0));
        fragmentShader->setTextureVariable("lightMap",
                                            mLightMapRT->getTargetTexture(ATT_Color0));

        ukn::SpriteBatch::DefaultObject().drawQuad(mCompositeTechnique, ukn::Vector2(-1, 1), ukn::Vector2(1, -1));
        
        mCompositeRT->detachFromRender();
    }

    void DeferredRenderer::makeGBuffer(SceneManager& scene) {
        MIST_PROFILE(L"DEFERRED_GBUFFER");
        mGBufferRT->attachToRender();
        
        GraphicDevice& gd = Context::Instance().getGraphicFactory().getGraphicDevice();

        gd.setBlendState(BlendStateObject::Opaque());
        gd.setDepthStencilState(DepthStencilStateObject::DepthRead());
        gd.setRasterizerState(RasterizerStateObject::CullCounterClockwise());

        // first clear the GBuffer
        ukn::SpriteBatch::DefaultObject().drawQuad(mClearTechnique, ukn::Vector2(-1, 1), ukn::Vector2(1, -1));

        gd.setDepthStencilState(DepthStencilStateObject::Default());
        gd.clear(ukn::CM_Depth, mist::color::Transparent, 1.f, 0);

        const FrameBufferPtr& fb = mGBufferRT->getFrameBuffer();
        const CameraPtr& cam = fb->getViewport().camera;

        // render scene
        scene.render(mGBufferTechnique, cam->getViewMatrix(), cam->getProjMatrix());

        mGBufferRT->detachFromRender();
    }

    void DeferredRenderer::prepare() {
        GraphicDevice& gd = Context::Instance().getGraphicFactory().getGraphicDevice();
        CameraPtr currCamera = gd.getCurrFrameBuffer()->getViewport().camera;

        mGBufferRT->attachCamera(currCamera);
        mLightMapRT->attachCamera(currCamera);
        mCompositeRT->attachCamera(currCamera);
    }

    void DeferredRenderer::renderScene(SceneManager& scene) {
        this->prepare();
        this->makeGBuffer(scene);
        this->makeLightMap(scene);
        this->makeFinal();
    }

}