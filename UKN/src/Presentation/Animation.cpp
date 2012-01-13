//
//  Animation.cpp
//  Project Unknown
//
//  Created by Ruiwei Bu on 12/22/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#include "UKN/Animation.h"
#include "UKN/TimeUtil.h"
#include "UKN/ConfigParser.h"
#include "UKN/Singleton.h"

namespace ukn {
    
    Animation::Animation():
    mDuration(0) {
        
    }
    
    Animation::~Animation() {
        
    }
        
    // storyboard
    StoryBoard::StoryBoard():
    mDuration(0),
    mStatus(AS_Stop),
    mLastUpdateTime(0) {
        
    }
    
    StoryBoard::~StoryBoard() {
        setStatus(AS_Stop);
    }
    
    StoryBoard::AnimationSet& StoryBoard::children() {
        return mAnimationSet;
    }
    
    const StoryBoard::AnimationSet& StoryBoard::children() const {
        return mAnimationSet;
    }
    
    AnimationStatus StoryBoard::getStatus() const {
        return mStatus;
    }
    
    void StoryBoard::setStatus(AnimationStatus status) {
        switch(status) {
            case AS_Playing:
                if(AS_Stop == mStatus) {
                    mStoryBoardStartTime = FrameCounter::Instance().getRunningTime();
                    getDuration();
                    mLastUpdateTime = 0;
                    
                } else if(AS_Paused == mStatus) {
                    mStoryBoardStartTime = FrameCounter::Instance().getRunningTime() - (mStoryBoardPauseTime - mStoryBoardStartTime);
                }
                mStatus = AS_Playing;
                StoryBoardManager::Instance().addStoryBoard(this);
                break;
                
            case AS_Paused:
                if(AS_Playing == mStatus) {
                    mStoryBoardPauseTime = FrameCounter::Instance().getRunningTime();
                    mStatus = AS_Paused;
                }
                break;
                
            case AS_Stop:
                if(AS_Playing == mStatus || AS_Paused == mStatus) {
                    mStatus = AS_Stop;
                    StoryBoardManager::Instance().delStoryBoard(this);

                    StoryBoardCompletedEventArgs args;
                    args.pass_time = (uint32)(FrameCounter::Instance().getRunningTime() - mStoryBoardStartTime);
                    mCompleteEvent.raise(this, args);
                }
                break;
        }
    }
    
    void StoryBoard::play() {
        setStatus(AS_Playing);
    }
    
    void StoryBoard::pause() {
        setStatus(AS_Paused);
    }
    
    void StoryBoard::stop() {
        setStatus(AS_Stop);
    }
    
    uint32 StoryBoard::getDuration() {
        uint32 max_duration = 0;
        AnimationSet::const_iterator it = mAnimationSet.begin();
        for(; it != mAnimationSet.end(); ++it) {
            if(it->start_time + it->animation_ptr->getDuration() > max_duration) {
                max_duration = it->start_time + it->animation_ptr->getDuration();
            }
        }
        if(mDuration < max_duration)
            mDuration = max_duration;
        return mDuration;
    }
    
    void StoryBoard::setDuration(uint32 ms) {
        getDuration();
        if(ms > mDuration)
            mDuration = ms;
    }
    
    void StoryBoard::update(uint64 now_time) {
        if(mStatus != AS_Playing) 
            return;
        
        uint64 pass_time = now_time - mStoryBoardStartTime;
        uint64 animation_end_time;
        AnimationSet::const_iterator it = mAnimationSet.begin();
        for(; it != mAnimationSet.end(); ++it) {
            animation_end_time = it->start_time + it->animation_ptr->getDuration();
            if(it->start_time < pass_time) {
                if(pass_time <= animation_end_time) {
                    it->animation_ptr->update((uint32)(pass_time - it->start_time), it->property_ptr);
                } else {
                    it->animation_ptr->update((uint32)(animation_end_time - it->start_time), it->property_ptr);
                }
            }
        }
        mLastUpdateTime = pass_time;
        if(pass_time > mDuration) {
            mStatus = AS_Stop;
            StoryBoardCompletedEventArgs args;
            args.pass_time = mDuration;
            mCompleteEvent.raise(this, args);
        }
    }
    
    Event<StoryBoardCompletedEventArgs>& StoryBoard::onComplete() {
        return mCompleteEvent;
    }
    
    StoryBoardManager::StoryBoardManager() {
        
    }
    
    StoryBoardManager::~StoryBoardManager() {
        
    }
    
    StoryBoardManager& StoryBoardManager::Instance() {
        static StoryBoardManager instance;
        return instance;
    }
    
    void StoryBoardManager::addStoryBoard(StoryBoard* sb) {
        mStoryBoards.push_back(sb);
    }
    
    void StoryBoardManager::delStoryBoard(StoryBoard* sb) {
        mStoryBoards.remove(sb);
    }
    
    void StoryBoardManager::onUpdate() {
        uint64 curr_time = FrameCounter::Instance().getRunningTime();
        
        StoryBoardList::iterator it = mStoryBoards.begin();
        StoryBoardList::iterator end = mStoryBoards.end();
        
        while(it != end) {
            if((*it)->getStatus() == AS_Stop) {
                it = mStoryBoards.erase(it);
            } else {
                (*it)->update(curr_time);
                ++it;
            }
        }
    }
    
    StoryBoardManager::StoryBoardList& StoryBoardManager::getStoryBoardList() {
        return mStoryBoards;
    }
    
    const StoryBoardManager::StoryBoardList& StoryBoardManager::getStoryBoardList() const {
        return mStoryBoards;
    }
    
    UKN_STATIC_RUN_CODE(StoryBoardManager::Instance());
    
    AnimationPlayer::AnimationPlayer() {
        
    }
    
    AnimationPlayer::~AnimationPlayer() {
        
    }
    
    bool AnimationPlayer::serialize(const ConfigParserPtr& parser) {
        // cannot serialize a story board
        return false;
    }
    
    inline void create_linear_animation(const ukn_string& type, void* prop, const ConfigParserPtr& parser, StoryBoard* storyboard) {
        if(type == "int") {
            LinearAnimation<int>* anim = new LinearAnimation<int>;
            anim->setFrom(parser->getInt("from", 0));
            anim->setTo(parser->getInt("to", 0));
            anim->setDuration(parser->getInt("duration", 0));
            storyboard->children().push_back(StoryBoard::AnimationInfo(parser->getInt("time", 0), 
                                                                       anim,
                                                                       prop));
            return;
        }
        
        if(type == "float") {
            LinearAnimation<float>* anim = new LinearAnimation<float>;
            anim->setFrom(parser->getFloat("from", 0));
            anim->setTo(parser->getFloat("to", 0));
            anim->setDuration(parser->getInt("duration", 0));
            storyboard->children().push_back(StoryBoard::AnimationInfo(parser->getInt("time", 0), 
                                                                       anim,
                                                                       prop));
            return;
        }
        
        if(type == "uint") {
            LinearAnimation<uint32>* anim = new LinearAnimation<uint32>;
            anim->setFrom(parser->getInt("from", 0));
            anim->setTo(parser->getInt("to", 0));
            anim->setDuration(parser->getInt("duration", 0));
            storyboard->children().push_back(StoryBoard::AnimationInfo(parser->getInt("time", 0), 
                                                                       anim,
                                                                       prop));
            return;
        }
        
        if(type == "double") {
            LinearAnimation<double>* anim = new LinearAnimation<double>;
            anim->setFrom(parser->getFloat("from", 0));
            anim->setTo(parser->getFloat("to", 0));
            anim->setDuration(parser->getInt("duration", 0));
            storyboard->children().push_back(StoryBoard::AnimationInfo(parser->getInt("time", 0), 
                                                                       anim,
                                                                       prop));
            return;
        }
        
        if(type == "color") {
            // to do
            return;
        }
    }
    
    inline void create_key_frame_animation(const ukn_string& type, void* prop, const ConfigParserPtr& parser, StoryBoard* storyboard) {
        
        if(type == "int") {
            KeyFrameAnimation<int>* anim = new KeyFrameAnimation<int>;
            
            int32 time = parser->getInt("time", 0);
            int32 duration = parser->getInt("duration", 0);
            
            if(duration == 0)
                return;
            anim->setDuration(duration);
            
            if(parser->toFirstChild()) {
                do {
                    if(parser->hasAttribute("val")) {
                        
                        int32 val = parser->getInt("val", 0);
                        int32 dur = parser->getInt("duration", 0);
                        int32 type = parser->getInt("type", 0);
                    
                        anim->getKeyFrames().push_back(KeyFrameAnimation<int>::KeyFrame(val, 
                                                                                      dur, 
                                                                                      type == 0 ? KFT_Linear : KFT_Discrete));
                    }
                    
                } while(parser->toNextChild());
                
                storyboard->children().push_back(StoryBoard::AnimationInfo(time, 
                                                                           anim,
                                                                           prop));
            }
            return;
        } 
        
        if(type == "float") {
            KeyFrameAnimation<float>* anim = new KeyFrameAnimation<float>;
            
            int32 time = parser->getInt("time", 0);
            int32 duration = parser->getInt("duration", 0);
            
            if(duration == 0)
                return;
            anim->setDuration(duration);
            
            if(parser->toFirstChild()) {
                do {
                    if(parser->hasAttribute("val")) {
                        
                        float val = parser->getFloat("val", 0);
                        int32 dur = parser->getInt("duration", 0);
                        int32 type = parser->getInt("type", 0);
                        
                        anim->getKeyFrames().push_back(KeyFrameAnimation<float>::KeyFrame(val, 
                                                                                        dur, 
                                                                                        type == 0 ? KFT_Linear : KFT_Discrete));
                    }
                    
                } while(parser->toNextChild());
                
                storyboard->children().push_back(StoryBoard::AnimationInfo(time, 
                                                                           anim,
                                                                           prop));
            }
            return;
        } 
        
        if(type == "double") {
            KeyFrameAnimation<double>* anim = new KeyFrameAnimation<double>;
            
            int32 time = parser->getInt("time", 0);
            int32 duration = parser->getInt("duration", 0);
            
            if(duration == 0)
                return;
            anim->setDuration(duration);
            
            if(parser->toFirstChild()) {
                do {
                    if(parser->hasAttribute("val")) {
                        int32 val = parser->getInt("val", 0);
                        int32 dur = parser->getInt("duration", 0);
                        int32 type = parser->getInt("type", 0);
                        
                        anim->getKeyFrames().push_back(KeyFrameAnimation<double>::KeyFrame(val, 
                                                                                         dur, 
                                                                                         type == 0 ? KFT_Linear : KFT_Discrete));
                    }
                    
                } while(parser->toNextChild());
                
                storyboard->children().push_back(StoryBoard::AnimationInfo(time, 
                                                                           anim,
                                                                           prop));
            }
            return;
        } 
        
        if(type == "uint") {
            KeyFrameAnimation<uint32>* anim = new KeyFrameAnimation<uint32>;
            
            int32 time = parser->getInt("time", 0);
            int32 duration = parser->getInt("duration", 0);
            
            if(duration == 0)
                return;
            anim->setDuration(duration);
            
            if(parser->toFirstChild()) {
                do {
                    if(parser->hasAttribute("val")) {
                        
                        int32 val = parser->getInt("val", 0);
                        int32 dur = parser->getInt("duration", 0);
                        int32 type = parser->getInt("type", 0);
                    
                        anim->getKeyFrames().push_back(KeyFrameAnimation<uint32>::KeyFrame(val, 
                                                                                       dur, 
                                                                                       type == 0 ? KFT_Linear : KFT_Discrete));
                        
                    }
                    
                } while(parser->toNextChild());
                
                storyboard->children().push_back(StoryBoard::AnimationInfo(time, 
                                                                           anim,
                                                                           prop));
            }
            return;
        } 
        
        
        if(type == "color") {
            // to do
        } 
    }
    
    bool AnimationPlayer::deserialize(const ConfigParserPtr& parser) {
        if(parser->toNode("animations")) {
            if(parser->toFirstChild()) {
                do {
                    StoryBoardPtr storyboard = new StoryBoard;
                    ukn_string name = parser->getString("name");
                    if(!name.empty()) {
                        bool isdefault = parser->getBool("default", false);
                        if(isdefault)
                            setDefault(name);
                        
                        if(parser->toFirstChild()) {
                            do {
                                ukn_string type = parser->getCurrentNodeName();
                                
                                // linear animation
                                if(type == "linear") {
                                    ukn_string prop_name = parser->getString("prop");
                                    ukn_string prop_type = parser->getString("prop_type");
                                    
                                    void* prop = getProperty(prop_name);
                                    if(prop) {
                                        create_linear_animation(prop_type, prop, parser, storyboard);
                                    }
                                    
                                // keyframe animation
                                } else if(type == "keyframe") {
                                    ukn_string prop_name = parser->getString("prop");
                                    ukn_string prop_type = parser->getString("prop_type");
                                    
                                    void* prop = getProperty(prop_name);
                                    if(prop) {
                                        create_key_frame_animation(prop_type, prop, parser, storyboard);
                                      
                                        // back to parent node
                                        parser->toParent();
                                    }
                                }
                                
                            } while(parser->toNextChild());
                            
                            // back to parent node
                            parser->toParent();
                            
                            // add storyboard
                            mStoryBoards[name] = storyboard;
                            // connect to finish event
                            storyboard->onComplete() += Bind(this, &AnimationPlayer::onAnimationFinished);
                        }
                            
                    }
                    
                } while(parser->toNextChild());
                
                parser->toParent();
            }
            
            parser->toParent();
        } else
            return false;
        
        if(!mDefault.empty())
            play(mDefault);
        
        return true;
    }
    
    bool AnimationPlayer::isStoryBoardExist(const ukn_string& name) const {
        return mStoryBoards.find(name) != mStoryBoards.end();
    }
    
    StoryBoardPtr AnimationPlayer::getStoryBoard(const ukn_string& name) const {
        StoryBoardMap::const_iterator it = mStoryBoards.find(name);
        return it != mStoryBoards.end() ? it->second : StoryBoardPtr();
    }
    
    void AnimationPlayer::play(const ukn_string& name) {
        StoryBoardMap::const_iterator it = mStoryBoards.find(name);
        if(it != mStoryBoards.end()) {
            it->second->play();
            mCurrent = name;
        }
    }
    
    void AnimationPlayer::pause() {
        StoryBoardMap::const_iterator it = mStoryBoards.find(mCurrent);
        if(it != mStoryBoards.end())
            it->second->pause();
    }
    
    void AnimationPlayer::stop() {
        StoryBoardMap::const_iterator it = mStoryBoards.find(mCurrent);
        if(it != mStoryBoards.end())
            it->second->stop();
    }
    
    void AnimationPlayer::addProperty(const ukn_string& prop_name, void* prop) {
        mProperties[prop_name] = prop;
    }
    
    void AnimationPlayer::delProperty(const ukn_string& name) {
        PropertyMap::iterator it = mProperties.find(name);
        if(it != mProperties.end())
            mProperties.erase(it);
    }
    
    void* AnimationPlayer::getProperty(const ukn_string& name) const {
        PropertyMap::const_iterator it = mProperties.find(name);
        return it != mProperties.end() ? it->second : 0;
    }
    
    size_t AnimationPlayer::size() const {
        return mStoryBoards.size();
    }
    
    Event<StoryBoardCompletedEventArgs>& AnimationPlayer::onComplete() {
        return mCompleteEvent;
    }
    
    void AnimationPlayer::setDefault(const ukn_string& name) {
        mDefault = name;
    }
    
    ukn_string AnimationPlayer::getDefault() const {
        return mDefault;
    }
    
    void AnimationPlayer::onAnimationFinished(void* sender, StoryBoardCompletedEventArgs& args) {
        if(!mDefault.empty()) {
            play(mDefault);
        }
    }
    
    ukn_string AnimationPlayer::getCurrent() const {
        return mCurrent;
    }
    
    
} // namespace ukn
