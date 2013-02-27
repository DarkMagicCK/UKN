#include "UKN/LightManager.h"

namespace ukn {

    LightManager::LightManager() {

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

} // namespace ukn