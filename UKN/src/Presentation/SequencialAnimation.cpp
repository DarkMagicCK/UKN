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
#include "UKN/ConfigParser.h"
#include "UKN/Asset.h"

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
    
    inline SequencialAnimationMode name_to_mode(const char* name) {
        if(strcmpnocase(name, "grid") == 0)
            return SA_Grid;
        if(strcmpnocase(name, "grid_custom") == 0)
            return SA_GridCustom;
        if(strcmpnocase(name, "seperate_files") == 0)
            return SA_SeperateFiles;
        return SA_Unknown;
    }
    
    bool SequencialAnimation::deserialize(const ConfigParserPtr& config) {
        if(config->toNode("SpriteSheet")) {
            ukn_string name = config->getString("name");
            if(!name.empty()) {
                mName = name;
                
                mGrids[0].mode = name_to_mode(config->getString("mode").c_str());
                if(mGrids[0].mode == SA_Unknown)
                    return false;
                
                switch(mGrids[0].mode) {
                    case SA_Grid:
                        mGrids[0].grid_width = config->getInt("width");
                        mGrids[0].grid_height = config->getInt("height");
                        mGrids[0].count = config->getInt("count");
                        mGrids[0].texture = AssetManager::Instance().load<ukn::Texture>(get_file_path(config->getName()) + string_to_wstring(config->getString("texture")));
                        
                        mFrameRate = config->getInt("frame_rate", 25);
                        break;
                        
                    case SA_GridCustom:
                        mGrids[0].texture = AssetManager::Instance().load<ukn::Texture>(config->getString("texture"));
                        mFrameRate = config->getInt("frame_rate", 25);
                        
                        if(config->toFirstChild()) {
                            do {
                                int childcount = config->getInt("count", 0);
                                if(childcount != 0) {
                                    for(int i=0; i<childcount; ++i) {
                                        GridInfo info;
                                        info.grid_width = config->getInt("width");
                                        info.grid_height = config->getInt("height");
                                        info.texture_pos_x = config->getInt("x") + info.grid_width * i;
                                        info.texture_pos_y = config->getInt("y");
                                        
                                        info.texture = mGrids[0].texture;
                                        
                                        mGrids.push_back(info);
                                    }
                                }
                                
                            } while(config->toNextChild());
                            
                            // to SpriteSheet
                            config->toParent();
                        }
                        mTotalCount = (uint32)mGrids.size();
                        
                        break;
                        
                    case SA_SeperateFiles:
                        if(config->toFirstChild()) {
                            do {
                                ukn_string texture_path = config->getString("texture");
                                if(!texture_path.empty()) {
                                    GridInfo info;
                                    info.texture = AssetManager::Instance().load<ukn::Texture>(get_file_path(config->getName()) + string_to_wstring(texture_path));
                                    info.texture_pos_x = 0;
                                    info.texture_pos_y = 0;
                                    if(info.texture) {
                                        info.grid_width = info.texture->getWidth();
                                        info.grid_height = info.texture->getHeight();
                                    }
                                }
                                
                            } while(config->toNextChild());
                            
                            // to SpriteSheet
                            config->toParent();
                        }
                        mTotalCount = (uint32)mGrids.size();
                        break;
                        
                    case SA_Unknown:
                        break;
                }
                
                config->toParent();
            } else 
                return false;
        }
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
        if(mTotalCount > 0 && mGrids[0].mode != SA_Unknown)
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
            
            switch(mGrids[0].mode) {
                case SA_Grid: {
                    GridInfo& root = mGrids[0];
                    int32 wcount = root.texture->getWidth() / root.grid_width;
                    
                    mCurrentGridInfo.texture_pos_x = (int32)(mCurrentGridIndex % wcount) * root.grid_width;
                    mCurrentGridInfo.texture_pos_y = (int32)(mCurrentGridIndex / wcount) * root.grid_height;
                    
                    break;
                }
                    
                case SA_GridCustom:
                case SA_SeperateFiles:
                    // root info takes mGrids[0]
                    mCurrentGridInfo = mGrids[mCurrentGridIndex+1];
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
