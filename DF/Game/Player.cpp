//
//  Player.cpp
//  Project Unknown
//
//  Created by Ruiwei Bu on 1/28/12.
//  Copyright (c) 2012 heizi. All rights reserved.
//

#include "Player.h"
#include "BoneAnimationPlayer.h"
#include "Scene.h"

#include "UKN/ConfigParser.h"

namespace df {
    
    Player::Player() {
        
    }

    Player::~Player() {
        
    }
    
    void Player::onComponentAdded(ukn::Component* component) {
        
    }
    
    void Player::onComponentRemoved(ukn::Component* component) {
        
    }
    
    void Player::onRender() {
        if(mRenderer)
            mRenderer->onRender(getParentScene()->getSpriteRenderer(),
                                mMovable.position().x,
                                mMovable.position().y);
    }
    
    bool Player::serialize(const ukn::ConfigParserPtr& config) {
        return false;
    }
    
    bool Player::deserialize(const ukn::ConfigParserPtr& config) {
        if(config->toNode("player")) {
            ukn::ukn_string type = config->getString("anim_type");
            if(type == "skeletal") {
                mRenderer = new BoneAnimationPlayer(config->getString("anim_path"));
                mRenderer->setOwner(this);
            }
            
            mSpeed = config->getFloat("base_speed");
            
            config->toParent();
            
            return true;
        }
        return false;
    }
    
    void Player::setPosition(const ukn::Vector2& pos) {
        mMovable.position() = pos;
    }
    
    ukn::Vector2 Player::getPosition() const {
        return mMovable.position();
    }
    
    void Player::moveTo(const ukn::Vector2& pos) {
        mMovable.moveTo(pos, (pos-getPosition()).length() / mSpeed);
    }
    
    void Player::onUpdate() {
        mMovable.update();
    }
    
}
