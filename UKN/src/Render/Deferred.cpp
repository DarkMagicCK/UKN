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

namespace ukn {

    DeferredRenderer::DeferredRenderer(const float2& size):
    mSize(size) {

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
                vertexShader = mEffect->createShader(MIST_LOAD_RESOURCE(L"vertex_deferred_clear.cg"), 
                                                     VERTEX_SHADER_DESC("VertexProgram"));
                fragmentShader = mEffect->createShader(MIST_LOAD_RESOURCE(L"fragment_deferred_clear.cg"), 
                                                       FRAGMENT_SHADER_DESC("FragmentProgram"));

                mClearPass = mEffect->appendPass();
                mClearPass->setFragmentShader(fragmentShader);
                mClearPass->setVertexShader(vertexShader);
                mClearPass->setVertexFormat(ukn::Vertex2D::Format());

                vertexShader = mEffect->createShader(MIST_LOAD_RESOURCE(L"vertex_deferred.cg"), 
                                                     VERTEX_SHADER_DESC("VertexProgram", ukn::VertexUVNormal::Format()));
                fragmentShader = mEffect->createShader(MIST_LOAD_RESOURCE(L"fragment_deferred.cg"), 
                                                       FRAGMENT_SHADER_DESC("FragmentProgram"));
            
                mGBufferPass = mEffect->appendPass();
                mGBufferPass->setFragmentShader(fragmentShader);
                mGBufferPass->setVertexShader(vertexShader);
                mGBufferPass->setVertexFormat(ukn::VertexUVNormal::Format());

                vertexShader = mEffect->createShader(MIST_LOAD_RESOURCE(L"vertex_deferred_lighting.cg"), 
                                                     VERTEX_SHADER_DESC("VertexProgram", ukn::Vertex2D::Format()));
                fragmentShader = mEffect->createShader(MIST_LOAD_RESOURCE(L"fragment_deferred_lighting.cg"), 
                                                       FRAGMENT_SHADER_DESC("FragmentProgram"));

                mDirectLightPass = mEffect->appendPass();
                mDirectLightPass->setFragmentShader(fragmentShader);
                mDirectLightPass->setVertexShader(vertexShader);
                mDirectLightPass->setVertexFormat(ukn::Vertex2D::Format());
            
                vertexShader = mEffect->createShader(MIST_LOAD_RESOURCE(L"vertex_deferred_pointlight.cg"), 
                                                     VERTEX_SHADER_DESC("VertexProgram", ukn::Vertex2D::Format()));
                fragmentShader = mEffect->createShader(MIST_LOAD_RESOURCE(L"fragment_deferred_pointlight.cg"), 
                                                       FRAGMENT_SHADER_DESC("FragmentProgram"));

                mPointLightPass = mEffect->appendPass();
                mPointLightPass->setFragmentShader(fragmentShader);
                mPointLightPass->setVertexShader(vertexShader);
                mPointLightPass->setVertexFormat(ukn::Vertex2D::Format());

                vertexShader = mEffect->createShader(MIST_LOAD_RESOURCE(L"vertex_deferred_composite.cg"), 
                                                     VERTEX_SHADER_DESC("VertexProgram", ukn::Vertex2D::Format()));
                fragmentShader = mEffect->createShader(MIST_LOAD_RESOURCE(L"fragment_deferred_composite.cg"), 
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

            mWireframeRS = gf.createRasterizerStateObject(desc);
        }
    }
}