#include "UKN/LightManager.h"

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

    LightManager::LightManager() {
        GraphicFactory& gf = Context::Instance().getGraphicFactory();

        mDepthWriteEffect = gf.createEffect();
        ukn::ShaderPtr vertexShader = mDepthWriteEffect->createShader(MIST_LOAD_RESOURCE(L"deferred/depth_vert.cg"), 
                                                                        VERTEX_SHADER_DESC("VertexProgram"));
        ukn::ShaderPtr fragmentShader = mDepthWriteEffect->createShader(MIST_LOAD_RESOURCE(L"deferred/depth_frag.cg"), 
                                                                        FRAGMENT_SHADER_DESC("FragmentProgram"));

        mDepthMapPass = mDepthWriteEffect->appendPass();
        mDepthMapPass->setFragmentShader(fragmentShader);
        mDepthMapPass->setVertexShader(vertexShader);
        mDepthMapPass->setVertexFormat(ukn::VertexUVNormal::Format());

        mShadowMapRT = MakeSharedPtr<ukn::CompositeRenderTarget>();
    }

    LightManager::~LightManager() {

    }

    const LightManager::LightSourceVec& LightManager::getDirectionalLights() const {
        return mDirectionalLights;
    }

    const LightManager::LightSourceVec& LightManager::getSpotLights() const {
        return mSpotLights;
    }

    void LightManager::addLight(const LightSourcePtr& light) {
        switch(light->type()) {
        case LS_Directional: mDirectionalLights.push_back(light); break;
        case LS_Spot:        mSpotLights.push_back(light); break;
        case LS_Point:       break;
        }
    }

    void LightManager::removeLight(const LightSourcePtr& light) {
        switch(light->type()) {
        case LS_Directional: mDirectionalLights.erase(std::remove(mDirectionalLights.begin(),
                                                                  mDirectionalLights.end(),
                                                                  light),
                                                      mDirectionalLights.end()); break;
        case LS_Spot:        mSpotLights.erase(std::remove(mSpotLights.begin(),
                                                           mSpotLights.end(),
                                                           light),
                                               mSpotLights.end()); break;
        case LS_Point:       break;
        }
    }

    void LightManager::renderShadowMap(GraphicDevice& gd, SceneManager& scene, const LightSourcePtr& ls) {
        const RenderTargetPtr& rt = ls->getShadowMap();

        mShadowMapRT->attach(ATT_Color0, ls->getShadowMap());
     //   mShadowMapRT->attach(ATT_DepthStencil, ls->getDSView());
        mShadowMapRT->attachToRender();

        gd.clear(CM_Color, color::Transparent, 1.0f, 0.f);

        Shader* fragmentShader = mDepthMapPass->getFragmentShader().get();
        fragmentShader->setFloatVectorVariable("lightPosition", ls->getPosition());

        SpotLight* sl = (SpotLight*)ls.get();
        fragmentShader->setFloatVariable("depthPrecision", sl->getFarPlane());

        scene.render(mDepthMapPass, ls->getViewMatrix(), ls->getProjMatrix());

        mShadowMapRT->detachFromRender();
    }

    void LightManager::makeShadowMaps(SceneManager& scene) {
        GraphicDevice& gd = Context::Instance().getGraphicFactory().getGraphicDevice();
        
        gd.setBlendState(BlendStateObject::Opaque());
        gd.setDepthStencilState(DepthStencilStateObject::Default());
        gd.setRasterizerState(RasterizerStateObject::CullCounterClockwise());
        
        CameraPtr currCamera = gd.getCurrFrameBuffer()->getViewport().camera;
        mShadowMapRT->attachCamera(currCamera);

        const LightManagerPtr& lights = scene.getLightManager();
        for(const LightSourcePtr& light: lights->getSpotLights()) {
            light->update();

            if(light->getCastShadows()) {
                this->renderShadowMap(gd, scene, light);
            }
        }
    }

} // namespace ukn