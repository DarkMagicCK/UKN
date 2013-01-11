//
//  Profile.h
//  Mist
//
//  Created by Robert Bu on 12/13/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef MISTProfile_h
#define MISTProfile_h

#include "mist/Platform.h"
#include "mist/TimeUtil.h"
#include "mist/Preprocessor.h"

#include <map>

#ifdef MIST_OS_WINDOWS
#include <unordered_map>
#endif

namespace mist {
    
    namespace detail {
        
        struct MIST_API Profile {
            Profile(const MistString& name);
            ~Profile();
            
            Timestamp ts;
            MistString name;
        };
    }
    
    struct MIST_API  ProfileData {
        uint64 time;
        
        uint64 average_time;
        uint64 max_time;
        uint64 min_time;
        
        uint64 recorded_frames;
        
        float time_ratio;
        float time_ratio_frame;
        
        MistString name;
        
        MistString toFormattedString() const;
        
        ProfileData():
        time(0),
        average_time(0),
        max_time(0),
        min_time(0),
        recorded_frames(1),
        time_ratio(0),
        time_ratio_frame(0),
        name(MistString()) {
            
        }
    };
        
    class MIST_API Profiler {
    protected:
        Profiler() { }
        ~Profiler() { }
        
    public:
        static Profiler& Instance();
        
        ProfileData get(const MistString& name) const;
        
    private:
        friend struct detail::Profile;
        friend class FrameCounter;
        
        void record(const MistString& name, uint64 time);
        void updateTimeRatio(uint64 frametime, uint64 frameDelta);
        
        typedef ukn_hash_map<MistString, ProfileData> ProfileDataMap;
        ProfileDataMap mProfiles;
    };
    
#define MIST_PROFILE(name) mist::detail::Profile MIST_UNIQUE_NAME(ukn_profile_)(name);
    
} // namespace mist


#endif
