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

	
	SceneManager::SceneManager() {

	}
    SceneManager::~SceneManager() {

	}
        
    SceneManagerPtr SceneManager::NullObject() {
		return SceneManagerPtr();
	}
        
    void SceneManager::clipScene() {

	}
        
    void SceneManager::addSceneObject(const SceneObjectPtr& obj) {

	}

    void SceneManager::delSceneObject(const SceneObjectPtr& obj) {


	}
        
    void SceneManager::addRenderable(const RenderablePtr& renderable) {

	}
        
    SceneManager::SceneObjectList& SceneManager::getSceneObjects() {
		return mSceneObjects;
	}

    const SceneManager::SceneObjectList& SceneManager::getSceneObjects() const {
		return mSceneObjects;
	}
        
    bool SceneManager::isBoxVisible(const Box& box) {
		return  false;
	}
        
    void SceneManager::clear() {

	}
        
    void SceneManager::update() {

	}
        
    uint32 SceneManager::numObjectsRendered() const {
		return 0;
	}

    uint32 SceneManager::numRenderableRendered() const {
		return 0;
	}

    uint32 SceneManager::numPrimitivesRenderered() const {
		return 0;
	}

    uint32 SceneManager::numVerticesRendered() const {
		return 0;
	}
        
    void SceneManager::flush() {

	}
        
    void SceneManager::onAddSceneObject(const SceneObjectPtr& obj) {

	}
    void SceneManager::onDelSceneObject(const SceneObjectPtr& obj) {

	}
    
} // namespace ukn