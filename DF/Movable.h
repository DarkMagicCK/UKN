//
//  Movable.h
//  Project Unknown
//
//  Created by Ruiwei Bu on 1/28/12.
//  Copyright (c) 2012 heizi. All rights reserved.
//

#ifndef Project_Unknown_Movable_h
#define Project_Unknown_Movable_h

#include "Prereq.h"

#include "TimeUpdater.h"

#include "ukn/MathUtil.h"

namespace df {
    
    class Movable {
    public:
        Movable();
        ~Movable();
        
        ukn::Vector2& position();
        const ukn::Vector2& position() const;
        
        void moveTo(const ukn::Vector2& pos, float t);
        
        void update();
        
    private:
        ukn::Vector2 mPosition;
        
        TimeUpdater<ukn::Vector2> mPositionUpdater;
    };
    
}


#endif
