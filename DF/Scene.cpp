//
//  Scene.cpp
//  Project Unknown
//
//  Created by Ruiwei Bu on 1/28/12.
//  Copyright (c) 2012 heizi. All rights reserved.
//

#include "Scene.h"
#include "SceneObject.h"

#include "UKN/Operations.h"
#include "UKN/Util.h"
#include "UKN/GraphicDevice.h"
#include "UKN/Context.h"
#include "UKN/SpriteBatch.h"

namespace df {
    
    Scene::Scene() {
        mSpriteRenderer = ukn::Context::Instance().getGraphicFactory().createSpriteBatch();
    }
    
    Scene::~Scene() {
        
    }
    
    SceneObjectPtr Scene::addSceneObject(SceneObject* obj) {
        mObjects.push_back(obj);
        obj->mParentScene = this;
        return mObjects.back();
    }
    
    SceneObjectPtr Scene::delSceneObject(SceneObject* obj) {
        SceneObjectList::iterator it = mObjects.begin();
        while(it != mObjects.end()) {
            if(it->get() == obj) {
                SceneObjectPtr ptr = *it;
                mObjects.erase(it);
                
                obj->mParentScene = 0;
                return ptr;
            }
        }
        return SceneObjectPtr();
    }
    
    Scene::SceneObjectList& Scene::getSceneObjects() {
        return mObjects;
    }
    
    const Scene::SceneObjectList& Scene::getSceneObjects() const {
        return mObjects;
    }
    
    ukn::SpriteBatch& Scene::getSpriteRenderer() {
        return *mSpriteRenderer;
    }
    
    void Scene::onRender() {
        mSpriteRenderer->begin(ukn::SBS_Deffered);
        
        UKN_FOR_EACH(SceneObjectPtr& ptr, mObjects) {
            ptr->onRender();
        }
        
        mSpriteRenderer->end();
    }
    
    void Scene::onUpdate() {
        UKN_FOR_EACH(SceneObjectPtr& ptr, mObjects) {
            ptr->onUpdate();
        }
    }
    
    bool Scene::serialize(const ConfigParserPtr& config) {
        return false;
    }
    
    bool Scene::deserialize(const ConfigParserPtr& config) {
        
        // 
        return true;
    }
    
}