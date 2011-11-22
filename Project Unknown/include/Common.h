//
//  Common.h
//  Project Unknown
//
//  Created by Robert Bu on 11/21/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_Common_h
#define Project_Unknown_Common_h

#include "Platform.h"

#ifdef UKN_OS_WINDOWS

#include <windows.h>

#else

#include <time.h>
#include <errno.h>

#endif // UKN_OS_WINDOWS

namespace ukn {
    
#ifdef UKN_OS_WINDOWS
    
    inline void msleep(uint32 msec) {
        Sleep(msec);
    }
    
#else
    
    // sleep for milliseconds
    // see my http://cc.byexamples.com/2007/05/25/nanosleep-is-better-than-sleep-and-usleep/ for more information
    inline void msleep(uint32_t msec) {
        {
            struct timespec timeout0;
            struct timespec timeout1;
            struct timespec* tmp;
            struct timespec* t0 = &timeout0;
            struct timespec* t1 = &timeout1;
            
            t0->tv_sec = msec / 1000;
            t0->tv_nsec = (msec % 1000) * (1000 * 1000);	
            while ((nanosleep(t0, t1) == (-1)) && (errno == EINTR)) {
                tmp = t0;
                t0 = t1;
                t1 = tmp;
            }
        }
    }
    
#endif // UKN_OS_WINDOWS
    
}


#endif
