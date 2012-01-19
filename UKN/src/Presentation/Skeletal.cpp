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
#include "UKN/TimeUtil.h"

namespace ukn {
    
    BoneAnimation::BoneAnimation():
    name(ukn_string()),
    repeat_count(1),
    is_default(false),
    mCurrentTime(0),
    mCurrentFrameIndex(-1),
    mCurrentRepeatCount(0),
    mCurrentStatus(AS_Stopped),
    mCurrentTransform(BoneKeyFrameData()),
    mTotalPlayedTime(0),
    mPausedTime(0) {
        
    }
    
    BoneAnimation::~BoneAnimation() {
    
    }
    
    void BoneAnimation::play() {
        setStatus(AS_Playing);
    }
    
    void BoneAnimation::stop() {
        setStatus(AS_Stopped);
    }
    
    void BoneAnimation::pause() {
        setStatus(AS_Paused);
    }
    
    void BoneAnimation::update(uint32 deltaTime) {
        switch(mCurrentStatus) {
            case AS_Playing: {
                mCurrentTime += deltaTime;
                mTotalPlayedTime += deltaTime;
                
                BoneKeyFrameData& data = key_frames[mCurrentFrameIndex];
                                
                if(mCurrentTime >= data.duration) {
                    mCurrentTime = 0;
                    
                    ++mCurrentFrameIndex;
                    if(mCurrentFrameIndex >= key_frames.size()-1) {
                        mCurrentFrameIndex = 0;
                        
                        ++mCurrentRepeatCount;
                        
                        if(mCurrentRepeatCount > repeat_count) {
                            setStatus(AS_Stopped);
                        }
                    }
                } else {
                    mCurrentTransform = lerp(data,
                                             key_frames[mCurrentFrameIndex+1],
                                             (float) mCurrentTime / data.duration);
                    
                }
                break;
            }
                
            case AS_Paused:
                mPausedTime += deltaTime;
                break;
                
            default:   
                break;
        }
    }
    
    size_t BoneAnimation::getTimePos(uint32 time) const {
        uint32 prevTime = 0;
        size_t i = 0;
        while(i < key_frames.size() && prevTime + key_frames[i].duration < time) {
            prevTime += key_frames[i].duration;
            ++i;
        }
        return i;
    }
    
    size_t BoneAnimation::getTimePos(uint32 time, uint32& prevTime) const {
        prevTime = 0;
        size_t i = 0;
        while(i < key_frames.size() && prevTime + key_frames[i].duration < time) {
            prevTime += key_frames[i].duration;
            ++i;
        }
        return i;
    }
    
    uint32 BoneAnimation::getDuration() const {
        uint32 duration = 0;
        UKN_FOR_EACH(BoneKeyFrameData& data, key_frames) {
            duration += data.duration;
        }
        return duration;
    }
    
    uint32 BoneAnimation::getPausedTime() const {
        return mPausedTime;
    }
    
    uint32 BoneAnimation::getTotalPlayedTime() const {
        return mTotalPlayedTime;
    }
    
    AnimationStatus BoneAnimation::getStatus() const {
        return mCurrentStatus;
    }
    
    const BoneKeyFrameData& BoneAnimation::getCurrentFrameData() const {
        return mCurrentTransform;
    }
    
    void BoneAnimation::setStatus(AnimationStatus status) {        
        switch(status) {
            case AS_Playing:
                if(mCurrentStatus != AS_Paused) {
                    if(key_frames.size() > 0) {
                        mCurrentTime = 0;
                        mCurrentFrameIndex = 0;
                        mCurrentRepeatCount = 0;
                    
                        mCurrentTransform = key_frames[0];
                
                        mPausedTime = 0;
                        mTotalPlayedTime = 0;
                    } else 
                        mCurrentStatus = AS_Stopped;
                } else {
                    mPausedTime = 0;
                }
                mCurrentStatus = status;

                break;
                
            case AS_Paused:
                mPausedTime = 0;
                mCurrentStatus = status;
                break;
                
            case AS_Stopped:   
                mCurrentTime = 0;
                mCurrentFrameIndex = -1;
                mPausedTime = 0;
                mCurrentRepeatCount = 0;
                mTotalPlayedTime = 0;
                mCurrentStatus = status;
                
                BoneAnimationCompleteArgs args(mTotalPlayedTime);
                complete_event.raise(this, args);
                break;
        }
    }
    
    Bone::Bone(const ukn_string& name):
    mName(name),
    mParent(0),
    mLength(0),
    mRotation(0),
    mOpacity(1.f),
    mMaxAngle(pi * 2),
    mMinAngle(-pi * 2),
    mPosition(Vector2()),
    mScale(Vector2(1.f, 1.f)),
    mInheritRotation(true),
    mInheritPosition(true),
    mInheritVisibility(true),
    mInheritScale(true),
    mInheritOpacity(true),
    mUserData(0)
	{
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
    
    float Bone::getRotation() const {
        if(mInheritRotation && mParent) {
            if(mCurrentAnimation != mAnimations.end()) {
                return mCurrentAnimation->second.getCurrentFrameData().rotation + mParent->getRotation();
            }
            return mRotation + mParent->getRotation();
        }
        if(mCurrentAnimation != mAnimations.end()) {
            return mCurrentAnimation->second.getCurrentFrameData().rotation;
        }
        return mRotation;
    }
    
    float Bone::getOpacity() const {
        if(mInheritOpacity && mParent) {
            if(mCurrentAnimation != mAnimations.end()) {
                return mCurrentAnimation->second.getCurrentFrameData().opacity + mParent->getOpacity();
            }
            return mRotation + mParent->getOpacity();
        }
        if(mCurrentAnimation != mAnimations.end()) {
            return mCurrentAnimation->second.getCurrentFrameData().opacity;
        }
        return mRotation;
    }
    
    Vector2 Bone::getScale() const {
        if(mInheritScale && mParent) {
            if(mCurrentAnimation != mAnimations.end()) {
                return mCurrentAnimation->second.getCurrentFrameData().scale * mParent->getScale();
            }
            return mScale * mParent->getScale();
        }
        if(mCurrentAnimation != mAnimations.end()) {
            return mCurrentAnimation->second.getCurrentFrameData().scale;
        }
        return mScale;
    }
    
    Vector2 Bone::getTextureCenter() const {
        return mTextureCenter;
    }
    
    Vector2 Bone::getPosition() const {
        if(!mParent)
            return mPosition;
        
        if(mInheritPosition && mParent) {
            if(mCurrentAnimation != mAnimations.end()) {
                return mCurrentAnimation->second.getCurrentFrameData().position + mParent->getPosition();
            }
            return mPosition + mParent->getPosition();
        }
        if(mCurrentAnimation != mAnimations.end()) {
            return mCurrentAnimation->second.getCurrentFrameData().position;
        }
        return mPosition;
    }
    
    bool Bone::isVisible() const {
        if(mInheritVisibility && mParent) {
            if(mCurrentAnimation != mAnimations.end()) {
                return mCurrentAnimation->second.getCurrentFrameData().visible & mParent->isVisible();
            }
            return mVisible & mParent->isVisible();
        }
        if(mCurrentAnimation != mAnimations.end()) {
            return mCurrentAnimation->second.getCurrentFrameData().visible;
        }
        return mVisible;
    }
    
    void Bone::setLength(float length) {
        mLength = length;
    }
    
    void Bone::setRotation(float rotation) {
        mRotation = rotation;
    }
    
    void Bone::setPosition(const Vector2& pos) {
        mPosition = pos;
    }
    
    void Bone::setVisible(bool flag) {
        mVisible = flag;
    }
    
    void Bone::setScale(const Vector2& scale) {
        mScale = scale;
    }
    
    void Bone::setTextureCenter(const Vector2& center) {
        mTextureCenter = center;
    }
    
    void Bone::setOpacity(float opacity) {
        mOpacity = opacity;
    }
    
    void Bone::setLayerDepth(float depth) {
        mLayerDepth = depth;
    }
    
    float Bone::getLayerDepth() const {
        return mLayerDepth;
    }
    
    Bone::AnimationMap& Bone::getAnimation() {
        return mAnimations;
    }
    
    const Bone::AnimationMap& Bone::getAnimation() const {
        return mAnimations;
    }
    
    void Bone::setUserData(void* data) {
        mUserData = data;
    }
    
    void* Bone::getUserData() const {
        return mUserData;
    }
    
    bool Bone::isInheritRotation() const {
        return mInheritRotation;
    }
    
    bool Bone::isInheritPosition() const {
        return mInheritPosition;
    }
    
    bool Bone::isInheritVisibility() const {
        return mInheritVisibility;
    }
    
    bool Bone::isInheritScale() const {
        return mInheritScale;
    }
    
    void Bone::setInheritRotation(bool flag) {
        mInheritRotation = flag;
    }
    
    void Bone::setInheritPosition(bool flag) {
        mInheritPosition = flag;
    }
    
    void Bone::setInheritVisibility(bool flag) {
        mInheritVisibility = flag;
    }
    
    void Bone::setInheritScale(bool flag) {
        mInheritScale = flag;
    }
    
    void Bone::setInheritOpacity(bool flag) {
        mInheritOpacity = flag;
    }
    
    void Bone::update(uint32 past_time) {
		if(mAnimations.empty())
			return;

        if(mCurrentAnimation != mAnimations.end())
            mCurrentAnimation->second.update(past_time);
        
        UKN_FOR_EACH(BonePtr& ptr, mChildren) {
            ptr->update(past_time);
        }
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
    
    size_t Bone::getSize() const {
        // self
        size_t size = 1;
        ChildrenList::const_iterator it = mChildren.begin();
        while(it != mChildren.end()) {
            size += (*it)->getSize();
            
            ++it;
        }
        return size;
    }
    
    void Bone::playAnimation(const ukn_string& name) {
        mCurrentAnimation = mAnimations.find(name);
        if(mCurrentAnimation != mAnimations.end())
            mCurrentAnimation->second.play();
        //mAnimation.play();
        
        UKN_FOR_EACH(BonePtr& ptr, mChildren) {
            ptr->playAnimation(name);
        }
    }
    
    void Bone::stopAnimation() {
        if(mCurrentAnimation != mAnimations.end())
           mCurrentAnimation->second.stop();
        
        UKN_FOR_EACH(BonePtr& ptr, mChildren) {
            ptr->stopAnimation();
        }
    }
    
    TexturePtr Bone::getTexture() const {
        return mTexture;
    }
    
    void Bone::pauseAnimation() {
        if(mCurrentAnimation != mAnimations.end())
            mCurrentAnimation->second.pause();
        
        UKN_FOR_EACH(BonePtr& ptr, mChildren) {
            ptr->pauseAnimation();
        }
    }
    
    SkeletalAnimation::SkeletalAnimation() {
        // create root bone for all child bones
        // use for positioning
        mRoot = new Bone("Root");
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

                        if(bone_name != "Root") {
                            bone = new Bone(bone_name);
                        } else {
                            bone = mRoot;
                        }
                        
                        ukn_string texture_path;
                        if(config->hasAttribute("texture_path")) {
                            texture_path = config->getString("texture_path");
                        } else if(config->hasAttribute("editor_texture_path")) {
                            texture_path = config->getString("editor_texture_path");
                        } else if(config->hasAttribute("texture")) {
                            texture_path = config->getString("texture");
                        }
                        if (!texture_path.empty()) {
                            bone->mTexture = AssetManager::Instance().load<Texture>(texture_path);
                        }
                           
                        // parse basic properties
                        bone->setPosition(Vector2(config->getFloat("x"),
                                                  config->getFloat("y")));
                        bone->setScale(Vector2(config->getFloat("scale_x", 1.f),
                                               config->getFloat("scale_y", 1.f)));
                        bone->setTextureCenter(Vector2(config->getFloat("texture_center_x"),
                                                       config->getFloat("texture_center_y")));
                                       
                        if(config->hasAttribute("length"))
                            bone->setLength(config->getFloat("length"));
                        else
                            bone->setLength(50.f);
                        
                        bone->setRotation(config->getFloat("rotation"));
                        bone->setVisible(config->getBool("visible", true));
                        bone->setOpacity(config->getFloat("opacity", 1.f));
                        bone->setLayerDepth(config->getFloat("layer_depth", 0.f));
                   
                        bone->mInheritVisibility = config->getBool("inherit_visibility");
                        bone->mInheritRotation = config->getBool("inherit_rotation");
                        bone->mInheritPosition = config->getBool("inherit_position");
                        bone->mInheritScale = config->getBool("inherit_scale");
                        bone->mInheritOpacity = config->getBool("inherit_opacity");
                                                
                        // parse animation
                        // bone->mAnimation.deserialize(config);
                        if(config->toNode("Animations")) {
                            if(config->toFirstChild()) {
                                do {
                                    ukn_string anim_name = config->getString("name");
                                    if(!anim_name.empty()) {
                                        BoneAnimation anim;
                                        anim.name = anim_name;
                                        anim.repeat_count = config->getInt("repeat_count");
                                        anim.is_default = config->getBool("default");
                                        if(anim.is_default)
                                            mDefaultAnimation = anim.name;
                                        
                                        if(config->toFirstChild()) {
                                            do {
                                                uint32 duration = config->getInt("duration");
                                                if(duration != 0) {
                                                    BoneKeyFrameData data;
                                                    data.duration = duration;
                                                    data.layer_depth = config->getFloat("layer_depth", 0.f);
                                                    data.visible = config->getBool("visible", true);
                                                    data.opacity = config->getFloat("opacity", 1.f);
                                                    data.rotation = config->getFloat("rotation");
                                                    
                                                    data.position = Vector2(config->getFloat("x"),
                                                                            config->getFloat("y"));
                                                    data.scale = Vector2(config->getFloat("scale_x", 1.f),
                                                                        config->getFloat("scale_y", 1.f));
                                                    
                                                    anim.key_frames.push_back(data);
                                                }
                                            } while(config->toNextChild());
                                            
                                            
                                            // to animatiosn
                                            config->toParent();
                                        }
                                        
                                        if(bone == mRoot) {
                                            std::pair<Bone::AnimationMap::iterator, bool> it = bone->getAnimation().insert(std::make_pair(anim_name, anim));
                                            it.first->second.complete_event += Bind(this, &SkeletalAnimation::onAnimationComplete);
                                        } else {
                                            bone->getAnimation().insert(std::make_pair(anim_name, anim));
                                        }
                                    }
                                    
                                } while(config->toNextChild());
                                
                                // to animations
                                config->toParent();
                            }
                            
                            // to bone
                            config->toParent();
                        }
                        
                        // add bone
                        if(bone_name != "Root") {
                            ukn_string parent = config->getString("parent");
                            addBone(bone, parent);
                        }
                    }
                    
                } while(config->toNextChild());
                
                // to skeletal
                // recover environment for possible future usage
                config->toParent();
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
        BonePtr parent = getBone(parent_name.empty() ? "Root" : parent_name);
        if(parent) {
            parent->getChildren().push_back(bone);
            bone->mParent = parent;
        } else 
            log_error(ukn_string("ukn::SkeletalAnimation:addBone: no parent bone with name ")+parent_name+" found");
    }
    
    inline void render_bone(SpriteBatch& spriteBatch, Bone& bone) {
        if(bone.getTexture()) {
            spriteBatch.draw(bone.getTexture(),
                             bone.getPosition(),
                             bone.getTextureCenter(),
                             bone.getRotation() * pi / 180.f,
                             bone.getScale(),
                             bone.getLayerDepth());
        }
        
        UKN_FOR_EACH(BonePtr& ptr, bone.getChildren()) {
            render_bone(spriteBatch, *ptr.get()); 
        }
    }
    
    void SkeletalAnimation::render(SpriteBatch& spriteBatch) {
        if(mRoot) {
            render_bone(spriteBatch, *mRoot.get());
        }
    }
    
    void SkeletalAnimation::onAnimationComplete(void* sender, BoneAnimationCompleteArgs& args) {
        if(!mDefaultAnimation.empty()) {
            play(mDefaultAnimation);
        }
    }
    
    void SkeletalAnimation::update() {
        mRoot->update((uint32)(FrameCounter::Instance().getPrevDelta() * 1000));
    }
    
    size_t SkeletalAnimation::getSize() const {
        if(mRoot) {
            return 1 + mRoot->getSize();
        }
        return 0;
    }
    
} // namespace ukn
 