//
//  Item.h
//  Project Unknown
//
//  Created by Ruiwei Bu on 1/28/12.
//  Copyright (c) 2012 heizi. All rights reserved.
//

#ifndef Project_Unknown_Item_h
#define Project_Unknown_Item_h

#include "SceneObject.h"

namespace df {
    
    enum ItemType {
        IT_Weapon,
        IT_Armor,
        IT_Helm,
        IT_Leg,
        IT_Shoes,
        IT_Ring,
        IT_Neck,
        IT_Recovery,
        IT_Misc,
        IT_Treasure,
    };
    
    class Item: public virtual Interface {
    public:
        Item(ItemType type);
        // force base class
        virtual ~Item() = 0;
        
        ItemType getType() const;
        
    private:
        ItemType mType;
    };
    
    
} // namespace item


#endif
