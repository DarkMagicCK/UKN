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

        for(const LightSourcePtr& light: lights->getDirectionalLights()) {
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

    void DeferredRenderer::renderRenderable(Renderable& renderable, GraphicDevice& gd, const EffectPassPtr& pass) {
        Shader* vertexShader = mGBufferPass->getVertexShader().get();
        Shader* fragmentShader = mGBufferPass->getFragmentShader().get();
     
        if(renderable.getDiffuseTex()) {
            fragmentShader->setTextureVariable("diffuseMap", renderable.getDiffuseTex());
        }
        if(renderable.getEmitTex()) {
            fragmentShader->setTextureVariable("emitMap", renderable.getEmitTex());
        }
        if(renderable.getNormalTex()) {
            fragmentShader->setTextureVariable("normalMap", renderable.getNormalTex());
        }
        if(renderable.getHeightTex()) {
            fragmentShader->setTextureVariable("heightMap", renderable.getHeightTex());
        }
        if(renderable.getSpecularTex()) {
            fragmentShader->setTextureVariable("specularMap", renderable.getSpecularTex());
        }

        // to do with material
        pass->begin();
        renderable.render();
        pass->end();
    }

    void DeferredRenderer::makeGBuffer() {
        mGBufferRT->attachToRender();
        
        GraphicDevice& gd = Context::Instance().getGraphicFactory().getGraphicDevice();
        SceneManager& scene = Context::Instance().getSceneManager();

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

        Shader* vertexShader = mGBufferPass->getVertexShader().get();
        Shader* fragmentShader = mGBufferPass->getFragmentShader().get();
        if(cam) {
            vertexShader->setMatrixVariable("viewMatrix", cam->getViewMatrix());
            vertexShader->setMatrixVariable("projectionMatrix", cam->getProjMatrix());
        }

        SceneManager::SceneObjectList& objects = scene.getSceneObjects();
        for(SceneObjectPtr& obj: objects) {
            
            vertexShader->setMatrixVariable("worldMatrix", obj->getModelMatrix());
      
            const RenderablePtr& pr = obj->getRenderable();
            if(pr) {
                Model* model = dynamic_cast<Model*>(pr.get());
                // if it's a model, render each mesh
                if(model) {
                    for(uint32 i=0; i<model->getMeshCount(); ++i) {
                        const MeshPtr& mesh = model->getMesh(i);
                        this->renderRenderable(*mesh.get(), gd, mGBufferPass);
                    }
                } else {
                    this->renderRenderable(*pr.get(), gd, mGBufferPass);
                }
            }
            
        }

        
        mGBufferRT->detachFromRender();
    }

    void DeferredRenderer::prepare() {
        GraphicDevice& gd = Context::Instance().getGraphicFactory().getGraphicDevice();
        CameraPtr currCamera = gd.getCurrFrameBuffer()->getViewport().camera;

        mGBufferRT->attachCamera(currCamera);
        mLightMapRT->attachCamera(currCamera);
        mCompositeRT->attachCamera(currCamera);
    }

    void DeferredRenderer::renderScene() {
        this->prepare();
        this->makeGBuffer();
        this->makeLightMap(Context::Instance().getSceneManager().getLightManager());
        this->makeFinal();
    }

}