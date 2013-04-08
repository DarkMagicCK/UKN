#include "UKN/SceneManager.h"
#include "UKN/SceneObject.h"
#include "UKN/Light.h"
#include "UKN/LightManager.h"
#include "UKN/GraphicDevice.h"
#include "UKN/GraphicFactory.h"
#include "UKN/Context.h"
#include "UKN/Model.h"
#include "UKN/Renderable.h"
#include "UKN/RenderBuffer.h"

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
		return mNumObjectsRendered;
	}

    uint32 SceneManager::numRenderableRendered() const {
		return mNumRenderableRendered;
	}

    uint32 SceneManager::numPrimitivesRenderered() const {
		return mNumPrimitivesRendered;
	}

    uint32 SceneManager::numVerticesRendered() const {
		return mNumVerticesRendered;
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

    void SceneManager::render(const EffectTechniquePtr& technique, const Matrix4& viewMat, const Matrix4& projMat) {
        mNumObjectsRendered = 0;
        mNumRenderableRendered = 0;
        mNumPrimitivesRendered = 0;
        mNumVerticesRendered = 0;
        
        GraphicDevice& gd = Context::Instance().getGraphicFactory().getGraphicDevice();
        
        const SceneManager::SceneObjectList& objects = this->getSceneObjects();
        Shader* vertexShader = technique->getPass(0)->getVertexShader().get();
        Shader* fragmentShader = technique->getPass(0)->getFragmentShader().get();
        
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
                        this->renderRenderable(gd, *mesh.get(), technique);
                    }
                } else {
                    this->renderRenderable(gd, *pr.get(), technique);
                }
            }
            mNumObjectsRendered++;
        }
    }

    
    void SceneManager::renderRenderable(GraphicDevice& gd, Renderable& renderable, const EffectTechniquePtr& technique) {
        Shader* vertexShader = technique->getPass(0)->getVertexShader().get();
        Shader* fragmentShader = technique->getPass(0)->getFragmentShader().get();
     
        if(renderable.getDiffuseTex()) {
            fragmentShader->setTextureVariable("diffuseMap", renderable.getDiffuseTex());
        } else {
            /* at least makesure there's a white texture for diffuse */
            uint32 c(0xFFFFFFFF);
            if(renderable.getMaterial()) {
                float3 d = renderable.getMaterial()->diffuse;
                c = COLOR_RGBA(d[0] * 255, d[1] * 255, d[2] * 255, 255);
            }
            static TexturePtr white = Context::Instance().getGraphicFactory()
                                        .create2DTexture(1, 1, 0, EF_RGBA8, (uint8*)&c);

            fragmentShader->setTextureVariable("diffuseMap", white);
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

        if(renderable.getMaterial()) {
            fragmentShader->setFloatVectorVariable("ambient", renderable.getMaterial()->ambient);
            fragmentShader->setFloatVariable("specular_power", renderable.getMaterial()->specular[0]);
            fragmentShader->setFloatVariable("specular_intensity", renderable.getMaterial()->specular_power);
        } else {
            fragmentShader->setFloatVectorVariable("ambient", float3(0, 0, 0));
            fragmentShader->setFloatVariable("specular_power", 0);
            fragmentShader->setFloatVariable("specular_intensity", 0);
        }

        mNumVerticesRendered += renderable.getRenderBuffer()->getVertexCount();
        mNumRenderableRendered ++;

        // to do with material
        renderable.render(technique);
    }

}