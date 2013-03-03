#include "UKN/SceneManager.h"
#include "UKN/SceneObject.h"
#include "UKN/Light.h"
#include "UKN/LightManager.h"
#include "UKN/GraphicDevice.h"
#include "UKN/GraphicFactory.h"
#include "UKN/Context.h"
#include "UKN/Model.h"

namespace ukn {

	SceneManager::SceneManager() {
        mLightManager = MakeSharedPtr<LightManager>();
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

    void SceneManager::removeSceneObject(const SceneObjectPtr& obj) {
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

    void SceneManager::addLight(const LightSourcePtr& light) {
        mLightManager->addLight(light);
    }

    void SceneManager::removeLight(const LightSourcePtr& light) {

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
    
    const LightManagerPtr& SceneManager::getLightManager() const {
        return mLightManager;
    }

    void SceneManager::render(const EffectPassPtr& pass, const Matrix4& viewMat, const Matrix4& projMat) {
        GraphicDevice& gd = Context::Instance().getGraphicFactory().getGraphicDevice();
        
        const SceneManager::SceneObjectList& objects = this->getSceneObjects();
        Shader* vertexShader = pass->getVertexShader().get();
        Shader* fragmentShader = pass->getFragmentShader().get();
        
        vertexShader->setMatrixVariable("viewMatrix", viewMat);
        vertexShader->setMatrixVariable("projectionMatrix", projMat);
        
        for(const SceneObjectPtr& obj: objects) {
            vertexShader->setMatrixVariable("worldMatrix", obj->getModelMatrix());
      
            const RenderablePtr& pr = obj->getRenderable();
            if(pr) {
                Model* model = dynamic_cast<Model*>(pr.get());
                // if it's a model, render each mesh
                if(model) {
                    for(uint32 i=0; i<model->getMeshCount(); ++i) {
                        const MeshPtr& mesh = model->getMesh(i);
                        this->renderRenderable(gd, *mesh.get(), pass);
                    }
                } else {
                    this->renderRenderable(gd, *pr.get(), pass);
                }
            }
        }
    }

    
    void SceneManager::renderRenderable(GraphicDevice& gd, Renderable& renderable, const EffectPassPtr& pass) {
        Shader* vertexShader = pass->getVertexShader().get();
        Shader* fragmentShader = pass->getFragmentShader().get();
     
        if(renderable.getDiffuseTex()) {
            fragmentShader->setTextureVariable("diffuseMap", renderable.getDiffuseTex());
        }
        if(renderable.getEmitTex()) {
            fragmentShader->setTextureVariable("emitMap", renderable.getEmitTex());
        }
        if(renderable.getNormalTex()) {
            fragmentShader->setTextureVariable("normalMap", renderable.getNormalTex());
        }
        if(renderable.getHeightTex()) {
            fragmentShader->setTextureVariable("heightMap", renderable.getHeightTex());
        }
        if(renderable.getSpecularTex()) {
            fragmentShader->setTextureVariable("specularMap", renderable.getSpecularTex());
        }

        // to do with material
        pass->begin();
        renderable.render();
        pass->end();
    }

}