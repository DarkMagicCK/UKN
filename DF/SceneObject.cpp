//
//  SceneObject.cpp
//  Project Unknown
//
//  Created by Ruiwei Bu on 1/28/12.
//  Copyright (c) 2012 heizi. All rights reserved.
//

#include "SceneObject.h"
#include "Scene.h"

namespace df {
    
    Scene* SceneObject::getParentScene() const {
        return mParentScene;
    }
    
}
