//
//  Scene.cpp
//  Project Unknown
//
//  Created by Robert Bu on 12/17/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#include "UKN/Scene.h"

namespace ukn {
    
    SceneObject::SceneObject(uint32 attribute):
    mAttribute(attribute) {
        
    }
    
    SceneObject::~SceneObject() {
        
    }
    
    const RenderablePtr& SceneObject::getRenderable() const {
        return mRenderable;
    }
    
    const Matrix4& SceneObject::getModelMatrix() const {
        static Matrix4 mat;
        return mat;
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

    
} // namespace ukn