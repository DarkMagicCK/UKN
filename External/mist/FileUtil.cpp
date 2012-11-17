//
//  FileUtil.cpp
//  Mist
//
//  Created by Robert Bu on 11/30/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#include "mist/Platform.h"
#include "mist/FileUtil.h"

#ifdef MIST_OS_WINDOWS
#include <Windows.h>
#include "Shlwapi.h"
#pragma comment(lib,"shlwapi.lib")

#elif defined(MIST_OS_FAMILY_APPLE)
#include "AppleStuff.h"
#include <sys/stat.h>
#include <unistd.h>
#else
#include <sys/stat.h>
#endif // MIST_OS_FAMILTY_APPLE

#ifdef MIST_OS_FAMILY_UNIX   
#include <pwd.h>
#endif // MIST_OS_FAMILTY_UNIX

#include <cstdio>
#include <fstream>

#include "mist/StringUtil.h"

namespace mist {
    
    bool File::FileExists(const MistString& filepath) {
#ifdef MIST_OS_WINDOWS
        return PathFileExistsW(filepath.c_str())?true:false;
        
#elif defined(MIST_OS_FAMILY_APPLE)
        return ukn_apple_file_exists(filepath);
#else
        
        struct stat sb;
        
        if (stat(MistString::WStringToString(filepath).c_str(), &sb) == 0 && S_ISREG(sb.st_mode)) {
            return true;
        }
#endif
        return false;
    }
    
    bool File::PathExists(const MistString& path) {
#ifdef MIST_OS_WINDOWS
        return PathFileExistsW(path.c_str())?true:false;

#else
        struct stat sb;
        
        if (stat(String::WStringToString(path).c_str(), &sb) == 0 && S_ISDIR(sb.st_mode)) {
            return true;
        }
#endif
        
        return false;
    }
    
    MistString Path::GetApplicationPath() {
#ifdef MIST_OS_WINDOWS
        wchar_t buffer[MAX_PATH];
        GetCurrentDirectoryW(MAX_PATH, buffer);
        
        return MistString(buffer)+L"/";
        
#elif defined(MIST_OS_FAMILY_APPLE)
        return ukn_apple_application_path() + L"/";
        
#endif
        return L"./";
    }
    
    MistString Path::CheckAndGetFontPath(const MistString& name) {
        
        if(File::FileExists(L"./"+name))
            return L"./"+name;
        
        // resource path
        {
            MistString fullpath(Path::GetResource() + name);
            
            if(File::FileExists(fullpath)) {
                return fullpath;
            }
        }
        
        // system font path
        {
            MistString fullpath(Path::GetFont() + name);
        
            if(File::FileExists(fullpath)) {
                return fullpath;
            }
        }
        
        // document path for ios and osx
        {
#if defined(MIST_OS_FAMILY_APPLE)
            MistString fullpath(ukn_apple_documents_path());
            
            if(File::FileExists(fullpath)) {
                return fullpath;
            }
#endif
        }
        
        return MistString();
    }
    
    MistString Path::GetEnv(const MistString& env) {
#if defined(MIST_OS_WINDOWS)
        DWORD len = GetEnvironmentVariableW(env.c_str(), 0, 0);
        if (len != 0) {
            char* buffer = new char[len];
            GetEnvironmentVariableW(env.c_str(), buffer, len);
            MistString result(buffer);
            delete [] buffer;
            return result;
        }
        
#elif defined(MIST_OS_FAMILY_UNIX)
        return String::StringToWStringFast(getenv(String::WStringToString(env).c_str()));
#endif
        
        return MistString();
    }
    
    MistString Path::GetCurrent() {
#if defined(MIST_OS_WINDOWS)
        wchar_t buffer[_MAX_PATH];
        DWORD n = GetCurrentDirectoryW(sizeof(buffer), buffer);
        if(n > 0 && n < sizeof(buffer)) {
            MistString result(buffer, n);
            if(result[n-1] != L'\\')
                result.append(L"\\");
            return result;
        }
        
#elif defined(MIST_OS_FAMILY_UNIX)
        std::string path;
        char cwd[PATH_MAX];
        if(getcwd(cwd, sizeof(cwd)))
            path = cwd;
        
        size_t n = path.size();
        if(n > 0 && path[n-1] != '/')
            path.append("/");
        return String::StringToWStringFast(path);
#endif      
    }
    
    MistString Path::GetHome() {
#if defined(MIST_OS_WINDOWS)
        MistString result = Path::GetEnv("HOMEDRIVE");
        result.append(Path::GetEnv("HOMEPATH"));
       
        size_t n = result.size();
        if(n > 0 && result[n-1] != L'\\')
            result.append(L"\\");
        return result;
        
#elif defined(MIST_OS_FAMILY_UNIX)
        std::string path;
        struct passwd* pwd = getpwuid(getuid());
        if(pwd)
            path = pwd->pw_dir;
        else {
            pwd = getpwuid(getuid());
            if(pwd) 
                path = pwd->pw_dir;
            else 
                return Path::GetEnv(L"HOME") + L"/";
        }
        
        size_t n = path.size();
        if(n > 0 && path[n-1] != '/')
            path.append("/");
        return String::StringToWStringFast(path);
#endif           
    }
    
    MistString Path::GetRoot() {
#if defined(MIST_OS_WINDOWS)
        
#elif defined(MIST_OS_FAMILY_UNIX)
        std::string path;
        char* tmp = getenv("TMPDIR");
        if(tmp) {
            path = tmp;
            size_t n = path.size();
            if(n > 0 && path[n-1] != '/')
                path.append("/");
            else 
                path = "/tmp/";
        }
        return String::StringToWStringFast(path);
#endif       
        return MistString();
    }
    
    MistString Path::GetTemp() {
#if defined(MIST_OS_WINDOWS)
        wchar_t buffer[_MAX_PATH];
        DWORD n = GetTempPathW(sizeof(buffer), buffer);
        if(n > 0 && n < sizeof(buffer)) {
            MistString result(buffer, n);
            if(result[n-1] != L'\\')
                result.append(L"\\");
            return result;
        } 
        
#elif defined(MIST_OS_FAMILY_UNIX)
        std::string path;
        char* tmp = getenv("TMPDIR");
        if(tmp) {
            path = tmp;
            std::string::size_type n = path.size();
            if(n > 0 && path[n-1] != '/')
                path.append("/");
            else 
                path = "/tmp/";
        }
        return String::StringToWStringFast(path);
        
#endif    
        return MistString();
    }
    
    MistString Path::GetWrittable() {
#if defined(MIST_OS_WINDOWS)
        return L"./";
        
#elif defined(MIST_OS_FAMILY_APPLE)
        return ukn_apple_documents_path();
        
#elif defined(MIST_OS_FAMILY_UNIX)
        return L"./";
#endif
    }
    
    MistString Path::GetResource() {
#if defined(MIST_OS_WINDOWS)
        return L"./";
        
#elif defined(MIST_OS_OSX)
        return GetApplicationPath() + L"/Contents/Resources/";

#elif defined(MIST_OS_IOS)
        return GetApplicationPath() + L"/";
        
#elif defined(MIST_OS_FAMILY_UNIX)
        return L"./";
#endif  
    }
    
    MistString Path::GetFont() {
#if defined(MIST_OS_WINDOWS)
        wchar_t buffer[_MAX_PATH];
        GetWindowsDirectoryW(buffer, _MAX_PATH-1);
        MistString path(buffer);
        return path + L"\\Fonts\\";
        
#elif defined(MIST_OS_FAMILY_APPLE)
        return L"/Library/Fonts/";
        
#elif defined(MIST_OS_LINUX)
        return L"/usr/share/fonts/";
        
#endif
        return L"./";
    }
    
    void Path::ListRoots(std::vector<MistString>& arr) {
        arr.clear();
        
#if defined(MIST_OS_WINDOWS)
        wchar_t buffer[128];
        DWORD n = GetLogicalDriveStringsW(sizeof(buffer)-1, buffer);
        wchar_t* it = buffer;
        wchar_t* end = buffer + (n > sizeof(buffer) ? sizeof(buffer): n);
        while(it < end) {
            std::wstring dev;
            while(it < end && *it)
                dev += *it++;
            arr.push_back(dev);
            ++it;
        }
        
#elif defined(MIST_OS_FAMILY_UNIX)
        arr.push_back(L"/");
#endif    
    }
    
    MistString Path::ExpandPath(const MistString& path) {
#if defined(MIST_OS_WINDOWS)
        wchar_t buffer[_MAX_PATH];
        DWORD n = ExpandEnvironmentStringsW(path.c_str(), buffer, sizeof(buffer));
        if(n > 0 && n < sizeof(buffer)) {
            return MistString(buffer, n-1);
        } else
            return path;
        
#elif defined(MIST_OS_FAMILY_UNIX)
        MistString result;
        MistString::const_iterator it  = path.begin();
        MistString::const_iterator end = path.end();
        if (it != end && *it == L'~') {
            ++it;
            if (it != end && *it == L'/') {
                result += Path::GetHome(); ++it;
            }
            else result += '~';
        }
        while (it != end) {
            if (*it == L'$') {
                std::wstring var;
                ++it;
                if (it != end && *it == L'{') {
                    ++it;
                    while (it != end && *it != L'}') var += *it++;
                    if (it != end) ++it;
                }
                else {
                    while (it != end && (((*it >= L'a' && *it <= L'z') || (*it >= L'A' && *it <= L'Z') || (*it >= L'0' && *it <= L'9')) || *it == L'_')) var += *it++;
                }
                result += Path::GetEnv(var);
            }
            else result += *it++;
        }
        return result;
#endif    
    }
    
} // namespace mist


#ifndef MIST_OS_WINDOWS

#include <dirent.h>

namespace mist {
    
    class DirectoryIterator::DirectoryIteratorImpl {
    public:
        DirectoryIteratorImpl(const MistString& path) {
            mDir = opendir(String::WStringToString((path+L"/")).c_str());
            
            if(mDir)
                next();
        }
        
        ~DirectoryIteratorImpl() {
            if(mDir)
                closedir(mDir);
        }
        
        void duplicate();
        void release();
        
        const MistString& get() const;
        const MistString& next() {
            if(!mDir)
                return mCurrent;
            
            do {
                struct dirent* entry = readdir(mDir);
                if(entry)
                    mCurrent = String::StringToWString(entry->d_name);
                else
                    mCurrent.clear();
            }
            while(mCurrent == L"." || mCurrent == L".." || (mCurrent.size() > 0 && mCurrent[0] == '.'));
            
            return mCurrent;
        }
        
    private:
        DIR* mDir;
        MistString mCurrent;
        int mRC;
    };
    
    const MistString& DirectoryIterator::DirectoryIteratorImpl::get() const {
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

namespace mist {
    
    class DirectoryIterator::DirectoryIteratorImpl {
    public:
        DirectoryIteratorImpl(const MistString& path) {
            std::MistString findPath = path + "/*";
            
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
        
        const MistString& get() const;
        const MistString& next() {
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
        MistString mCurrent;
        int mRC;
    };
    
    const std::MistString& DirectoryIterator::DirectoryIteratorImpl::get() const {
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
    
} // namespace mist

#endif // MIST_OS_WINDOWS

namespace mist {
    
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
    
    DirectoryIterator::DirectoryIterator(const MistString& path):
    mPath(path+L"/"),
    mImpl(new DirectoryIteratorImpl(path)) {
        mIsEnd = false;
        
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
            MistString n = mImpl->next();
            if(n.empty())
                mIsEnd = true;
            else {
                mFile = mPath + mImpl->get();
            }
        }
        return *this;
    }
    
}