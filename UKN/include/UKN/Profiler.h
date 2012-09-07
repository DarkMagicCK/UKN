//
//  Profile.h
//  Project Unknown
//
//  Created by Robert Bu on 12/13/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_Profile_h
#define Project_Unknown_Profile_h

#include "UKN/Platform.h"
#include "UKN/TimeUtil.h"
#include "UKN/Preprocessor.h"

#include <map>

#ifdef UKN_OS_WINDOWS
#include <unordered_map>
#endif

namespace ukn {
    
    namespace detail {
        
        struct Profile {
            Profile(const UknString& name);
            ~Profile();
            
            Timestamp ts;
            UknString name;
        };
    }
    
    struct ProfileData {
        uint64 time;
        
        uint64 average_time;
        uint64 max_time;
        uint64 min_time;
        
        uint64 recorded_frames;
        
        float time_ratio;
        float time_ratio_frame;
        
        UknString name;
        
        UknString toFormattedString() const;
        
        ProfileData():
        time(0),
        average_time(0),
        max_time(0),
        min_time(0),
        recorded_frames(1),
        time_ratio(0),
        time_ratio_frame(0),
        name(UknString()) {
            
        }
    };
        
    class Profiler {
    protected:
        Profiler() { }
        ~Profiler() { }
        
    public:
        static Profiler& Instance();
        
        ProfileData get(const UknString& name) const;
        
    private:
        friend struct detail::Profile;
        friend class FrameCounter;
        
        void record(const UknString& name, uint64 time);
        void updateTimeRatio(uint64 frametime, uint64 frameDelta);
        
        typedef ukn_hash_map<UknString, ProfileData> ProfileDataMap;
        ProfileDataMap mProfiles;
    };
    
#define UKN_PROFILE(name) ukn::detail::Profile UKN_UNIQUE_NAME(ukn_profile_)(name);
    
} // namespace ukn


#endif
