//
//  Character.h
//  Project Unknown
//
//  Created by Ruiwei Bu on 1/28/12.
//  Copyright (c) 2012 heizi. All rights reserved.
//

#ifndef Project_DF_Character_h
#define Project_DF_Character_h

#include "SceneObject.h"

#include "UKN/Component.h"

namespace df {
    
    /*
     base interface class for a character in game
     such as player, enemy and NPC
     */
    class Character: public SceneObject, public ukn::ComponentHolder {
    public:
        
    };
}

#endif
