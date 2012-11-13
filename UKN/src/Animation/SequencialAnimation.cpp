//
//  SequencialAnimation.cpp
//  Project Unknown
//
//  Created by Ruiwei Bu on 1/17/12.
//  Copyright (c) 2012 heizi. All rights reserved.
//

#include "UKN/SequencialAnimation.h"
#include "UKN/Texture.h"
#include "UKN/Asset.h"

#include "mist/ConfigParser.h"
#include "mist/TimeUtil.h"

namespace ukn {
    
    SequencialAnimation::SequencialAnimation():
    mCurrentStatus(AS_Stopped),
    mCurrentGridIndex(0),
    mTotalCount(0),
    mFrameRate(25),
    mRepeatCount(-1),
    mFrameDelta(0.f),
    mCurrentDelta(0.f),
    mCurrentRepeatCount(0) {
        mGrids.push_back(GridInfo());
    }
    
    SequencialAnimation::SequencialAnimation(const TexturePtr& texture, int32 grid_width, int32 grid_height, uint32 count, uint32 framerate):
    mCurrentStatus(AS_Stopped),
    mCurrentGridIndex(0),
    mTotalCount(count),
    mFrameRate(framerate),
    mRepeatCount(-1),
    mCurrentDelta(0.f),
    mCurrentRepeatCount(0) {
        GridInfo info;
        info.texture = texture;
        info.grid_width = grid_width;
        info.grid_height = grid_height;
        info.count = count;
        info.texture_pos_x = info.texture_pos_y = 0;
        info.mode = SA_Grid;
        mGrids.push_back(info);
        
        play();
    }
    
    SequencialAnimation::~SequencialAnimation() {
        
    }
    
    bool SequencialAnimation::serialize(const ConfigParserPtr& config) {
        
        return false;
    }
    
    inline SequencialAnimationMode name_to_mode(const UknString& name) {
        if(name == L"grid")
            return SA_Grid;
        if(name == L"grid_custom")
            return SA_GridCustom;
        if(name == L"seperate_files")
            return SA_SeperateFiles;
        return SA_Unknown;
    }
    
    bool SequencialAnimation::deserialize(const ConfigParserPtr& config) {
        if(config->toNode(L"SpriteSheet")) {
            UknString name = config->getString(L"name");
            if(!name.empty()) {
                mName = name;
                
                mGrids[0].mode = name_to_mode(config->getString(L"mode").c_str());
                if(mGrids[0].mode == SA_Unknown)
                    return false;
                
                switch(mGrids[0].mode) {
                    case SA_Grid:
                        mGrids[0].grid_width = config->getInt(L"width");
                        mGrids[0].grid_height = config->getInt(L"height");
                        mGrids[0].count = config->getInt(L"count");
                        mGrids[0].texture = AssetManager::Instance().load<ukn::Texture>(String::GetFilePath(config->getName()) + (config->getString(L"texture")));
                        
                        mFrameRate = config->getInt(L"frame_rate", DefaultFrameRate);
                        break;
                        
                    case SA_GridCustom:
                        mGrids[0].texture = AssetManager::Instance().load<ukn::Texture>(config->getString(L"texture"));
                        mFrameRate = config->getInt(L"frame_rate", DefaultFrameRate);
                        
                        if(config->toFirstChild()) {
                            do {
                                int childcount = config->getInt(L"count", 0);
                                if(childcount != 0) {
                                    for(int i = 0; i < childcount; ++i) {
                                        GridInfo info;
                                        info.grid_width = config->getInt(L"width");
                                        info.grid_height = config->getInt(L"height");
                                        info.texture_pos_x = config->getInt(L"x") + info.grid_width * i;
                                        info.texture_pos_y = config->getInt(L"y");
                                        
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
                                UknString texture_path = config->getString(L"texture");
                                if(!texture_path.empty()) {
                                    GridInfo info;
                                    info.texture = AssetManager::Instance().load<ukn::Texture>(String::GetFilePath(config->getName()) + texture_path);
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
                return true;
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
                    mFrameDelta = 0.f;
                
                mCurrentStatus = AS_Playing;
                
                if(mGrids[0].mode == SA_Grid)
                    mCurrentGridInfo = mGrids[0];
                else
                    mCurrentGridInfo = mGrids[1];
                
                break;
                
            case AS_Paused:
                mCurrentStatus = AS_Paused;
                break;
                
            case AS_Stopped:
                mCurrentStatus = AS_Stopped;
                mCompleteEvent.raise(this, _NullEventArgs);
                break;
                
            default:
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
            if(mTotalCount != 1) {
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
