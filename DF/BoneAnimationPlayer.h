//
//  BoneAnimationPlayer.h
//  Project Unknown
//
//  Created by Ruiwei Bu on 1/28/12.
//  Copyright (c) 2012 heizi. All rights reserved.
//

#ifndef Project_Unknown_BoneAnimationPlayer_h
#define Project_Unknown_BoneAnimationPlayer_h

#include "Prereq.h"
#include "AnimationLayer.h"

#include "UKN/Skeletal.h"

namespace df {
    
    class BoneAnimationPlayer: public AnimationPlayer {
    public:
        BoneAnimationPlayer(const ukn::String& file);
        ~BoneAnimationPlayer();
        
        void play(const char* name, bool startover=false);
        void stop();
        void onUpdate();
        void onRender(ukn::SpriteBatch& renderer, float x, float y);
        
        void onAnimationComplete(void* sender, ukn::NullEventArgs& args);
        
    private:
        ukn::SkeletalAnimation mAnimation;
        
        ukn_string mAnimationToPlay;
    };
    
}

#endif
