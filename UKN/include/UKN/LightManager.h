#ifndef UKN_LIGHT_MANAGER_H_
#define UKN_LIGHT_MANAGER_H_

#include "UKN/PreDeclare.h"
#include "UKN/Light.h"

namespace ukn {

    class UKN_API LightManager: Uncopyable {
    public:
        LightManager();
        ~LightManager();

    public:
        typedef std::vector<LightSourcePtr> LightSourceVec;
        const LightSourceVec& getDirectionalLights() const;
        const LightSourceVec& getSpotLights() const;

        void addLight(const LightSourcePtr& light);
        void removeLight(const LightSourcePtr& light);

        void makeShadowMaps(SceneManager& scene);

        const RenderBufferPtr& getSpotLightGeometry() const;
        const RenderBufferPtr& getPointLightGeometry() const;

    private:
        void renderShadowMap(GraphicDevice& gd, SceneManager& scene, const LightSourcePtr& ls);
        
        LightSourceVec mDirectionalLights;
        LightSourceVec mSpotLights;

        EffectPtr mDepthWriteEffect;
        EffectPassPtr mDepthMapPass;
        
        RenderBufferPtr mPointLightGeometry;
        RenderBufferPtr mSpotLightGeometry;

        CompositeRenderTargetPtr mShadowMapRT;
    };

    typedef SharedPtr<LightManager> LightManagerPtr;

}

#endif // UKN_LIGHT_MANAGER_H_