#include "UKN/LightManager.h"

namespace ukn {

    LightManager::LightManager() {

    }

    LightManager::~LightManager() {

    }

    const LightManager::DirectionalLightVec& LightManager::getDirectionalLights() const {
        return mDirectionalLights;
    }

    void LightManager::addLight(const DirectionalLightPtr& light) {
        mDirectionalLights.push_back(light);
    }

    void LightManager::removeLight(const DirectionalLightPtr& light) {
        for(DirectionalLightVec::const_iterator it = mDirectionalLights.begin(), end = mDirectionalLights.end();
            it != end;
            ++it) {
               if(*it == light) {
                   mDirectionalLights.erase(it);
                   break;
               }
        }
    }

    void LightManager::addLight(const SpotLightPtr& light) {
        mSpotLights.push_back(light);
    }

    void LightManager::removeLight(const SpotLightPtr& light) {
        for(SpotLightVec::const_iterator it = mSpotLights.begin(), end = mSpotLights.end();
            it != end;
            ++it) {
               if(*it == light) {
                   mSpotLights.erase(it);
                   break;
               }
        }
    }

} // namespace ukn