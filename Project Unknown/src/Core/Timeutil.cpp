//
//  Timeutil.cpp
//  Project Unknown
//
//  Created by Ruiwei Bu on 11/24/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#include "TimeUtil.h"

#if !defined(UKN_OS_WINDOWS)
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/times.h>
#else
#include <windows.h>
#endif

namespace ukn {
    
    Timestamp::Timestamp()
    {
        update();
    }
    
    
    Timestamp::Timestamp(TimeVal tv)
    {
        _ts = tv;
    }
    
    
    Timestamp::Timestamp(const Timestamp& other)
    {
        _ts = other._ts;
    }
    
    
    Timestamp::~Timestamp()
    {
    }
    
    
    Timestamp& Timestamp::operator = (const Timestamp& other)
    {
        _ts = other._ts;
        return *this;
    }
    
    
    Timestamp& Timestamp::operator = (TimeVal tv)
    {
        _ts = tv;
        return *this;
    }
    
    
    void Timestamp::swap(Timestamp& timestamp)
    {
        std::swap(_ts, timestamp._ts);
    }
    
    
    Timestamp Timestamp::FromEpochTime(std::time_t t)
    {
        return Timestamp(TimeVal(t)*Resolution());
    }
    
    
    Timestamp Timestamp::FromUtcTime(UtcTimeVal val)
    {
        val -= (TimeDiff(0x01b21dd2) << 32) + 0x13814000;
        val /= 10;
        return Timestamp(val);
    }
    
    Timestamp::TimeVal Timestamp::CurrentTime() {
        TimeVal ts;
#if defined(_WIN32)
        
        FILETIME ft;
        GetSystemTimeAsFileTime(&ft);
        ULARGE_INTEGER epoch; // UNIX epoch (1970-01-01 00:00:00) expressed in Windows NT FILETIME
        epoch.LowPart  = 0xD53E8000;
        epoch.HighPart = 0x019DB1DE;
        
        ULARGE_INTEGER tts;
        tts.LowPart  = ft.dwLowDateTime;
        tts.HighPart = ft.dwHighDateTime;
        tts.QuadPart -= epoch.QuadPart;
        ts = tts.QuadPart/10;
        
#else
        
        struct timeval tv;
        if (gettimeofday(&tv, NULL))
            throw ("cannot get time of day");
        ts = TimeVal(tv.tv_sec)*Resolution() + tv.tv_usec;
        
#endif
        return ts;
    }
    
    
    void Timestamp::update()
    {
#if defined(_WIN32)
        
        FILETIME ft;
        GetSystemTimeAsFileTime(&ft);
        ULARGE_INTEGER epoch; // UNIX epoch (1970-01-01 00:00:00) expressed in Windows NT FILETIME
        epoch.LowPart  = 0xD53E8000;
        epoch.HighPart = 0x019DB1DE;
        
        ULARGE_INTEGER ts;
        ts.LowPart  = ft.dwLowDateTime;
        ts.HighPart = ft.dwHighDateTime;
        ts.QuadPart -= epoch.QuadPart;
        _ts = ts.QuadPart/10;
        
#else
        
        struct timeval tv;
        if (gettimeofday(&tv, NULL))
            throw ("cannot get time of day");
        _ts = TimeVal(tv.tv_sec)*Resolution() + tv.tv_usec;
        
#endif
    }
    
    
#if defined(_WIN32)
    
    
    Timestamp Timestamp::fromFileTimeNP(uint32 fileTimeLow, uint32 fileTimeHigh)
    {
        ULARGE_INTEGER epoch; // UNIX epoch (1970-01-01 00:00:00) expressed in Windows NT FILETIME
        epoch.LowPart  = 0xD53E8000;
        epoch.HighPart = 0x019DB1DE;
        
        ULARGE_INTEGER ts;
        ts.LowPart  = fileTimeLow;
        ts.HighPart = fileTimeHigh;
        ts.QuadPart -= epoch.QuadPart;
        
        return Timestamp(ts.QuadPart/10);
    }
    
    
    void Timestamp::toFileTimeNP(uint32& fileTimeLow, uint32& fileTimeHigh) const
    {
        ULARGE_INTEGER epoch; // UNIX epoch (1970-01-01 00:00:00) expressed in Windows NT FILETIME
        epoch.LowPart  = 0xD53E8000;
        epoch.HighPart = 0x019DB1DE;
        
        ULARGE_INTEGER ts;
        ts.QuadPart  = _ts*10;
        ts.QuadPart += epoch.QuadPart;
        fileTimeLow  = ts.LowPart;
        fileTimeHigh = ts.HighPart;
    }
    
    
#endif
    
} // namespace ukn
