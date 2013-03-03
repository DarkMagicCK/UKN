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
            mSize = float2(wnd.width(), wnd.height());
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

                mClearPass = mEffect->appendPass();
                mClearPass->setFragmentShader(fragmentShader);
                mClearPass->setVertexShader(vertexShader);
                mClearPass->setVertexFormat(ukn::Vertex2D::Format());

                vertexShader = mEffect->createShader(MIST_LOAD_RESOURCE(L"deferred/gbuffer_vert.cg"), 
                                                     VERTEX_SHADER_DESC("VertexProgram"));
                fragmentShader = mEffect->createShader(MIST_LOAD_RESOURCE(L"deferred/gbuffer_frag.cg"), 
                                                       FRAGMENT_SHADER_DESC("FragmentProgram"));
            
                mGBufferPass = mEffect->appendPass();
                mGBufferPass->setFragmentShader(fragmentShader);
                mGBufferPass->setVertexShader(vertexShader);
                mGBufferPass->setVertexFormat(ukn::VertexUVNormal::Format());

                vertexShader = mEffect->createShader(MIST_LOAD_RESOURCE(L"deferred/directionallight_vert.cg"), 
                                                     VERTEX_SHADER_DESC("VertexProgram"));
                fragmentShader = mEffect->createShader(MIST_LOAD_RESOURCE(L"deferred/directionallight_frag.cg"), 
                                                       FRAGMENT_SHADER_DESC("FragmentProgram"));

                mDirectionalLightPass = mEffect->appendPass();
                mDirectionalLightPass->setFragmentShader(fragmentShader);
                mDirectionalLightPass->setVertexShader(vertexShader);
                mDirectionalLightPass->setVertexFormat(ukn::Vertex2D::Format());

                vertexShader = mEffect->createShader(MIST_LOAD_RESOURCE(L"deferred/spotlight_vert.cg"), 
                                                     VERTEX_SHADER_DESC("VertexProgram"));
                fragmentShader = mEffect->createShader(MIST_LOAD_RESOURCE(L"deferred/spotlight_frag.cg"), 
                                                       FRAGMENT_SHADER_DESC("FragmentProgram"));

                mSpotLightPass = mEffect->appendPass();
                mSpotLightPass->setFragmentShader(fragmentShader);
                mSpotLightPass->setVertexShader(vertexShader);
                mSpotLightPass->setVertexFormat(ukn::Vertex2D::Format());
            
                vertexShader = mEffect->createShader(MIST_LOAD_RESOURCE(L"vertex_deferred_pointlight.cg"), 
                                                     VERTEX_SHADER_DESC("VertexProgram"));
                fragmentShader = mEffect->createShader(MIST_LOAD_RESOURCE(L"fragment_deferred_pointlight.cg"), 
                                                       FRAGMENT_SHADER_DESC("FragmentProgram"));

                mPointLightPass = mEffect->appendPass();
                mPointLightPass->setFragmentShader(fragmentShader);
                mPointLightPass->setVertexShader(vertexShader);
                mPointLightPass->setVertexFormat(ukn::Vertex2D::Format());

                vertexShader = mEffect->createShader(MIST_LOAD_RESOURCE(L"deferred/composite_vert.cg"), 
                                                     VERTEX_SHADER_DESC("VertexProgram"));
                fragmentShader = mEffect->createShader(MIST_LOAD_RESOURCE(L"deferred/composite_frag.cg"), 
                                                       FRAGMENT_SHADER_DESC("FragmentProgram"));

                mCompositePass = mEffect->appendPass();
                mCompositePass->setFragmentShader(fragmentShader);
                mCompositePass->setVertexShader(vertexShader);
                mCompositePass->setVertexFormat(ukn::Vertex2D::Format());

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
        mLightMapRT->attachToRender();
        
        GraphicDevice& gd = Context::Instance().getGraphicFactory().getGraphicDevice();
        gd.clear(ukn::CM_Color, ukn::color::Transparent, 1.f, 0.f);
       
        const FrameBufferPtr& fb = mLightMapRT->getFrameBuffer();
        const CameraPtr& cam = fb->getViewport().camera;
    
        Matrix4 invView = cam->getViewMatrix().inverted();
        Matrix4 invViewProj = (cam->getViewMatrix() * cam->getProjMatrix()).inverted();

        BlendStatePtr prevBS = gd.getBlendState();
        gd.setBlendState(mLightMapBS);
        gd.setDepthStencilState(DepthStencilStateObject::DepthRead());

        const LightManagerPtr& lights = scene.getLightManager();

        // directional lights
        {
            Shader* fragmentShader = mDirectionalLightPass->getFragmentShader().get();

            fragmentShader->setMatrixVariable("inverseView", invView);
            fragmentShader->setMatrixVariable("inverseViewProj", invViewProj);
            fragmentShader->setFloatVariable("cameraPosition", 3, cam->getEyePos().value);
            fragmentShader->setFloatVariable("gbufferSize", 2, mSize.value);
        
            fragmentShader->setTextureVariable("colorMap", 
                                                mGBufferRT->getTargetTexture(ukn::ATT_Color0));
            fragmentShader->setTextureVariable("normalMap", 
                                                mGBufferRT->getTargetTexture(ukn::ATT_Color1));
            fragmentShader->setTextureVariable("depthMap", 
                                                mGBufferRT->getTargetTexture(ukn::ATT_Color2));

            for(const LightSourcePtr& light: lights->getDirectionalLights()) {
                fragmentShader->setFloatVectorVariable("lightColor", light->getColor());
                fragmentShader->setFloatVariable("lightDirection", 3, light->getDirection().value);
                fragmentShader->setFloatVariable("lightIntensity", light->getIntensity());

                mDirectionalLightPass->begin();
                ukn::SpriteBatch::DefaultObject().drawQuad(ukn::Vector2(-1, 1), ukn::Vector2(1, -1));
                mDirectionalLightPass->end();
            }
        }
        // spot lights
        {
            Shader* fragmentShader = mDirectionalLightPass->getFragmentShader().get();
            Shader* vertexShader = mDirectionalLightPass->getVertexShader().get();
            
            vertexShader->setMatrixVariable("viewMatrix", cam->getViewMatrix());
            vertexShader->setMatrixVariable("projectionMatrix", cam->getProjMatrix());

            fragmentShader->setMatrixVariable("inverseView", invView);
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
                if(light->getAttenuationTexture()) {
                    fragmentShader->setTextureVariable("attenuationMap", light->getAttenuationTexture());
                }
                if(light->getCastShadows() && light->getShadowMap()) {
                    fragmentShader->setTextureVariable("shadowMap", light->getShadowMap()->getTexture());
                }
                
                SpotLight* sl = (SpotLight*)light.get();

                vertexShader->setMatrixVariable("worldMatrix", sl->getWorldMatrix());
                fragmentShader->setMatrixVariable("lightViewProj", sl->getViewMatrix() * sl->getProjMatrix());
                fragmentShader->setFloatVectorVariable("lightPosition", sl->getPosition());
                fragmentShader->setFloatVectorVariable("lightColor", sl->getColor());
                fragmentShader->setFloatVectorVariable("lightDirection", sl->getDirection());
                fragmentShader->setFloatVariable("lightIntensity", sl->getIntensity());

                fragmentShader->setFloatVariable("lightAngleCos", sl->lightAngleCos());
                fragmentShader->setFloatVariable("lightHeight", sl->getFarPlane());
                fragmentShader->setIntVariable("hasShadow", sl->getCastShadows() ? 1 : 0);
                fragmentShader->setFloatVariable("shadowMapSize", (float)sl->getShadowMapResolution());
                fragmentShader->setFloatVariable("depthPrecision", sl->getFarPlane());
                fragmentShader->setFloatVariable("depthBias", sl->getDepthBias());

                {
                    float3 L = cam->getEyePos() - sl->getPosition();
                    float SL = abs(L.dot(sl->getDirection()));

                    if(SL < sl->lightAngleCos()) {
                        gd.setRasterizerState(RasterizerStateObject::CullCounterClockwise());
                    } else {
                        gd.setRasterizerState(RasterizerStateObject::CullClockwise());
                    }

                    mSpotLightPass->begin();
                    // render spot light
                    mSpotLightPass->end();
                }

            }

        }

        mLightMapRT->detachFromRender();

        gd.setBlendState(BlendStateObject::Opaque());
        gd.setDepthStencilState(DepthStencilStateObject::Default());
        gd.setRasterizerState(RasterizerStateObject::CullCounterClockwise());

        gd.setBlendState(prevBS);
    }

    void DeferredRenderer::makeFinal() {
        mCompositeRT->attachToRender();

        GraphicDevice& gd = Context::Instance().getGraphicFactory().getGraphicDevice();
        gd.clear(ukn::CM_Color, ukn::color::Transparent, 1.f, 0.f);

        const ShaderPtr& fragmentShader = mCompositePass->getFragmentShader();
        fragmentShader->setTextureVariable("colorMap", 
                                            mGBufferRT->getTargetTexture(ATT_Color0));
        fragmentShader->setTextureVariable("lightMap",
                                            mLightMapRT->getTargetTexture(ATT_Color0));

        mCompositePass->begin();
        ukn::SpriteBatch::DefaultObject().drawQuad(ukn::Vector2(-1, 1), ukn::Vector2(1, -1));
        mCompositePass->end();

        mCompositeRT->detachFromRender();
    }

    void DeferredRenderer::makeGBuffer(SceneManager& scene) {
        mGBufferRT->attachToRender();
        
        GraphicDevice& gd = Context::Instance().getGraphicFactory().getGraphicDevice();

        gd.setBlendState(BlendStateObject::Opaque());
        gd.setDepthStencilState(DepthStencilStateObject::DepthRead());
        gd.setRasterizerState(RasterizerStateObject::CullCounterClockwise());

        // first clear the GBuffer
        mClearPass->begin();
        ukn::SpriteBatch::DefaultObject().drawQuad(ukn::Vector2(-1, 1), ukn::Vector2(1, -1));
        mClearPass->end();    

        gd.setDepthStencilState(DepthStencilStateObject::Default());
        gd.clear(ukn::CM_Depth, mist::color::Transparent, 1.f, 0);

        const FrameBufferPtr& fb = mGBufferRT->getFrameBuffer();
        const CameraPtr& cam = fb->getViewport().camera;

        // render scene
        scene.render(mGBufferPass, cam->getViewMatrix(), cam->getProjMatrix());

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