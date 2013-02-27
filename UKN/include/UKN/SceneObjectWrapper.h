#ifndef UKN_SCENE_OBJECT_WRAPPER_H_
#define UKN_SCENE_OBJECT_WRAPPER_H_

#include "UKN/PreDeclare.h"
#include "UKN/SceneObject.h"

namespace ukn {

    // scene object wrappers, such as LightSource, Camera
    // to do
    class SceneObjectCamera: public SceneObject {
    public:
        SceneObjectCamera(const CameraPtr& cam);
        virtual ~SceneObjectCamera();

    private:
        CameraPtr mCamera;
    };

    class SceneObjectLightSource: public SceneObject {
    public:
        SceneObjectLightSource(const LightSourcePtr& light);
        virtual ~SceneObjectLightSource();

    private:
        LightSourcePtr mLightSource;
    };

}


#endif // UKN_SCENE_OBJECT_WRAPPER_H_