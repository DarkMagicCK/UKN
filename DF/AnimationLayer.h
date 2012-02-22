//
//  AnimationLayer.h
//  Project Unknown
//
//  Created by Ruiwei Bu on 1/28/12.
//  Copyright (c) 2012 heizi. All rights reserved.
//

#ifndef Project_Unknown_AnimationLayer_h
#define Project_Unknown_AnimationLayer_h

#include "Prereq.h"

#include "ukn/Event.h"
#include "ukn/Component.h"

#include "ukn/Animation.h"

namespace df {
    
    class AnimationPlayer: public ukn::Component {
    public:
        virtual ~AnimationPlayer()  { }
        
        UKN_DEF_COMPONENT_NAME(AnimationPlayer);
        
    public:
        virtual void play(const char* name, bool startover=false) = 0;
        virtual void stop() = 0;
        
        virtual void onUpdate() = 0;
        virtual void onRender(ukn::SpriteBatch& renderer, float x, float y) = 0;
        
        void notifyParent(ukn::AnimationStatus status);
    };
    
}


#endif
