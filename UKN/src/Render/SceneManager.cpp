#include "UKN/SceneManager.h"
#include "UKN/SceneObject.h"

namespace ukn {

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
        mSceneObjects.push_back(obj);
        this->onAddSceneObject(obj);
	}

    void SceneManager::delSceneObject(const SceneObjectPtr& obj) {
        for(SceneObjectList::const_iterator it = mSceneObjects.begin(), end = mSceneObjects.end();
            it != end;
            ++it) {
               if(*it == obj) {
                   mSceneObjects.erase(it);
                   this->onDelSceneObject(obj);
                   break;
               }
        }
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
        for(SceneObjectPtr& object: mSceneObjects) {
            object->update();
        }
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
    

}