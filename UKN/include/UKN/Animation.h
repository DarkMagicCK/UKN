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
#include "UKN/Serializer.h"
#include "UKN/Class.h"

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
            KeyFrameType    frame_type;
            
            KeyFrame() {
                duration = 0;
                frame_type = KFT_Discrete;
            }
            
            KeyFrame(T val, uint32 dur, KeyFrameType type):
            value(val),
            duration(dur),
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
        
        size_t getTimePos(uint32 time, uint32& prev_time) {
            size_t i = 0;
            prev_time = 0;
            while(i < mKeyFrames.size() && 
                  prev_time + mKeyFrames[i].duration <= time) {
                prev_time += mKeyFrames[i].duration;
                ++i;
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
    
    
    struct StoryBoardCompletedEventArgs {
        uint32 pass_time;
    };
    
    class StoryBoard {
    public:
        struct AnimationInfo {
            uint32 start_time;
            SharedPtr<Animation> animation_ptr;
            void* property_ptr;
            
            AnimationInfo(uint32 start, const SharedPtr<Animation>& anime, void* property):
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
    
    typedef SharedPtr<StoryBoard> StoryBoardPtr;
    
    class AnimationPlayer: public ConfigSerializable {
    public:
        AnimationPlayer();
        ~AnimationPlayer();
        
        virtual bool serialize(const ConfigParserPtr& parser);
        virtual bool deserialize(const ConfigParserPtr& parser);
        
        bool isStoryBoardExist(const ukn_string& name) const;
        
        StoryBoardPtr getStoryBoard(const ukn_string& name) const;
        
        void play(const ukn_string& name);
        void pause();
        void stop();
        
        // property references
        // required for deserialization
        void  addProperty(const ukn_string& name, void* prop);
        void  delProperty(const ukn_string& name);
        void* getProperty(const ukn_string& name) const;
        
        size_t size() const;
        
        Event<StoryBoardCompletedEventArgs>& onComplete();
        
        void       setDefault(const ukn_string& name);
        ukn_string getDefault() const;
        ukn_string getCurrent() const;
                
    private:
        typedef std::map<ukn_string, StoryBoardPtr> StoryBoardMap;
        StoryBoardMap mStoryBoards;
        
        Event<StoryBoardCompletedEventArgs> mCompleteEvent;
        
        typedef std::map<ukn_string, void*> PropertyMap;
        PropertyMap mProperties;
        
        ukn_string mDefault;
        ukn_string mCurrent;
        
        void onAnimationFinished(void* sender, StoryBoardCompletedEventArgs* args);
    };
       
} // namespace ukn

#endif
