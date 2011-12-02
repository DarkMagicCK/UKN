//
//  FileUtil.cpp
//  Project Unknown
//
//  Created by Ruiwei Bu on 11/30/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#include "UKN/Platform.h"
#include "UKN/FileUtil.h"

#ifdef UKN_OS_WINDOWS
#include <Windows.h>
#include "Shlwapi.h"
#pragma comment(lib,"shlwapi.lib")

#elif defined(UKN_OS_FAMILY_APPLE)
#include "AppleStuff.h"
#include <sys/stat.h>
#else
#include <sys/stat.h>
#endif

#include <cstdio>
#include <fstream>

#include "UKN/StringUtil.h"

namespace ukn {
    
    UKN_API bool file_exists(const ukn_wstring& filepath) {
#ifdef UKN_OS_WINDOWS
        return PathFileExistsW(filepath.c_str())?true:false;
        
#elif defined(UKN_OS_FAMILY_APPLE)
        return ukn_apple_file_exists(filepath);
#endif
        
        struct stat sb;
        
        if (stat(wstring_to_string(filepath).c_str(), &sb) == 0 && S_ISREG(sb.st_mode)) {
            return true;
        }
        
        return true;
    }
    
    UKN_API bool path_exists(const ukn_wstring& path) {
#ifdef UKN_OS_WINDOWS
        return PathFileExistsW(filepath.c_str())?true:false;

#else
        struct stat sb;
        
        if (stat(wstring_to_string(path).c_str(), &sb) == 0 && S_ISDIR(sb.st_mode)) {
            return true;
        }
#endif
        
        return false;
    }
    
    UKN_API ukn_wstring get_writtable_path(const ukn_wstring& filePath) {
#ifndef UKN_OS_FAMILY_APPLE
        return ukn_get_application_path() + filePath;
#else
        return ukn_apple_documents_path() + filePath;
#endif
    }
    
    UKN_API ukn_wstring get_application_path() {
#ifdef UKN_OS_WINDOWS
        wchar_t buffer[MAX_PATH];
        GetCurrentDirectoryW(MAX_PATH, buffer);
        
        return filePath+L"/";
        
#elif defined(UKN_OS_FAMILY_APPLE)
        return ukn_apple_application_path() + L"/";
        
#endif
        return L"./";
    }
    
    
} // namespace ukn


#ifndef UKN_OS_WINDOWS

#include <dirent.h>

namespace ukn {
    
    class DirectoryIterator::DirectoryIteratorImpl {
    public:
        DirectoryIteratorImpl(const ukn_string& path) {
            mDir = opendir((path+"/").c_str());
            
            if(mDir)
                next();
        }
        
        ~DirectoryIteratorImpl() {
            if(mDir)
                closedir(mDir);
        }
        
        void duplicate();
        void release();
        
        const ukn_string& get() const;
        const ukn_string& next() {
            do {
                struct dirent* entry = readdir(mDir);
                if(entry)
                    mCurrent = entry->d_name;
                else
                    mCurrent.clear();
            }
            while(mCurrent == "." || mCurrent == ".." || (mCurrent.size() > 0 && mCurrent[0] == '.'));
            
            return mCurrent;
        }
        
    private:
        DIR* mDir;
        ukn_string mCurrent;
        int mRC;
    };
    
    const ukn_string& DirectoryIterator::DirectoryIteratorImpl::get() const {
        return mCurrent;
    }
    
    inline void DirectoryIterator::DirectoryIteratorImpl::duplicate() {
        ++mRC;
    }
    
    inline void DirectoryIterator::DirectoryIteratorImpl::release() {
        if (--mRC) {
            delete this;
        }
    }    
} 

#else

#include <Windows.h>

namespace ukn {
    
    class DirectoryIterator::DirectoryIteratorImpl {
    public:
        DirectoryIteratorImpl(const ukn_string& path) {
            std::string findPath = path + "/*";
            
            mFH = FindFirstFileA(findPath.c_str(), &mFD);
            if(mFH == INVALID_HANDLE_VALUE) {
				DWORD error = GetLastError();
                if(error != ERROR_NO_MORE_FILES) {
                    
                }
            } else {
                mCurrent = mFD.cFileName;
                if(mCurrent == "." || mCurrent == "..")
                    next();
            }
        }
        
        ~DirectoryIteratorImpl() {
            if(mFH != INVALID_HANDLE_VALUE)
                FindClose(mFH);
        }
        
        void duplicate();
        void release();
        
        const ukn_string& get() const;
        const ukn_string& next() {
            do {
                if(FindNextFileA(mFH, &mFD) != 0) {
                    mCurrent = mFD.cFileName;
                } else 
                    mCurrent.clear();
            }
            while(mCurrent == "." || mCurrent == "..");
            
            return mCurrent;
        }
        
    private:
        HANDLE mFH;
        WIN32_FIND_DATAA mFD;
        ukn_string mCurrent;
        int mRC;
    };
    
    const std::string& DirectoryIterator::DirectoryIteratorImpl::get() const {
        return mCurrent;
    }
    
    inline void DirectoryIterator::DirectoryIteratorImpl::duplicate() {
        ++mRC;
    }
    
    inline void DirectoryIterator::DirectoryIteratorImpl::release() {
        if (--mRC) {
            delete this;
        }
    }
    
} // namespace ukn

#endif // UKN_OS_WINDOWS

namespace ukn {
    
    DirectoryIterator::DirectoryIterator():
    mImpl(0) {
        mIsEnd = false;
    }
    
    DirectoryIterator::DirectoryIterator(const DirectoryIterator& iterator):
    mPath(iterator.mPath),
    mFile(iterator.mFile),
    mImpl(iterator.mImpl) {
        mIsEnd = false;
        if(mImpl) {
            mImpl->duplicate();
        }
    }
    
    DirectoryIterator::DirectoryIterator(const ukn_string& path):
    mPath(path+"/"),
    mImpl(new DirectoryIteratorImpl(path)) {
        mIsEnd = false;
        
        mFile = mPath + mImpl->get();
    }
    
    DirectoryIterator::DirectoryIterator(const ukn_wstring& path):
    mPath(wstring_to_string(path+L"/")) {
        mIsEnd = false;
        mImpl = new DirectoryIteratorImpl(mPath.c_str());
        
        mFile = mPath + mImpl->get();
    }
    
    DirectoryIterator::~DirectoryIterator() {
        if(mImpl)
            mImpl->release();
    }
    
    DirectoryIterator& DirectoryIterator::operator =(const DirectoryIterator& rhs) {
        if(this != &rhs) {
            if(mImpl)
                mImpl->duplicate();
            mImpl = rhs.mImpl;
            if(mImpl) {
                mImpl->duplicate();
                mPath = rhs.mPath;
                mIsEnd = rhs.mIsEnd;
                mFile = rhs.mFile;
            }
            return *this;
        }
        return *this;
    }
    
    
    DirectoryIterator& DirectoryIterator ::operator++() {
        if(mImpl) {
            std::string n = mImpl->next();
            if(n.empty())
                mIsEnd = true;
            else {
                mFile = mPath + mImpl->get();
            }
        }
        return *this;
    }
    
}