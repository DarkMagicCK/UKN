//
//  Item.cpp
//  Project Unknown
//
//  Created by Ruiwei Bu on 1/28/12.
//  Copyright (c) 2012 heizi. All rights reserved.
//

#include "Item.h"

namespace df {
    
    Item::Item(ItemType type):
    mType(type) {
        
    }
    
    ItemType Item::getType() const {
        return mType;
    }
    
}
