//
//  SequencialAnimation.cpp
//  Project Unknown
//
//  Created by Ruiwei Bu on 1/17/12.
//  Copyright (c) 2012 heizi. All rights reserved.
//

#include "UKN/SequencialAnimation.h"
#include "UKN/TimeUtil.h"
#include "UKN/Texture.h"

namespace ukn {
    
    SequencialAnimation::SequencialAnimation():
    mCurrentStatus(AS_Stopped),
    mCurrentGridIndex(0),
    mTotalCount(0),
    mFrameRate(25),
    mRepeatCount(1),
    mFrameDelta(0.f),
    mCurrentDelta(0.f),
    mCurrentRepeatCount(0) {
        mGrids.append(GridInfo());
    }
    
    SequencialAnimation::~SequencialAnimation() {
        
    }
    
    bool SequencialAnimation::serialize(const ConfigParserPtr& config) {
        
        return false;
    }
    
    bool SequencialAnimation::deserialize(const ConfigParserPtr& config) {
        
        return false;
    }
    
    uint32 SequencialAnimation::getCount() const {
        return mTotalCount;
    }
    
    const SequencialAnimation::GridInfo& SequencialAnimation::getRootInfo() const {
        return mGrids[0];
    }
    
    const SequencialAnimation::GridList& SequencialAnimation::getGridList() const {
        return mGrids;
    }
    
    void SequencialAnimation::play() {
        if(mTotalCount > 0 && mGrids[0].type != SA_Unknown)
            setStatus(AS_Playing);
    }
    
    void SequencialAnimation::pause() {
        setStatus(AS_Paused);
    }
    
    void SequencialAnimation::stop() {
        setStatus(AS_Stopped);
    }
    
    void SequencialAnimation::setStatus(AnimationStatus status) {
        switch(status) {
            case AS_Playing:
                if(mCurrentStatus != AS_Paused) {
                    mCurrentGridIndex = 0;
                    mCurrentDelta = 0.f;
                    
                    mCurrentRepeatCount = 0;
                }
                
                if(mFrameRate != 0)
                    mFrameDelta = 1.f / mFrameRate;
                else 
                    mFrameRate = 0.f;
                
                mCurrentStatus = AS_Playing;
                mCurrentGridInfo = mGrids[0];
                
                break;
                
            case AS_Paused:
                mCurrentStatus = AS_Paused;
                break;
                
            case AS_Stopped:
                mCurrentStatus = AS_Stopped;
                mCompleteEvent.raise(this, _NullEventArgs);
                break;
        }
    }
    
    AnimationStatus SequencialAnimation::getStatus() const {
        return mCurrentStatus;
    }
    
    SequencialAnimation::CompleteEvent& SequencialAnimation::onComplete() {
        return mCompleteEvent;
    }
    
    void SequencialAnimation::update() {
        if(mCurrentStatus == AS_Playing) {
            mCurrentDelta += FrameCounter::Instance().getPrevDelta();
            if(mCurrentDelta >= mFrameDelta) {
                ++mCurrentGridIndex;
                if(mCurrentGridIndex >= mTotalCount) {
                    mCurrentGridIndex = 0;
                    
                    if(mRepeatCount != -1) {
                        mCurrentRepeatCount++;
                        if(mCurrentRepeatCount >= mRepeatCount) {
                            setStatus(AS_Stopped);
                        }
                    }
                }
            }
            
            switch(mGrids[0].type) {
                case SA_Grid: {
                    GridInfo& root = mGrids[0];
                    int32 wcount = root.texture->getWidth() / root.grid_width;
                    
                    mCurrentGridInfo.texture_pos_x = (mCurrentGridIndex % wcount) * root.grid_width;
                    mCurrentGridInfo.texture_pos_y = (mCurrentGridIndex / wcount) * root.grid_height;
                    
                    break;
                }
                case SA_GridCustom:
                case SA_SeperateFiles:
                    mCurrentGridInfo = mGrids[mCurrentGridIndex];
                    break;
                    
                case SA_Unknown:
                    break;
            }
        }
    }
    
    int32 SequencialAnimation::getFrameRate() const {
        return mFrameRate;
    }
    
    void SequencialAnimation::setFrameRate(int32 fr) {
        mFrameRate = fr;
    }
    
    const SequencialAnimation::GridInfo& SequencialAnimation::getCurrentGridInfo() const {
        return mCurrentGridInfo;
    }
    
} // namespace ukn
