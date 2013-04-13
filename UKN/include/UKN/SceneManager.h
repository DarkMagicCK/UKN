#ifndef Project_Unknown_SceneManager_h
#define Project_Unknown_SceneManager_h

#include "UKN/PreDeclare.h"
#include "UKN/Frustum.h"
#include "UKN/Renderable.h"
#include "UKN/Shader.h"
#include "UKN/LightManager.h"
#include "UKN/SceneObject.h"

namespace ukn {

    class UKN_API SceneManager: Uncopyable {
    public:
        typedef std::vector<SceneObjectPtr> SceneObjectList;

    public:
        SceneManager();
        virtual ~SceneManager();
        
        static SceneManagerPtr NullObject();
        
        virtual void clipScene();
        
        void addSceneObject(const SceneObjectPtr& obj);
        void removeSceneObject(const SceneObjectPtr& obj);
        
        SceneObjectList& getSceneObjects();
        const SceneObjectList& getSceneObjects() const;

        void addLight(const LightSourcePtr& light);
        void removeLight(const LightSourcePtr& light);
        
        virtual bool isBoxVisible(const Box& box);
        
        void clear();
        
        void update();
        
        uint32 numObjectsRendered() const;
        uint32 numRenderableRendered() const;
        uint32 numPrimitivesRenderered() const;
        uint32 numVerticesRendered() const;

        const LightManagerPtr& getLightManager() const;

        void render(const EffectTechniquePtr& technique, const Matrix4& viewMat, const Matrix4& projMat, uint32 flag);
        
    protected:
        void flush();
        void renderRenderable(GraphicDevice& gd, Renderable& renderable, const EffectTechniquePtr& technique);

        virtual void onAddSceneObject(const SceneObjectPtr& obj);
        virtual void onDelSceneObject(const SceneObjectPtr& obj);
        
    protected:
        const Frustum* mFrustum;
        
        SceneObjectList mSceneObjects;
        
    private:
        void flushScene();
        
        uint32 mNumObjectsRendered;
        uint32 mNumRenderableRendered;
        uint32 mNumPrimitivesRendered;
        uint32 mNumVerticesRendered;

        LightManagerPtr mLightManager;
    };
    
}


#endif