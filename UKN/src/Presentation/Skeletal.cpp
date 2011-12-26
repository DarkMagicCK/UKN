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
    mName(name),
    mParent(0),
    mLength(0),
    mAngle(0),
    mBaseAngle(0),
    mTextureAngle(0),
    mMaxAngle(pi * 2),
    mMinAngle(-pi * 2),
    mPostion(Vector2()),
    mUserData(0),
    mAbsoluteAngle(false),
    mAbsolutePosition(false),
    mScale(Vector2(1.f, 1.f)) {
        // register properties for animation
        mAnimation.addProperty("x", &mPostion.x);
        mAnimation.addProperty("y", &mPostion.y);
        mAnimation.addProperty("length", &mLength);
        mAnimation.addProperty("angle", &mAngle);
        mAnimation.addProperty("scalex", &mScale.x);
        mAnimation.addProperty("scaley", &mScale.y);
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
        if(mAbsoluteAngle)
            return mAngle;
        else
            return mAngle + (mParent != 0 ? mParent->getAngle() : 0);
    }
    
    Vector2 Bone::getPosition() const {
        if(mParent && !mAbsolutePosition) {
            Vector2 v2 = mParent->getPosition() + Vector2(mParent->mLength * cosf(mParent->getAngle()),
                                                          -mParent->mLength * sinf(mParent->getAngle()));
            return mPostion + v2;
        }
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
    
    void Bone::render(SpriteBatch& sprBatch, const Vector2& basePos) {
        if(mTexture) {
            Vector2 pos = basePos + getPosition() + mOffset;
            printf("%s, (%f, %f), %f, %f, %f\n", mName.c_str(), pos.x, pos.y, getAngle(), mBaseAngle, mTextureAngle);
            
            float angle = getAngle() - mBaseAngle + mTextureAngle;
            if(angle > mMaxAngle) angle = mMaxAngle;
            if(angle < mMinAngle) angle = mMinAngle;
            sprBatch.draw(mTexture, 
                          pos.x, 
                          pos.y,
                          mBasePoint.x, 
                          mBasePoint.y,
                          angle,
                          1.f,
                          1.f);
        }
        
        if(!mChildren.empty()) {
            ChildrenList::const_iterator it = mChildren.begin();
            while(it != mChildren.end()) {
                (*it)->render(sprBatch, basePos);
                
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
                        BonePtr bone;

                        if(bone_name != "root") {
                            bone = new Bone(bone_name);
                        } else {
                            bone = mRoot;
                        }
                           
                        // parse basic properties
                        bone->setPosition(Vector2(config->getFloat("x"),
                                                  config->getFloat("y")));
                        bone->setLength(config->getFloat("length"));
                        bone->setAngle(config->getFloat("angle"));
                                                
                        bone->mOffset = Vector2(config->getFloat("offx"),
                                                config->getFloat("offy"));
                        
                        bone->mTextureAngle = config->getFloat("texture_angle");
                        
                        bone->mBaseAngle = bone->getAngle();
                        
                        bone->mAbsolutePosition = config->getBool("absolute_pos");
                        bone->mAbsoluteAngle = config->getBool("absolute_angle");
                        
                        bone->mBasePoint = Vector2(config->getFloat("basex"),
                                                   config->getFloat("basey"));
                        
                        ukn_string texture_name = config->getString("texture");
                        bone->mTexture = AssetManager::Instance().load<Texture>(texture_name);
                        
                        // parse animation
                        bone->mAnimation.deserialize(config);
                        
                        // add bone
                        if(bone_name != "root") {
                            ukn_string parent = config->getString("parent");
                            addBone(bone, parent);
                        }
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
        mPosition = pos;
    }
    
    Vector2 SkeletalAnimation::getPosition() const {
        return mPosition;
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
            bone->mParent = parent;
        } else 
            log_error(ukn_string("ukn::SkeletalAnimation:addBone: no parent bone with name ")+parent_name+" found");
    }
    
    void SkeletalAnimation::render(SpriteBatch& spriteBatch) {
        if(mRoot) {
            mRoot->render(spriteBatch, mPosition);
        }
    }
    
    size_t SkeletalAnimation::size() const {
        if(mRoot) {
            return 1 + mRoot->size();
        }
        return 0;
    }
    
} // namespace ukn
 
