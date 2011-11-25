//
//  CpuInfo.h
//  Project Unknown
//
//  Created by Ruiwei Bu on 11/24/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_CpuInfo_h
#define Project_Unknown_CpuInfo_h

#include "Platform.h"

namespace ukn {
    
    /**
     * Retrives cpuinfo from current platform
     **/
    
    class CpuInfo {
    public:
        enum Feature {
            HTT             = 1UL << 0,
            MMX             = 1UL << 1,
            SSE             = 1UL << 2,
            SSE2            = 1UL << 3,
            SSE3            = 1UL << 4,
            SSSE3           = 1UL << 5,
            SSE41           = 1UL << 6,
            SSE42           = 1UL << 7,
            SSE4A           = 1UL << 8,
            MisalignedSSE   = 1UL << 9,
            X64             = 1UL << 10,
            FMA             = 1UL << 11,
            MOVBE           = 1UL << 12,
            POPCNT          = 1UL << 13,
            AES             = 1UL << 14,
            AVX             = 1UL << 15,
            LZCNT           = 1UL << 16
        };
        
    public:
        CpuInfo();
        
        const std::string& getCPUString() const {
            return mCPUString;
        }
        
        const std::string& getCPUBrandString() const {
            return mCPUBrandString;
        }
        
        bool testFeature(Feature feature) const {
            return mFeatureMask & feature ? true : false;
        }
        
        int getNumHWThreads() const {
            return mNumHWThreads;
        }
        
        int getNumCores() const {
            return mNumCores;
        }
        
    private:
        std::string mCPUString;
        std::string mCPUBrandString;
        uint64 mFeatureMask;
        
        int mNumCores;
        int mNumHWThreads;
    };
    
} // namespace ukn


#endif
