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

} // namespace ukn