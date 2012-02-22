//
//  AnimationLayer.cpp
//  Project Unknown
//
//  Created by Ruiwei Bu on 1/28/12.
//  Copyright (c) 2012 heizi. All rights reserved.
//

#include "AnimationLayer.h"

namespace df {
    
    void AnimationPlayer::notifyParent(ukn::AnimationStatus status) {
        if(this->getOwner() != 0) {
            ukn::MessageEventArgs args((ukn::int32)status);
            this->getOwner()->notify(this, args);
        }
    }
    
} // namespace df
