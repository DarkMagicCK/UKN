//
//  Animation.cpp
//  Project Unknown
//
//  Created by Ruiwei Bu on 12/22/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#include "UKN/Animation.h"
#include "UKN/TimeUtil.h"
#include "UKN/Singleton.h"

namespace ukn {
    
    Animation::Animation():
    mDuration(0) {
        
    }
    
    Animation::~Animation() {
        
    }
    
    void IntLinearAnimation::update(uint32 past_time, void* property_ptr) {
        *(int32*)property_ptr = (int32)(slerp((float)getFrom(), (float)getTo(), (real)past_time/getDuration())+0.5);
    }
    
    void UIntLinearAnimation::update(uint32 past_time, void* property_ptr) {
        *(uint32*)property_ptr = (uint32)(slerp((float)getFrom(), (float)getTo(), (real)past_time/getDuration())+0.5);
    }
    
    void DoubleLinearAnimation::update(uint32 past_time, void* property_ptr) {
         *(double*)property_ptr = slerp(getFrom(), getTo(), (real)past_time/getDuration());
    }
    
    void FloatLinearAnimation::update(uint32 past_time, void* property_ptr) {
         *(float*)property_ptr = slerp(getFrom(), getTo(), (real)past_time/getDuration());
    }
    
    void ColorLinearAnimation::update(uint32 past_time, void* property_ptr) {
        ((Color*)property_ptr)->r = slerp(getFrom().r, getTo().r, (real)past_time/getDuration());
        ((Color*)property_ptr)->g = slerp(getFrom().g, getTo().g, (real)past_time/getDuration());
        ((Color*)property_ptr)->b = slerp(getFrom().b, getTo().b, (real)past_time/getDuration());
        ((Color*)property_ptr)->a = slerp(getFrom().a, getTo().a, (real)past_time/getDuration());
    }
    
    void IntKeyFrameAnimation::update(uint32 past_time, void* property_ptr) {
        size_t timepos = getTimePos(past_time);
        if(0 < timepos && timepos < mKeyFrames.size()) {
            KeyFrame& kf = mKeyFrames[timepos-1];
            switch(kf.frame_type) {
                case KFT_Discrete:
                    *(int32*)property_ptr = kf.value;
                    break;
                case KFT_Linear:
                    *(int32*)property_ptr = (int32)(slerp((float)kf.value, 
                                                          (float)mKeyFrames[timepos].value,
                                                          (real)(past_time)/mKeyFrames[timepos].duration)+0.5);
                    break;
                    
            }
        } else if(timepos == 0) {
            
        } else {
            *(int32*)property_ptr = mKeyFrames[timepos-1].value;
        }
    }
    
    void UIntKeyFrameAnimation::update(uint32 past_time, void* property_ptr) {
        size_t timepos = getTimePos(past_time);
        if(0 < timepos && timepos < mKeyFrames.size()) {
            KeyFrame& kf = mKeyFrames[timepos-1];
            switch(kf.frame_type) {
                case KFT_Discrete:
                    *(uint32*)property_ptr = kf.value;
                    break;
                case KFT_Linear:
                    *(uint32*)property_ptr = (uint32)(slerp((float)kf.value,
                                                            (float)mKeyFrames[timepos].value, 
                                                            (real)(past_time)/mKeyFrames[timepos].duration)+0.5);
                    break;
                    
            }
        } else if(timepos == 0) {
            
        } else {
            *(uint32*)property_ptr = mKeyFrames[timepos-1].value;
        }
    }
    
    void DoubleKeyFrameAnimation::update(uint32 past_time, void* property_ptr) {
        size_t timepos = getTimePos(past_time);
        if(0 < timepos && timepos < mKeyFrames.size()) {
            KeyFrame& kf = mKeyFrames[timepos-1];
            switch(kf.frame_type) {
                case KFT_Discrete:
                    *(double*)property_ptr = kf.value;
                    break;
                case KFT_Linear:
                    *(double*)property_ptr = slerp(kf.value, 
                                                   mKeyFrames[timepos].value,
                                                   (real)(past_time)/mKeyFrames[timepos].duration);
                    break;
                    
            }
        } else if(timepos == 0) {
            
        } else {
            *(double*)property_ptr = mKeyFrames[timepos-1].value;
        }
    }
    
    void FloatKeyFrameAnimation::update(uint32 past_time, void* property_ptr) {
        size_t timepos = getTimePos(past_time);
        if(0 < timepos && timepos < mKeyFrames.size()) {
            KeyFrame& kf = mKeyFrames[timepos-1];
            switch(kf.frame_type) {
                case KFT_Discrete:
                    *(float*)property_ptr = kf.value;
                    break;
                case KFT_Linear:
                    *(float*)property_ptr = slerp(kf.value,
                                                  mKeyFrames[timepos].value, 
                                                  (real)(past_time)/mKeyFrames[timepos].duration);
                    break;
                    
            }
        } else if(timepos == 0) {
            
        } else {
            *(float*)property_ptr = mKeyFrames[timepos-1].value;
        }
    }
    
    void ColorKeyFrameAnimation::update(uint32 past_time, void* property_ptr) {
        size_t timepos = getTimePos(past_time);
        if(0 < timepos && timepos < mKeyFrames.size()) {
            KeyFrame& kf = mKeyFrames[timepos-1];
            switch(kf.frame_type) {
                case KFT_Discrete:
                    *(Color*)property_ptr = kf.value;
                    break;
                case KFT_Linear:
                    ((Color*)property_ptr)->r = slerp(kf.value.r, 
                                                      mKeyFrames[timepos].value.r, 
                                                      (real)(past_time)/mKeyFrames[timepos].duration);
                    
                    ((Color*)property_ptr)->g = slerp(kf.value.g, 
                                                      mKeyFrames[timepos].value.g, 
                                                      (real)(past_time)/mKeyFrames[timepos].duration);
                    
                    ((Color*)property_ptr)->b = slerp(kf.value.b, 
                                                      mKeyFrames[timepos].value.b,
                                                      (real)(past_time)/mKeyFrames[timepos].duration);
                    
                    ((Color*)property_ptr)->a = slerp(kf.value.a, 
                                                      mKeyFrames[timepos].value.a,
                                                      (real)(past_time)/mKeyFrames[timepos].duration);
                    break;
                    
            }
        } else if(timepos == 0) {
            
        } else {
            *(Color*)property_ptr = mKeyFrames[timepos-1].value;
        }
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
    
} // namespace ukn
