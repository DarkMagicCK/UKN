#include "UKN/SceneObjectWrapper.h"

namespace ukn {

    SceneObjectCamera::SceneObjectCamera(const CameraPtr& cam):
    SceneObject(SOA_Cullable | SOA_Moveable | SOA_NotCastShadow),
    mCamera(cam) {

    }

    SceneObjectCamera::~SceneObjectCamera() {

    }

    SceneObjectLightSource::SceneObjectLightSource(const LightSourcePtr& light):
    SceneObject(SOA_Cullable | SOA_Moveable | SOA_NotCastShadow),
    mLightSource(light) {

    }

    SceneObjectLightSource::~SceneObjectLightSource() {

    }


} // namespace ukn