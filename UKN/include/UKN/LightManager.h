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
        typedef std::vector<DirectionalLightPtr> DirectionalLightVec;
        const DirectionalLightVec& getDirectionalLights() const;

        typedef std::vector<SpotLightPtr> SpotLightVec;
        const SpotLightVec& getSpotLights() const;

        void addLight(const DirectionalLightPtr& light);
        void removeLight(const DirectionalLightPtr& light);

        void addLight(const SpotLightPtr& light);
        void removeLight(const SpotLightPtr& light);

        // temporary functions for testing..
        void beginShadowMap();
        void endShadowMap();

    private:
        DirectionalLightVec mDirectionalLights;
        SpotLightVec mSpotLights;

        EffectPtr mDepthWriteEffect;
    };

    typedef SharedPtr<LightManager> LightManagerPtr;

}

#endif // UKN_LIGHT_MANAGER_H_