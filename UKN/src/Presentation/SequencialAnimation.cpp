//
//  SequencialAnimation.cpp
//  Project Unknown
//
//  Created by Ruiwei Bu on 1/17/12.
//  Copyright (c) 2012 heizi. All rights reserved.
//

#include "UKN/SequencialAnimation.h"

namespace ukn {
    
    SequencialAnimation::SequencialAnimation():
    mCurrentStatus(AS_Stopped),
    mCurrentGridIndex(0) {
        mGrids.append(GridInfo());
    }
    
    SequencialAnimation::~SequencialAnimation() {
        
    }
    
    bool SequencialAnimation::serialize(const ConfigParserPtr& config) {
        
    }
    
    bool SequencialAnimation::deserialize(const ConfigParserPtr& config) {
        
        
    }
    
    uint32 SequencialAnimation::getCount() const {
        return mRootInfo.count;
    }
    
    const SequencialAnimation::GridInfo& SequencialAnimation::getRootInfo() const {
        return mGrids[0];
    }
    
    const SequencialAnimation::GridList& SequencialAnimation::getGridList() const {
        return mGrids;
    }
    
    void SequencialAnimation::play() {
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
                
                
        }
    }
    
    AnimationStatus SequencialAnimation::getStatus() const {
        return mCurrentStatus;
    }
    
    SequencialAnimation::CompleteEvent& SequencialAnimation::onComplete() {
        return mCompleteEvent;
    }
    
    
    
} // namespace ukn
