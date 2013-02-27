#include "UKN/SceneObject.h"
#include "UKN/Renderable.h"

namespace ukn {

    SceneObject::SceneObject(uint32 attribute):
    mAttribute(attribute) {
        
    }

    SceneObject::SceneObject(const RenderablePtr& renderable, uint32 attr):
    mRenderable(renderable),
    mAttribute(attr) {
        if(mRenderable) {
            mRenderable->setModelMatrix(mModelMat);
        }
    }
    
    SceneObject::~SceneObject() {
        
    }

    const RenderablePtr& SceneObject::getRenderable() const {
        return mRenderable;
    }
    
    const Matrix4& SceneObject::getModelMatrix() const {
        return mModelMat;
    }

    void SceneObject::setModelMatrix(const Matrix4& mat) {
        mModelMat = mat;
    }
    
    Box SceneObject::getBound() const {
        return mRenderable->getBound();
    }
    
    void SceneObject::addToSceneManager() {
        
    }
    
    void SceneObject::update() {
        
    }
    
    uint32 SceneObject::getAttribute() const {
        return mAttribute;
    }
    
    bool SceneObject::isVisible() const {
        return (mAttribute & SOA_Unvisible) == 0;
    }
    
    void SceneObject::setVisible(bool flag) {
        if(flag) {
            mAttribute &= ~SOA_Unvisible;
        } else {
            mAttribute |= SOA_Unvisible;
        }
    }

	

}