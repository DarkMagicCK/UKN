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
                                    new ukn::RenderTarget(mSize[0],
                                                          mSize[1],
                                                          1,
                                                          ukn::EF_RGBA8));
            mGBufferRT->attach(ukn::ATT_Color1,
                                    new ukn::RenderTarget(mSize[0],
                                                          mSize[1],
                                                          1,
                                                          ukn::EF_RGBA8));
            mGBufferRT->attach(ukn::ATT_Color2,
                                    new ukn::RenderTarget(mSize[0],
                                                          mSize[1],
                                                          1,
                                                          ukn::EF_Float));
            mGBufferRT->attach(ukn::ATT_DepthStencil,
                                    new ukn::RenderTarget(mSize[0],
                                                          mSize[1],
                                                          ukn::EF_D16));

            
            mLightMapRT = new ukn::CompositeRenderTarget();
            mLightMapRT->attach(ukn::ATT_Color0,
                                    new ukn::RenderTarget(800,
                                                          600,
                                                          1,
                                                          ukn::EF_RGBA8));
            
            mCompositeRT = new ukn::CompositeRenderTarget();
            mCompositeRT->attach(ukn::ATT_Color0,
                                    new ukn::RenderTarget(800,
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
            desc.fillmode = RSP_FillSolid;
            desc.cullface = RSP_CullBack;
            mWireframeRS = gf.createRasterizerStateObject(desc);
        }
        return true;
    }

    void DeferredRenderer::begin(const LightManagerPtr& lights) {
        GraphicDevice& gd = Context::Instance().getGraphicFactory().getGraphicDevice();
        CameraPtr currCamera = gd.getCurrFrameBuffer()->getViewport().camera;

        mGBufferRT->attachCamera(currCamera);
        mLightMapRT->attachCamera(currCamera);
        mCompositeRT->attachCamera(currCamera);

        // lights for this render
        mLights = lights;

        mGBufferRT->attachToRender();
        
        gd.setBlendState(BlendStateObject::Opaque());
        gd.setDepthStencilState(DepthStencilStateObject::DepthRead());
        gd.setRasterizerState(RasterizerStateObject::CullCounterClockwise());

        // first clear the GBuffer
        mClearPass->begin();
        ukn::SpriteBatch::DefaultObject().drawQuad(ukn::Vector2(-1, 1), ukn::Vector2(1, -1));
        mClearPass->end();    

        gd.setDepthStencilState(DepthStencilStateObject::Default());
        gd.clear(ukn::CM_Depth, mist::color::Transparent, 1.f, 0);
        gd.setRasterizerState(mWireframeRS);

        // model rendering start
        mBegan = true;
    }

    void DeferredRenderer::end() {
        mBegan = false;

        // model rendering end
        mGBufferRT->detachFromRender();

        this->makeLightMap(mLights);
        this->composite();
    }

    void DeferredRenderer::renderBuffer(const RenderBufferPtr& buffer, const TexturePtr& tex, const Matrix4& worldMat) {
        this->startRenderBuffer(worldMat, tex);
        
        GraphicDevice& gd = Context::Instance().getGraphicFactory().getGraphicDevice();
        gd.renderBuffer(buffer);
 
        this->endRenderBuffer();
    }

    void DeferredRenderer::startRenderBuffer(const Matrix4& worldMat, const TexturePtr& tex) {
        const FrameBufferPtr& fb = mGBufferRT->getFrameBuffer();
        const CameraPtr& cam = fb->getViewport().camera;

        const ShaderPtr& vertexShader = mGBufferPass->getVertexShader();
        if(cam) {
            vertexShader->setMatrixVariable("viewMatrix", cam->getViewMatrix());
            vertexShader->setMatrixVariable("projectionMatrix", cam->getProjMatrix());
        }
        vertexShader->setMatrixVariable("worldMatrix", worldMat);
        mGBufferPass->getFragmentShader()->setTextureVariable("tex", tex);

        mGBufferPass->begin();
    }

    void DeferredRenderer::endRenderBuffer() {
        mGBufferPass->end();
    }

    void DeferredRenderer::makeLightMap(const LightManagerPtr& lights) {
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

        const ShaderPtr& fragmentShader = mDirectionalLightPass->getFragmentShader();

        fragmentShader->setMatrixVariable("inverseView", invView);
        fragmentShader->setMatrixVariable("inverseViewProj", invViewProj);
        fragmentShader->setFloatVariable("cameraPosition", 3, cam->getEyePos().value);
        fragmentShader->setFloatVariable("gbufferSize", 2, mSize.value);
        
        float specularPower = 0.4; float specularIntensity = 0.8;
        fragmentShader->setFloatVariable("specularIntensity", 1, &specularIntensity);
        fragmentShader->setFloatVariable("specularPower", 1, &specularPower);

        fragmentShader->setTextureVariable("colorMap", 
                                            mGBufferRT->getTargetTexture(ukn::ATT_Color0));
        fragmentShader->setTextureVariable("normalMap", 
                                            mGBufferRT->getTargetTexture(ukn::ATT_Color1));
        fragmentShader->setTextureVariable("depthMap", 
                                            mGBufferRT->getTargetTexture(ukn::ATT_Color2));

        for(const DirectionalLightPtr& light: lights->getDirectionalLights()) {
            fragmentShader->setFloatVectorVariable("lightColor", light->getColor());
            fragmentShader->setFloatVariable("lightDirection", 3, light->getDirection().value);
            
            float intensity = light->getIntensity();
            fragmentShader->setFloatVariable("lightIntensity", 1, &intensity);

            mDirectionalLightPass->begin();
            ukn::SpriteBatch::DefaultObject().drawQuad(ukn::Vector2(-1, 1), ukn::Vector2(1, -1));
            mDirectionalLightPass->end();
        }

        mLightMapRT->detachFromRender();

        gd.setBlendState(BlendStateObject::Opaque());
        gd.setDepthStencilState(DepthStencilStateObject::Default());
        gd.setRasterizerState(RasterizerStateObject::CullCounterClockwise());

        gd.setBlendState(prevBS);
    }

    void DeferredRenderer::composite() {
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
}


/*

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
            
                deferredFragmentShader->setFloatVectorVariable("lightDirection", ukn::float4(-1, 0, 0, 1));
                deferredFragmentShader->setFloatVectorVariable("lightColor", ukn::float4(0, 0, 1, 1));
            
                effect->getPass(1)->begin();
                ukn::SpriteBatch::DefaultObject().drawQuad(ukn::Vector2(-1, 1), ukn::Vector2(1, -1));
                effect->getPass(1)->end();

                deferredFragmentShader->setFloatVectorVariable("lightDirection", ukn::float4(1, 0, 0, 1));
                deferredFragmentShader->setFloatVectorVariable("lightColor", ukn::float4(1, 0, 0, 1));
        
                effect->getPass(1)->begin();
                ukn::SpriteBatch::DefaultObject().drawQuad(ukn::Vector2(-1, 1), ukn::Vector2(1, -1));
                effect->getPass(1)->end();
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

            */