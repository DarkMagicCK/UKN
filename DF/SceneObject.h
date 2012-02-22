//
//  SceneObject.h
//  Project Unknown
//
//  Created by Ruiwei Bu on 1/28/12.
//  Copyright (c) 2012 heizi. All rights reserved.
//

#ifndef Project_DF_SceneObject_h
#define Project_DF_SceneObject_h

#include "Prereq.h"

#include <vector>

namespace df {
    
    class Scene;
    
    class SceneObject: public virtual Interface {
    public:
        virtual ~SceneObject() { }
        
    public:
        virtual void onRender() = 0;
        virtual void onUpdate() = 0;
        
    public:
        Scene* getParentScene() const;
        
    private:
        friend class Scene;
        Scene* mParentScene;
    };
    
}


#endif
