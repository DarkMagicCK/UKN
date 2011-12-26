//
//  Skeletal.cpp
//  Project Unknown
//
//  Created by Ruiwei Bu on 12/25/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#include "UKN/Skeletal.h"
#include "UKN/Logger.h"
#include "UKN/Texture.h"
#include "UKN/Asset.h"
#include "UKN/ConfigParser.h"

namespace ukn {
    
    Bone::Bone(const ukn_string& name):
    mName(name) {
        // register properties for animation
        mAnimation.addProperty("x", &mPostion.x);
        mAnimation.addProperty("y", &mPostion.y);
        mAnimation.addProperty("length", &mLength);
        mAnimation.addProperty("angle", &mAngle);
    }
    
    Bone::~Bone() {
        
    }
    
    Bone::ChildrenList& Bone::getChildren() {
        return mChildren;
    }
    
    const Bone::ChildrenList& Bone::getChildren() const {
        return mChildren;
    }
    
    float Bone::getLength() const {
        return mLength;
    }
    
    float Bone::getAngle() const {
        return mAngle;
    }
    
    Vector2 Bone::getPosition() const {
        return mPostion;
    }
    
    void Bone::setLength(float length) {
        mLength = length;
    }
    
    void Bone::setAngle(float angle) {
        mAngle = angle;
    }
    
    void Bone::setPosition(const Vector2& pos) {
        mPostion = pos;
    }
    
    AnimationPlayer& Bone::getAnimation() {
        return mAnimation;
    }
    
    const AnimationPlayer& Bone::getAnimation() const {
        return mAnimation;
    }
    
    void Bone::setUserData(void* data) {
        mUserData = data;
    }
    
    void* Bone::getUserData() const {
        return mUserData;
    }
    
    BonePtr Bone::findChild(const ukn_string& name) const {
        ChildrenList::const_iterator it = mChildren.begin();
        while(it != mChildren.end()) {
            if((*it)->getName() == name) {
                return *it;
            }
            BonePtr next_child = (*it)->findChild(name);
            if(next_child)
                return next_child;
            ++it;
        }
        return BonePtr();
    }
        
    ukn_string Bone::getName() const {
        return mName;
    }
    
    size_t Bone::size() const {
        // self
        size_t size = 1;
        ChildrenList::const_iterator it = mChildren.begin();
        while(it != mChildren.end()) {
            size += (*it)->size();
            
            ++it;
        }
        return size;
    }
    
    void Bone::playAnimation(const ukn_string& name) {
        mAnimation.play(name);
        
        if(!mChildren.empty()) {
            ChildrenList::const_iterator it = mChildren.begin();
            while(it != mChildren.end()) {
                (*it)->playAnimation(name);
            
                ++it;
            }
        }
    }
    
    void Bone::stopAnimation() {
        mAnimation.stop();
        
        if(!mChildren.empty()) {
            ChildrenList::const_iterator it = mChildren.begin();
            while(it != mChildren.end()) {
                (*it)->stopAnimation();
                
                ++it;
            }
        }
    }
    
    TexturePtr Bone::getTexture() const {
        return mTexture;
    }
    
    void Bone::pauseAnimation() {
        mAnimation.pause();
        
        if(!mChildren.empty()) {
            ChildrenList::const_iterator it = mChildren.begin();
            while(it != mChildren.end()) {
                (*it)->pauseAnimation();
                
                ++it;
            }
        }
    }
    
    SkeletalAnimation::SkeletalAnimation() {
        // create root bone for all child bones
        // use for positioning
        mRoot = new Bone("root");
    }
    
    SkeletalAnimation::~SkeletalAnimation() {
        
    }
    
    bool SkeletalAnimation::serialize(const ConfigParserPtr& config) {
        // to do
        return false;
    }
    
    bool SkeletalAnimation::deserialize(const ConfigParserPtr& config) {
        if(config->toNode("skeletal")) {
            if(config->toFirstChild()) {
                do {
                    ukn_string bone_name = config->getString("name");
                    if(!bone_name.empty()) {
                        BonePtr bone = new Bone(bone_name);
                        
                        // parse basic properties
                        bone->setPosition(Vector2(config->getFloat("x"),
                                                  config->getFloat("y")));
                        bone->setLength(config->getFloat("length"));
                        bone->setAngle(config->getFloat("angle"));
                        
                        ukn_string texture_name = config->getString("texture");
                        bone->mTexture = AssetManager::Instance().load<Texture>(texture_name);
                        
                        // parse animation
                        bone->mAnimation.deserialize(config);
                        
                        // add bone
                        ukn_string parent = config->getString("parent");
                        addBone(bone, parent);
                    }
                    
                } while(config->toNextChild());
            } 
        }
        return true;
    }
    
    void SkeletalAnimation::play(const ukn_string& name) {
        mRoot->playAnimation(name);
    }
    
    void SkeletalAnimation::pause() {
        mRoot->pauseAnimation();
    }
    
    void SkeletalAnimation::stop() {
        mRoot->stopAnimation();
    }
    
    void SkeletalAnimation::setPosition(const Vector2& pos) {
        mRoot->setPosition(pos);
    }
    
    Vector2 SkeletalAnimation::getPosition() const {
        return mRoot->getPosition();
    }
    
    BonePtr SkeletalAnimation::getRoot() const {
        return mRoot;
    }
    
    BonePtr SkeletalAnimation::getBone(const ukn_string& name) const {
        if(mRoot->getName() == name)
            return mRoot;
        
        return mRoot->findChild(name);
    }
    
    void SkeletalAnimation::addBone(const BonePtr& bone, const ukn_string& parent_name) {
        BonePtr parent = getBone(parent_name.empty() ? "root" : parent_name);
        if(parent) {
            parent->getChildren().push_back(bone);
        } else 
            log_error(ukn_string("ukn::SkeletalAnimation:addBone: no parent bone with name ")+parent_name+" found");
    }
    
    void SkeletalAnimation::render(const SpriteBatch& spriteBatch) {
        
    }
    
    size_t SkeletalAnimation::size() const {
        if(mRoot) {
            return 1 + mRoot->size();
        }
        return 0;
    }
    
} // namespace ukn
 
