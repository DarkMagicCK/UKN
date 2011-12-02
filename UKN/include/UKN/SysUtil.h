//
//  SysUtil.h
//  Project Unknown
//
//  Created by Ruiwei Bu on 12/1/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_SysUtil_h
#define Project_Unknown_SysUtil_h

#include "Platform.h"

namespace ukn {
    
    enum MessageBoxOption {
        MBO_OK          = 1UL,
        MBO_OKCancel    = 1UL << 1,
        MBO_IconError   = 1UL << 2,
        MBO_IconWarning = 1UL << 3,
        MBO_IconInfo    = 1UL << 4,
    };
    
    enum MessageBoxButton {
        MBB_OK          = 1,
        MBB_Cancel      = 2,
    };
    
    UKN_API MessageBoxButton message_box(const ukn_string& mssg, const ukn_string& title, MessageBoxOption option);
    UKN_API MessageBoxButton message_box(const ukn_wstring& mssg, const ukn_wstring& title, MessageBoxOption option);

    // in mhz
    UKN_API uint64 get_system_processor_speed();
    // in bytes
    UKN_API uint64 get_system_memory_size();
    // formatted string
    UKN_API ukn_string get_os_version();
    
    
    class DirectoryIteratorImpl;
        
    class UKN_API DirectoryIterator {
    public:
        DirectoryIterator();
        DirectoryIterator(const DirectoryIterator& iterator);
        explicit DirectoryIterator(const ukn_string& path);
        explicit DirectoryIterator(const ukn_wstring& path);
        ~DirectoryIterator();
        
        const ukn_string& file() const;
        const ukn_string& path() const;
        
        bool isEnd() const;
        
        DirectoryIterator& operator=(const DirectoryIterator& it);
        
        DirectoryIterator& operator++();
        
        bool operator == (const DirectoryIterator& iterator) const;
        bool operator != (const DirectoryIterator& iterator) const;
        
    private:
        ukn_string mPath;
        ukn_string mFile;
        bool mIsEnd;
        DirectoryIteratorImpl* mImpl;
    };
    
    inline const ukn_string& DirectoryIterator::path() const {
        return mPath;
    }
    
    inline bool DirectoryIterator::isEnd() const {
        return mIsEnd;
    }
    
    inline const ukn_string& DirectoryIterator::file() const {
        return mFile;
    }
    
    inline bool DirectoryIterator::operator ==(const DirectoryIterator& rhs) const {
        return file() == rhs.file();
    }
    
    inline bool DirectoryIterator::operator !=(const DirectoryIterator& rhs) const {
        return file() != rhs.file();
    }
    
    /**
     * Retrives cpuinfo from current platform
     **/
    
    class UKN_API CpuInfo {
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

}


#endif