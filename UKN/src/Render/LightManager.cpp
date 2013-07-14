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

#include "mist/Profiler.h"

namespace ukn {

    LightManager::LightManager() {
        GraphicFactory& gf = Context::Instance().getGraphicFactory();

        // tests
        mDepthWriteEffect = gf.createEffect();

        mEXPDepthMapTechnique = mDepthWriteEffect->appendTechnique();
        mDepthMapTechnique = mDepthWriteEffect->appendTechnique();
        mBlurTechnique = mDepthWriteEffect->appendTechnique();
        this->reloadShaders();

        mShadowMapRT = MakeSharedPtr<ukn::CompositeRenderTarget>();

        // temporary 
        mSpotLightGeometry = ModelLoader::BuildFromSphere(mist::Sphere(Vector3(0, 0, 0), 1), 5);
    
        mPointLightGeometry = ModelLoader::BuildFromSphere(mist::Sphere(Vector3(0, 0, 0), 1), 5);
    }

    LightManager::~LightManager() {

    }

    void LightManager::reloadShaders() {
        ukn::ShaderPtr vertexShader = mDepthWriteEffect->createShader(MIST_LOAD_RESOURCE(L"deferred/expdepth_vert.cg"), 
                                                                        VERTEX_SHADER_DESC("VertexProgram"));
        ukn::ShaderPtr fragmentShader = mDepthWriteEffect->createShader(MIST_LOAD_RESOURCE(L"deferred/expdepth_frag.cg"), 
                                                                        FRAGMENT_SHADER_DESC("FragmentProgram"));
        mEXPDepthMapTechnique->clear();
        mEXPDepthMapTechnique->appendPass(fragmentShader, vertexShader, ShaderPtr());
        
        vertexShader = mDepthWriteEffect->createShader(MIST_LOAD_RESOURCE(L"deferred/depth_vert.cg"), 
                                                                        VERTEX_SHADER_DESC("VertexProgram"));
        fragmentShader = mDepthWriteEffect->createShader(MIST_LOAD_RESOURCE(L"deferred/depth_frag.cg"), 
                                                                        FRAGMENT_SHADER_DESC("FragmentProgram"));
        mDepthMapTechnique->clear();
        mDepthMapTechnique->appendPass(fragmentShader, vertexShader, ShaderPtr());
        
        vertexShader = mDepthWriteEffect->createShader(MIST_LOAD_RESOURCE(L"deferred/shadowmap_blur_vert.cg"), 
                                                                        VERTEX_SHADER_DESC("VertexProgram"));
        fragmentShader = mDepthWriteEffect->createShader(MIST_LOAD_RESOURCE(L"deferred/shadowmap_blur_frag.cg"), 
                                                                        FRAGMENT_SHADER_DESC("FragmentProgram"));
        mBlurTechnique->clear();
        mBlurTechnique->appendPass(fragmentShader, vertexShader, ShaderPtr());
    }

    const LightManager::LightSourceVec& LightManager::getDirectionalLights() const {
        return mDirectionalLights;
    }

    const LightManager::LightSourceVec& LightManager::getSpotLights() const {
        return mSpotLights;
    }

    const LightManager::LightSourceVec& LightManager::getPointLights() const {
        return mPointLights;
    }

    void LightManager::addLight(const LightSourcePtr& light) {
        switch(light->type()) {
        case LS_Directional: mDirectionalLights.push_back(light); break;
        case LS_Spot:        mSpotLights.push_back(light); break;
        case LS_Point:       mPointLights.push_back(light); break;
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
        case LS_Point:       mPointLights.erase(std::remove(mPointLights.begin(),
                                                            mPointLights.end(),
                                                            light),
                                                mPointLights.end());
        }
    }

    void LightManager::renderShadowMap(GraphicDevice& gd, SceneManager& scene, const LightSourcePtr& ls) {
        const RenderTargetPtr& rt = ls->getShadowMap();
        EffectTechniquePtr techniuqe;
        if(ls->type() == LS_Directional)
            techniuqe = mDepthMapTechnique;
        else
            techniuqe = mEXPDepthMapTechnique;

        mShadowMapRT->attach(ATT_Color0, ls->getShadowMap());
        mShadowMapRT->attach(ATT_DepthStencil, 
                             ls->getDSView());

        mShadowMapRT->attachToRender();

        gd.clear(CM_Color | CM_Depth, color::Transparent, 1.0f, 0);

        Shader* fragmentShader = mDepthMapTechnique->getPass(0)->getFragmentShader().get();
        fragmentShader->setFloatVectorVariable("lightPosition", ls->getPosition());
        
        const CameraPtr& cam = ls->getCamera(0);
        fragmentShader->setFloatVariable("depthPrecision", cam->getFarPlane());

        scene.render(techniuqe, cam->getViewMatrix(), cam->getProjMatrix(), ~SOA_NotCastShadow);

        mShadowMapRT->detachFromRender();
    }

    void LightManager::makeShadowMaps(SceneManager& scene) {
        if(!mDepthMapTechnique->getPass(0)->isOK())
            return;

        MIST_PROFILE(L"SHADOW_MAP");

        GraphicDevice& gd = Context::Instance().getGraphicFactory().getGraphicDevice();
        
        gd.setBlendState(BlendStateObject::Opaque());
        gd.setDepthStencilState(DepthStencilStateObject::Default());
        gd.setRasterizerState(RasterizerStateObject::CullCounterClockwise());

        LightManagerPtr lights = scene.getLightManager();
        for(const LightSourcePtr& light: lights->getDirectionalLights()) {
            if(light->getCastShadows()) {
                mShadowMapRT->attachCamera(light->getCamera(0));
                this->renderShadowMap(gd, scene, light);
            }
        }

        lights = scene.getLightManager();
        for(const LightSourcePtr& light: lights->getSpotLights()) {
            if(light->getCastShadows()) {
                mShadowMapRT->attachCamera(light->getCamera(0));
                this->renderShadowMap(gd, scene, light);
            }
        }
    }

    const RenderBufferPtr& LightManager::getSpotLightGeometry() const {
        return mSpotLightGeometry;
    }
   
    const RenderBufferPtr& LightManager::getPointLightGeometry() const {
        return mPointLightGeometry;
    }

} // namespace ukn