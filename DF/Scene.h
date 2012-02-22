//
//  Scene.h
//  Project Unknown
//
//  Created by Ruiwei Bu on 1/28/12.
//  Copyright (c) 2012 heizi. All rights reserved.
//

#ifndef Project_DF_Scene_h
#define Project_DF_Scene_h

#include "Prereq.h"
#include "UKN/Serializer.h"

#include <list>

namespace df {
    
    class SceneObject;
    typedef SharedPtr<SceneObject> SceneObjectPtr;
    
    class Scene: public ukn::IConfigSerializable {
    public:
        Scene();
        ~Scene();
        
        SceneObjectPtr addSceneObject(SceneObject* obj);
        SceneObjectPtr delSceneObject(SceneObject* obj);
        
        // IConfigSerializable
    public:
        virtual bool serialize(const ConfigParserPtr& config);
        virtual bool deserialize(const ConfigParserPtr& config);
        
    public:
        
        void onRender();
        void onUpdate();
        
    public:
        ukn::SpriteBatch& getSpriteRenderer();
        
    public:
        typedef std::vector<SceneObjectPtr> SceneObjectList;
        
        SceneObjectList& getSceneObjects();
        const SceneObjectList& getSceneObjects() const;
        
    private:
        SceneObjectList mObjects;
        ukn::SpriteBatchPtr mSpriteRenderer;
    };
    
}

#endif
