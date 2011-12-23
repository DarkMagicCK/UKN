//
//  Animation.h
//  Project Unknown
//
//  Created by Ruiwei Bu on 12/22/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_Animation_h
#define Project_Unknown_Animation_h

#include "UKN/Platform.h"
#include "UKN/MathUtil.h"
#include "UKN/PreDeclare.h"
#include "UKN/Color.h"
#include "UKN/Event.h"
#include "UKN/AutoUpdate.h"

#include <vector>

namespace ukn {
    
    class Animation {
    public:
        virtual ~Animation();
        
        virtual uint32 getDuration() const {
            return mDuration;
        }
        
        virtual void setDuration(uint32 dur) {
            mDuration = dur;
        }
        
        virtual void update(uint32 time, void* proprty_ptr) = 0;
        
    protected:
        Animation();
        
        uint32 mDuration;
    };
    
    template<typename T>
    class LinearAnimation: public Animation {
    public:
        virtual T getFrom() const {
            return mFrom;
        }
        
        virtual T getTo() const {
            return mTo;
        }
        
        virtual void setFrom(T from) {
            mFrom = from;
        }
        
        virtual void setTo(T to) {
            mTo = to;
        }
        
    protected:
        T mFrom;
        T mTo;
    };
    
    class IntLinearAnimation: public LinearAnimation<int32> {
    public:
        virtual void update(uint32 past_time, void* property_ptr);
    };
    
    class UIntLinearAnimation: public LinearAnimation<uint32> {
    public:
        virtual void update(uint32 past_time, void* property_ptr);
    };
    
    class DoubleLinearAnimation: public LinearAnimation<double> {
    public:
        virtual void update(uint32 past_time, void* property_ptr);
    };
    
    class FloatLinearAnimation: public LinearAnimation<float> {
    public:
        virtual void update(uint32 past_time, void* property_ptr);
    };
    
    class ColorLinearAnimation: public LinearAnimation<Color> {
    public:
        virtual void update(uint32 past_time, void* property_ptr);
    };
    
    enum KeyFrameType {
        // linear
        KFT_Linear,
        
        // discrete
        KFT_Discrete,
    };
    
    template<typename T>
    class KeyFrameAnimation: public Animation {
    public:
        struct KeyFrame {
            T               value;
            uint32          duration;
            uint32          time_stamp;
            KeyFrameType    frame_type;
            
            KeyFrame() {
                duration = 0;
                time_stamp = 0;
                frame_type = KFT_Discrete;
            }
            
            KeyFrame(T val, uint32 dur, uint32 ts, KeyFrameType type):
            value(val),
            duration(dur),
            time_stamp(ts),
            frame_type(type) {
                
            }
        };
        
    private:
        typedef std::vector<KeyFrame> KeyFrameList;
        
    public:
        uint32 getDuration() const {
            uint32 dur = 0;
            
            typename KeyFrameList::const_iterator it = mKeyFrames.begin();
            for(; it != mKeyFrames.end(); ++it) {
                dur += it->duration;
            }
            return dur;
        }
        
        KeyFrameList& getKeyFrames() {
            return mKeyFrames;
        }
        
        const KeyFrameList& getKeyFrames() const {
            return mKeyFrames;
        }
        
        size_t getTimePos(uint32 time) {
            size_t i = 0;
            uint32 ttime = 0;
            while(i < mKeyFrames.size() && 
                  ttime + mKeyFrames[i].duration <= time) {
                ++i;
                ttime += mKeyFrames[i].duration;
            }
            return i;
        }
        
    protected:
        KeyFrameList mKeyFrames;
    };
    
    class IntKeyFrameAnimation: public KeyFrameAnimation<int32> {
    public:
        virtual void update(uint32 past_time, void* property_ptr);
    };
    
    class UIntKeyFrameAnimation: public KeyFrameAnimation<uint32> {
    public:
        virtual void update(uint32 past_time, void* property_ptr);
    };
    
    class DoubleKeyFrameAnimation: public KeyFrameAnimation<double> {
    public:
        virtual void update(uint32 past_time, void* property_ptr);
    };
    
    class FloatKeyFrameAnimation: public KeyFrameAnimation<float> {
    public:
        virtual void update(uint32 past_time, void* property_ptr);
    };
    
    class ColorKeyFrameAnimation: public KeyFrameAnimation<Color> {
    public:
        virtual void update(uint32 past_time, void* property_ptr);
    };
    
    
    enum AnimationStatus {
        AS_Stop,
        AS_Playing,
        AS_Paused,
    };
    
    class StoryBoard {
    public:
        struct StoryBoardCompletedEventArgs {
            uint32 pass_time;
        };
        
        
        struct AnimationInfo {
            uint32 start_time;
            Animation* animation_ptr;
            void* property_ptr;
            
            AnimationInfo(uint32 start, Animation* anime, void* property):
            start_time(start),
            animation_ptr(anime),
            property_ptr(property) {
                
            }
        };
        
    public:
        StoryBoard();
        ~StoryBoard();
        
        typedef std::vector<AnimationInfo> AnimationSet;
        
        AnimationSet& children();
        const AnimationSet& children() const;
        
        void setStatus(AnimationStatus states);
        AnimationStatus getStatus() const;
        
        void play();
        void pause();
        void stop();
        
        void setDuration(uint32 ms);
        uint32 getDuration();
        
        void update(uint64 pass_time);
        
        Event<StoryBoardCompletedEventArgs>& onComplete();
        
    private:
        Event<StoryBoardCompletedEventArgs> mCompleteEvent;
        
        AnimationSet mAnimationSet;
        AnimationStatus mStatus;
        
        uint64 mStoryBoardStartTime, mStoryBoardPauseTime;
        uint32 mDuration;
        uint64 mLastUpdateTime;
        
        friend class StoryBoardManager;
    };
    
    class StoryBoardManager: public AutoUpdate {
    protected:
        StoryBoardManager();
        ~StoryBoardManager();
        
    public:
        static StoryBoardManager& Instance();
        
        void addStoryBoard(StoryBoard* sb);
        void delStoryBoard(StoryBoard* sb);
        
        void onUpdate();
        
        typedef std::list<StoryBoard*> StoryBoardList;
        
        StoryBoardList& getStoryBoardList();
        const StoryBoardList& getStoryBoardList() const;
        
    private:
        StoryBoardList mStoryBoards;
    };
       
} // namespace ukn

#endif
