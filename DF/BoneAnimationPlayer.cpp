//
//  BoneAnimationLayer.cpp
//  Project Unknown
//
//  Created by Ruiwei Bu on 1/28/12.
//  Copyright (c) 2012 heizi. All rights reserved.
//

#include "BoneAnimationPlayer.h"
#include "Scene.h"
#include "SceneObject.h"

#include "UKN/Asset.h"

namespace df {
    
    BoneAnimationPlayer::BoneAnimationPlayer(const ukn::String& file) {
        ConfigParserPtr config = ukn::AssetManager::Instance().load<ukn::ConfigParser>(file);
        if(config) {
            mAnimation.deserialize(config);
        }
    }
    
    BoneAnimationPlayer::~BoneAnimationPlayer() {
        
        
    }
    
    void BoneAnimationPlayer::play(const char* name, bool startover) {
        if(!startover) {
            mAnimationToPlay = name;
        }
    }
    
    void BoneAnimationPlayer::onAnimationComplete(void* sender, ukn::NullEventArgs& args) {
        if(!mAnimationToPlay.empty()) {
            this->play(mAnimationToPlay.c_str(), true);
        }
        
        this->notifyParent(ukn::AS_Completed);
    }
   
    void BoneAnimationPlayer::stop() {
        mAnimation.stop();
    }
    
    void BoneAnimationPlayer::onUpdate() {
        mAnimation.update();
    }
    
    void BoneAnimationPlayer::onRender(ukn::SpriteBatch& renderer, float x, float y) {
        mAnimation.setPosition(ukn::Vector2(x, y));
        mAnimation.render(renderer);
    }
    
} // namespace df
