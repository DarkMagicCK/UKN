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

#include "UKN/SSAO.h"
#include "UKN/Fog.h"

namespace ukn {

    DeferredRenderer::DeferredRenderer(const float2& size):
    mSize(size) {
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
            mGBufferRT = MakeSharedPtr<ukn::CompositeRenderTarget>();
            mGBufferRT->attach(ukn::ATT_Color0,
                               MakeSharedPtr<ukn::RenderTarget>((uint32)mSize[0],
                                                                (uint32)mSize[1],
                                                                1,
                                                                ukn::EF_RGBA8));
            mGBufferRT->attach(ukn::ATT_Color1,
                               MakeSharedPtr<ukn::RenderTarget>((uint32)mSize[0],
                                                                (uint32)mSize[1],
                                                                1,
                                                                ukn::EF_RGBA8));
            mGBufferRT->attach(ukn::ATT_Color2,
                               MakeSharedPtr<ukn::RenderTarget>((uint32)mSize[0],
                                                                (uint32)mSize[1],
                                                                1,
                                                                ukn::EF_Float));

            mLightMap = MakeSharedPtr<ukn::RenderTarget>((uint32)mSize[0],
                                                                (uint32)mSize[1],
                                                                1,
                                                                ukn::EF_RGBA8);
       
            mGBufferRT->attach(ukn::ATT_DepthStencil,
                               MakeSharedPtr<ukn::RenderTarget>((uint32)mSize[0],
                                                                (uint32)mSize[1],
                                                                ukn::EF_D32));

            
            mLightMapRT = MakeSharedPtr<ukn::CompositeRenderTarget>();

     //       mGBufferRT->attach(ukn::ATT_Color3, mLightMapRT->getTarget(ukn::ATT_Color0));
            
            mCompositeRT = MakeSharedPtr<ukn::CompositeRenderTarget>();
            mCompositeRT->attach(ukn::ATT_Color0,
                                 MakeSharedPtr<ukn::RenderTarget>((uint32)mSize[0],
                                                                  (uint32)mSize[1],
                                                                  1,
                                                                  ukn::EF_RGBA8));
            
        }
        /* shaders */
        // temporary stored as file for test
        {
            mEffect = gf.createEffect();

            if(mEffect) {
                mClearTechnique = mEffect->appendTechnique();
                mGBufferTechnique = mEffect->appendTechnique();
                mDirectionalLightTechnique = mEffect->appendTechnique();
                mSpotLightTechnique = mEffect->appendTechnique();
                mPointLightTechnique = mEffect->appendTechnique();
                mCompositeTechnique = mEffect->appendTechnique();

                this->reloadShaders();
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

            mLightMapBS = gf.createBlendStateObject(desc);
        }
        /* rasterizer state */
        {
            RasterizerStateDesc desc;
            desc.fillmode = RSP_FillWireFrame;
            desc.cullface = RSP_CullBack;
            mWireframeRS = gf.createRasterizerStateObject(desc);
        }

        // register default posteffects
        this->registerPostEffect<SSAO>(L"SSAO");
        this->registerPostEffect<Fog>(L"Fog");
        return true;
    }

    void DeferredRenderer::reloadShaders() {
        ukn::ShaderPtr vertexShader;
        ukn::ShaderPtr fragmentShader;
            
        vertexShader = mEffect->createShader(MIST_LOAD_RESOURCE(L"deferred/clear_vert.cg"), 
                                                VERTEX_SHADER_DESC("VertexProgram"));
        fragmentShader = mEffect->createShader(MIST_LOAD_RESOURCE(L"deferred/clear_frag.cg"), 
                                                FRAGMENT_SHADER_DESC("FragmentProgram"));
        mClearTechnique->appendPass(fragmentShader, vertexShader, ShaderPtr());

        vertexShader = mEffect->createShader(MIST_LOAD_RESOURCE(L"deferred/gbuffer_vert.cg"), 
                                                VERTEX_SHADER_DESC("VertexProgram"));
        fragmentShader = mEffect->createShader(MIST_LOAD_RESOURCE(L"deferred/gbuffer_frag.cg"), 
                                                FRAGMENT_SHADER_DESC("FragmentProgram"));
        mGBufferTechnique->appendPass(fragmentShader, vertexShader, ShaderPtr());

        vertexShader = mEffect->createShader(MIST_LOAD_RESOURCE(L"deferred/directionallight_vert.cg"), 
                                                VERTEX_SHADER_DESC("VertexProgram"));
        fragmentShader = mEffect->createShader(MIST_LOAD_RESOURCE(L"deferred/directionallight_frag.cg"), 
                                                FRAGMENT_SHADER_DESC("FragmentProgram"));
        mDirectionalLightTechnique->appendPass(fragmentShader, vertexShader, ShaderPtr());

        vertexShader = mEffect->createShader(MIST_LOAD_RESOURCE(L"deferred/spotlight_vert.cg"), 
                                                VERTEX_SHADER_DESC("VertexProgram"));
        fragmentShader = mEffect->createShader(MIST_LOAD_RESOURCE(L"deferred/spotlight_frag.cg"), 
                                                FRAGMENT_SHADER_DESC("FragmentProgram"));
        mSpotLightTechnique->appendPass(fragmentShader, vertexShader, ShaderPtr());
        
        vertexShader = mEffect->createShader(MIST_LOAD_RESOURCE(L"deferred/pointlight_vert.cg"), 
                                                VERTEX_SHADER_DESC("VertexProgram"));
        fragmentShader = mEffect->createShader(MIST_LOAD_RESOURCE(L"deferred/pointlight_frag.cg"), 
                                                FRAGMENT_SHADER_DESC("FragmentProgram"));
        mPointLightTechnique->appendPass(fragmentShader, vertexShader, ShaderPtr());
        
        vertexShader = mEffect->createShader(MIST_LOAD_RESOURCE(L"deferred/composite_vert.cg"), 
                                                VERTEX_SHADER_DESC("VertexProgram"));
        fragmentShader = mEffect->createShader(MIST_LOAD_RESOURCE(L"deferred/composite_frag.cg"), 
                                                FRAGMENT_SHADER_DESC("FragmentProgram"));
        mCompositeTechnique->appendPass(fragmentShader, vertexShader, ShaderPtr());
    }

    void DeferredRenderer::makeLightMap(SceneManager& scene) {
        MIST_PROFILE(L"DEFERRED_LIGHTMAP");
        mLightMapRT->attach(ATT_Color0, mLightMap);
        mLightMapRT->attachToRender();

        GraphicDevice& gd = Context::Instance().getGraphicFactory().getGraphicDevice();
      //  gd.clear(ukn::CM_Color, ukn::color::Transparent, 1.f, 0);
       
        const FrameBufferPtr& fb = mLightMapRT->getFrameBuffer();
        const CameraPtr& cam = fb->getViewport().camera;
        const Frustum& frustum = cam->getViewFrustum();
    
        Matrix4 invViewProj = (cam->getViewMatrix() * cam->getProjMatrix()).inverted();

        gd.setBlendState(mLightMapBS);
        gd.setDepthStencilState(DepthStencilStateObject::None());
        gd.setSamplerState(SamplerStateObject::LinearWrap());

        const LightManagerPtr& lights = scene.getLightManager();

        // directional lights
        {
            MIST_PROFILE(L"DEFERRED_LIGHTMAP_DIRECTIONAL");

            if(mDirectionalLightTechnique->getPass(0)->isOK()) {
                
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
                        fragmentShader->setFloatVectorVariable("lightPosition", light->getPosition());
                        fragmentShader->setFloatVariable("depthBias", light->getDepthBias());
                
                        const CameraPtr& lightCam = light->getCamera(0);
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
        }
        // spot lights
        {
            MIST_PROFILE(L"DEFERRED_LIGHTMAP_SPOT");

            if(mSpotLightTechnique->getPass(0)->isOK()) {
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
        }
        // point lights
        {
            MIST_PROFILE(L"DEFERRED_LIGHTMAP_POINT");

            if(mPointLightTechnique->getPass(0)->isOK()) {
                if(lights->getPointLights().size() > 0) {
                    Shader* fragmentShader = mPointLightTechnique->getPass(0)->getFragmentShader().get();
                    Shader* vertexShader = mPointLightTechnique->getPass(0)->getVertexShader().get();
            
                    vertexShader->setMatrixVariable("viewMatrix", cam->getViewMatrix());
                    vertexShader->setMatrixVariable("projectionMatrix", cam->getProjMatrix());

                    fragmentShader->setMatrixVariable("inverseViewProj", invViewProj);
                    fragmentShader->setMatrixVariable("viewMat", cam->getViewMatrix());
                    fragmentShader->setMatrixVariable("projMat", cam->getProjMatrix());
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
        }
        mLightMapRT->detachFromRender();
        mLightMapRT->detach(ATT_Color0);
           
        gd.setBlendState(BlendStateObject::Opaque());
        gd.setDepthStencilState(DepthStencilStateObject::Default());
        gd.setRasterizerState(RasterizerStateObject::CullCounterClockwise());
    }

    void DeferredRenderer::makeFinal() {
        if(!mCompositeTechnique->getPass(0)->isOK())
            return;

         mCompositeRT->attachToRender();
        
        GraphicDevice& gd = Context::Instance().getGraphicFactory().getGraphicDevice();
        gd.clear(ukn::CM_Color, ukn::color::Transparent, 1.f, 0);

        const ShaderPtr& fragmentShader = mCompositeTechnique->getPass(0)->getFragmentShader();
        fragmentShader->setTextureVariable("colorMap", 
                                            mGBufferRT->getTargetTexture(ATT_Color0));
        fragmentShader->setTextureVariable("lightMap",
                                            mLightMapRT->getTargetTexture(ATT_Color0));

        ukn::SpriteBatch::DefaultObject().drawQuad(mCompositeTechnique, 
                                                    ukn::Vector2(-1, 1), 
                                                    ukn::Vector2(1, -1));

        mCompositeRT->detachFromRender();
    }

    void DeferredRenderer::makeGBuffer(SceneManager& scene) {
        if(!mClearTechnique->getPass(0)->isOK() ||
            !mGBufferTechnique->getPass(0)->isOK())
            return;

        MIST_PROFILE(L"DEFERRED_GBUFFER");
        mGBufferRT->attach(ATT_Color3, mLightMap);

        mGBufferRT->attachToRender();
        
        GraphicDevice& gd = Context::Instance().getGraphicFactory().getGraphicDevice();

        gd.setBlendState(BlendStateObject::Opaque());
        gd.setDepthStencilState(DepthStencilStateObject::DepthRead());
        gd.setRasterizerState(RasterizerStateObject::CullCounterClockwise());
        gd.setSamplerState(SamplerStateObject::LinearWrap());

        // first clear the GBuffer
        ukn::SpriteBatch::DefaultObject().drawQuad(mClearTechnique, ukn::Vector2(-1, 1), ukn::Vector2(1, -1));

        gd.setDepthStencilState(DepthStencilStateObject::Default());
        gd.clear(ukn::CM_Depth, mist::color::Transparent, 1.f, 0);

        const FrameBufferPtr& fb = mGBufferRT->getFrameBuffer();
        const CameraPtr& cam = fb->getViewport().camera;

        // render scene
        scene.render(mGBufferTechnique,
                     cam->getViewMatrix(),
                     cam->getProjMatrix(),
                     SOA_Cullable | SOA_Moveable | SOA_Overlay);

        mGBufferRT->detachFromRender();
        mGBufferRT->detach(ATT_Color3);
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

        TexturePtr target = mCompositeRT->getTarget(ATT_Color0)->getTexture();
        for(std::pair<MistString, PostEffectPtr>& pe: mPostEffects) {
            pe.second->render(target,
                              this->getNormalTarget()->getTexture(),
                              this->getDepthTarget()->getTexture());
            target = pe.second->getFinalTexture();
        }
    }

    RenderTargetPtr DeferredRenderer::getColorTarget() const {
        return mGBufferRT->getTarget(ATT_Color0);
    }

    RenderTargetPtr DeferredRenderer::getDepthTarget() const {
        return mGBufferRT->getTarget(ATT_Color2);
    }

    RenderTargetPtr DeferredRenderer::getNormalTarget() const {
        return mGBufferRT->getTarget(ATT_Color1);
    }
    
    RenderTargetPtr DeferredRenderer::getLightTarget() const {
        return mLightMapRT->getTarget(ATT_Color0);
    }

    TexturePtr DeferredRenderer::getFinalTexture() const {
        if(mPostEffects.empty()) 
            return mCompositeRT->getTarget(ATT_Color0)->getTexture();
        else
            return mPostEffects.back().second->getFinalTexture();
    }

    void DeferredRenderer::deregisterPostEffect(const MistString& name) {
        mPostEffectFactory.deregisterClass(name);
    }

    bool DeferredRenderer::addPostEffect(const MistString& name, const MistString& after) {
        PostEffect* pe = mPostEffectFactory.createClass(name);
        if(!pe) {
            log_error(L"DeferredRenderer::addPostEffect: error intiatiating post Effect " + name);
        } else {
            if(!pe->init(this->size())) {
                log_error(L"DeferredRenderer::addPostEffect: error initializing post effect " + name);
                return false;
            }
        }
        if(after.length() == 0) {
            mPostEffects.push_back(std::make_pair(name, PostEffectPtr(pe)));
            return true;
        } else {
            for(PostEffectQueue::iterator it = mPostEffects.begin(), end = mPostEffects.end();
                it != end;
                ++it) {
                    if(it->first == after) {
                        mPostEffects.insert(it, std::make_pair(name, PostEffectPtr(pe)));
                        return true;
                    }
            }
            log_info(L"DeferredRenderer::addPostEffectAfter: error finding post effect with name in post effect queue " + after);
            log_info(L"DeferredRenderer::addPostEffectAfter: effect appended to back");
            mPostEffects.push_back(std::make_pair(name, PostEffectPtr(pe)));
            return true;
        }
        return false;
    }

    void DeferredRenderer::removePostEffect(const MistString& name) {
        for(PostEffectQueue::iterator it = mPostEffects.begin(), end = mPostEffects.end();
            it != end;
            ++it) {
                if(it->first == name) {
                    mPostEffects.erase(it);
                    break;
                }
        }
    }

    const DeferredRenderer::PostEffectQueue& DeferredRenderer::getPostEffects() const {
        return mPostEffects;
    }

    PostEffectPtr DeferredRenderer::getPostEffect(const MistString& name) const {
        for(PostEffectQueue::const_iterator it = mPostEffects.begin(), end = mPostEffects.end();
            it != end;
            ++it) {
                if(it->first == name) {
                    return it->second;
                    break;
                }
        }
        return PostEffectPtr();
    }

}