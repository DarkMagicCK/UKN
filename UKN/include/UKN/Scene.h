//
//  Scene.h
//  Project Unknown
//
//  Created by Robert Bu on 11/23/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_Scene_h
#define Project_Unknown_Scene_h

#include "UKN/Platform.h"
#include "UKN/PreDeclare.h"
#include "UKN/MathUtil.h"
#include "UKN/Uncopyable.h"
#include "UKN/Frustum.h"
#include "UKN/Renderable.h"

#include <vector>

namespace ukn {
    
    class UKN_API SceneNode {
    public:
        virtual ~SceneNode() { }
        
        virtual void addRenderable(const RenderablePtr& renderable) {
            mRenderables.push_back(renderable);
        }
        
    protected:
        typedef std::vector<RenderablePtr> RenderableList;
        RenderableList mRenderables;
    };
    
    enum SceneObjectAttribute {
        SOA_Cullable    = 1UL << 0,
        SOA_Overlay     = 1UL << 1,
        SOA_Moveable    = 1UL << 2,
        SOA_Unvisible   = 1UL << 3
    };
    
    class UKN_API SceneObject {
    public:
        explicit SceneObject(uint32 attribute);
        virtual ~SceneObject();
        
        const RenderablePtr& getRenderable() const;
        
        virtual const Matrix4& getModelMatrix() const;
        virtual Box getBound() const;
        
        virtual void addToSceneManager();
        virtual void update();
        
        uint32 getAttribute() const;
        bool isVisible() const;
        void setVisible(bool flag);
        
    protected:
        uint32 mAttribute;
        RenderablePtr mRenderable;
    };
    
    class UKN_API SceneManager: Uncopyable {
    public:
        typedef std::vector<SceneObjectPtr> SceneObjectList;
        
    protected:
        typedef std::vector<RenderablePtr> RenderItemsList;
        
    public:
        SceneManager();
        virtual ~SceneManager();
        
        static SceneManagerPtr NullObject();
        
        virtual void clipScene();
        
        void addSceneObject(const SceneObjectPtr& obj);
        void delSceneObject(const SceneObjectPtr& obj);
        
        void addRenderable(const RenderablePtr& renderable);
        
        SceneObjectList& getSceneObjects();
        const SceneObjectList& getSceneObjects() const;
        
        virtual bool isBoxVisible(const Box& box);
        
        void clear();
        
        void update();
        
        uint32 numObjectsRendered() const;
        uint32 numRenderableRendered() const;
        uint32 numPrimitivesRenderered() const;
        uint32 numVerticesRendered() const;
        
    protected:
        void flush();
        
        virtual void onAddSceneObject(const SceneObjectPtr& obj);
        virtual void onDelSceneObject(const SceneObjectPtr& obj);
        
    protected:
        const Frustum* mFrustim;
        
        SceneObjectList mSceneObjects;
        
    private:
        void flushScene();
        
        uint32 mNumObjectsRendered;
        uint32 mNumRenderableRendered;
        uint32 mNumPrimitivesRendered;
        uint32 mNumVerticesRendered;
    };
    
} // namespace ukn


#endif
