#include "UKN/SceneManager.h"

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
    

}