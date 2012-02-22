//
//  Movable.cpp
//  Project Unknown
//
//  Created by Ruiwei Bu on 1/28/12.
//  Copyright (c) 2012 heizi. All rights reserved.
//

#include "Movable.h"

namespace df {

    Movable::Movable():
    mPosition(ukn::Vector2(0.f, 0.f)),
    mPositionUpdater(mPosition) {
        
    }
    
    Movable::~Movable() {
        
    }
    
    ukn::Vector2& Movable::position() {
        return mPosition;
    }
    
    const ukn::Vector2& Movable::position() const {
        return mPosition;
    }
    
    void Movable::moveTo(const ukn::Vector2& pos, float t) {
        mPositionUpdater.restart(mPosition, pos, t);
    }
        
    void Movable::update() {
        float delta = ukn::FrameCounter::Instance().getPrevDelta();
        mPositionUpdater.update(delta);
    }
    
}
