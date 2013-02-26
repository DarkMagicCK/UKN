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

        void addLight(const DirectionalLightPtr& light);
        void removeLight(const DirectionalLightPtr& light);

    private:
        std::vector<DirectionalLightPtr> mDirectionalLights;
    };

    typedef SharedPtr<LightManager> LightManagerPtr;

}

#endif // UKN_LIGHT_MANAGER_H_