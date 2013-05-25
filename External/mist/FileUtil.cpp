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
#include <Shlwapi.h>
#pragma comment(lib,"shlwapi.lib")

#include <shellapi.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <io.h>

#elif defined(MIST_OS_FAMILY_APPLE)
#include "AppleStuff.h"
#include <sys/stat.h>
#include <unistd.h>
#else
#include <sys/stat.h>
#endif // MIST_OS_FAMILTY_APPLE

#ifdef MIST_OS_FAMILY_UNIX   
#include <pwd.h>
#include <sys/fcntl.h>
#endif // MIST_OS_FAMILTY_UNIX

#include <cstdio>
#include <fstream>

#include "mist/StringUtil.h"
#include "mist/Logger.h"

namespace mist {

#undef CopyFile
#undef MoveFile
#undef DeleteFile
#undef CreateDirectory
    
    bool File::FileExists(const MistString& filepath) {
#ifdef MIST_OS_WINDOWS
        return PathFileExistsW(filepath.c_str())?true:false;
        
#elif defined(MIST_OS_FAMILY_APPLE)
        return mist_apple_file_exists(filepath);
#else
        
        struct stat sb;
        
        if (stat(Miststring::WStringToString(filepath).c_str(), &sb) == 0 && S_ISREG(sb.st_mode)) {
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
        
        if (stat(string::WStringToString(path).c_str(), &sb) == 0 && S_ISDIR(sb.st_mode)) {
            return true;
        }
#endif
        
        return false;
    }
    
#if defined(MIST_OS_FAMILY_UNIX)
    
    static bool mist_generic_cp_mv(const char* bin, const char *source, const char *dest, bool r = false) {
        int childExitStatus;
        pid_t pid;
        int status;
        if (!source || !dest) {
            return false;
        }
        
        pid = fork();
        
        if (pid == 0) {
            if(r)
                execl(bin, bin, "-R", source, dest, (char *)0);
            else
                execl(bin, bin, source, dest, (char *)0);
            
        }
        else if (pid < 0) {
            return false;
        }
        else {
            pid_t ws = waitpid( pid, &childExitStatus, WNOHANG);
            if (ws == -1) {
                return false;
            }
            
            if( WIFEXITED(childExitStatus)) {
                status = WEXITSTATUS(childExitStatus); /* zero is normal exit */
                return status == 0;
            }
        }
        return false;
    }
    
    static bool mist_generic_rm(const char *source, bool r = false) {
        int childExitStatus;
        pid_t pid;
        int status;
        if (!source) {
            return false;
        }
        
        pid = fork();
        
        if (pid == 0) {
            if(r)
                execl("/bin/rm", "/bin/rm", "-R", source, (char *)0);
            else
                execl("/bin/rm", "/bin/rm", source, (char *)0);
            
        }
        else if (pid < 0) {
            return false;
        }
        else {
            pid_t ws = waitpid( pid, &childExitStatus, WNOHANG);
            if (ws == -1) {
                return false;
            }
            
            if( WIFEXITED(childExitStatus)) {
                status = WEXITSTATUS(childExitStatus); /* zero is normal exit */
                return status == 0;
            }
        }
        return false;
    }
#endif
    
    
    bool File::CopyFile(const MistString& src, const MistString& dst) {
     
#if defined(MIST_OS_WINDOWS)
        
        return ::CopyFileW(src.c_str(),
                           dst.c_str(),
                           FALSE) == TRUE;
        
#elif defined(MIST_OS_FAMILY_APPLE)
        return mist_apple_copyitem(src, dst);
        
#elif defined(MIST_OS_FAMILY_UNIX)
        return mist_generic_cp_mv("/bin/cp",
                                  string::WStringToString(src).c_str(),
                                  string::WStringToString(dst).c_str());
        
#endif
        
    }
    
    bool File::CopyDirectory(const MistString& src, const MistString& dst) {
#if defined(MIST_OS_WINDOWS)
        SHFILEOPSTRUCT s = { 0 };
        s.hwnd = 0;
        s.wFunc = FO_COPY;
        s.fFlags = FOF_SILENT;
        s.pTo = dst.c_str();
        s.pFrom = src.c_str();
        return SHFileOperation(&s) == 0;
        
#elif defined(MIST_OS_FAMILY_APPLE)
        return mist_apple_copyitem(src, dst);      
        
#elif defined(MIST_OS_FAMILY_UNIX)
        return mist_generic_cp_mv("/bin/cp",
                                  string::WStringToString(src).c_str(),
                                  string::WStringToString(dst).c_str(),
                                  true);
        
#endif
        
    }
    
    bool File::MoveFile(const MistString& src, const MistString& dst) {
#if defined(MIST_OS_WINDOWS)
        return ::MoveFileW(src.c_str(),
                           dst.c_str()) == TRUE;
        
#elif defined(MIST_OS_FAMILY_APPLE)
        return mist_apple_moveitem(src, dst);
        
#elif defined(MIST_OS_FAMILY_UNIX)
        return ::rename(string::WStringToString(src).c_str(),
                        string::WStringToString(dst).c_str()) == 0;
        
#endif
    }
    
    bool File::MoveDirectory(const MistString& src, const MistString& dst) {
#if defined(MIST_OS_WINDOWS)
        return ::MoveFileW(src.c_str(),
                           dst.c_str()) == TRUE;
        
#elif defined(MIST_OS_FAMILY_APPLE)
        return mist_apple_moveitem(src, dst);
        
#elif defined(MIST_OS_FAMILY_UNIX)
        return ::rename(string::WStringToString(src).c_str(),
                        string::WStringToString(dst).c_str()) == 0;
        
#endif
    }
    
    bool File::DeleteFile(const MistString& src) {
#if defined(MIST_OS_WINDOWS)
        return ::DeleteFileW(src.c_str()) == TRUE;
        
#elif defined(MIST_OS_FAMILY_APPLE)
        return mist_apple_deleteitem(src);
        
        
#elif defined(MIST_OS_FAMILY_UNIX)
        return ::remove(string::WStringToString(src).cstr()) == 0;
        
#endif
    }
    
    bool File::DeleteDirectory(const MistString& src) {
#if defined(MIST_OS_WINDOWS)
        return ::RemoveDirectoryW(src.c_str()) == TRUE;
        
#elif defined(MIST_OS_FAMILY_APPLE)
        return mist_apple_deleteitem(src);
        
#elif defined(MIST_OS_FAMILY_UNIX)
        return mist_generic_rm(string::WStringToString(src).c_str(),
                               true);
        
#endif
    }
    
    bool File::CreateDirectory(const MistString& dir){
#if defined(MIST_OS_WINDOWS)
        return ::CreateDirectoryW(dir.c_str(),
                                  NULL) == TRUE;
        
#elif defined(MIST_OS_FAMILY_APPLE)

        return mist_apple_createdirectory(dir);
        
#elif defined(MIST_OS_FAMILY_UNIX)
        return mkdir(string::WStringToString(dir).c_str(),
                     S_IRWXU) == 0;
        
#endif
    }
    
    File::FileInfo File::GetFileInfo(const MistString& src) {
        FileInfo info;
#if defined(MIST_OS_WINDOWS)
        struct __stat64 fs;
#else
        struct stat fs;
#endif

#if defined(MIST_OS_WINDOWS)
		
		int fd = ::_open(string::WStringToString(src).c_str(),
                         O_RDONLY);
#else
        int fd = ::open(string::WStringToString(src).c_str(),
                        O_RDONLY);
#endif

#if defined(MIST_OS_WINDOWS)
        ::_fstat64(fd, &fs);
#else
        ::fstat(fd, &fs);
#endif
        
#if defined(MIST_OS_WINDOWS)
		info.creation_time = fs.st_ctime;
        info.accessed_time = fs.st_atime;
        info.modified_time = fs.st_mtime;
#else
		info.creation_time = fs.st_ctimespec.tv_sec;
        info.accessed_time = fs.st_atimespec.tv_sec;
        info.modified_time = fs.st_mtimespec.tv_sec;
#endif

        info.size = fs.st_size;
        info.num_links = fs.st_nlink;
        info.serial = fs.st_uid;
        
        ::close(fd);
        
        return info;
    }
    
    
    MistString Path::GetApplicationPath() {
#ifdef MIST_OS_WINDOWS
        wchar_t buffer[MAX_PATH];
        GetCurrentDirectoryW(MAX_PATH, buffer);
        
        return MistString(buffer)+L"/";
        
#elif defined(MIST_OS_FAMILY_APPLE)
        return mist_apple_application_path() + L"/";
        
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
            MistString fullpath(mist_apple_documents_path());
            
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
            wchar_t* buffer = new wchar_t[len];
            GetEnvironmentVariableW(env.c_str(), buffer, len);
            MistString result(buffer);
            delete [] buffer;
            return result;
        }
        
#elif defined(MIST_OS_FAMILY_UNIX)
        return string::StringToWStringFast(getenv(string::WStringToString(env).c_str()));
#endif
        
        return MistString();
    }
    
    MistString Path::GetCurrent() {
#if defined(MIST_OS_WINDOWS)
        wchar_t buffer[_MAX_PATH];
        DWORD n = GetCurrentDirectoryW(sizeof(buffer), buffer);
        if(n > 0) {
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
        return string::StringToWStringFast(path);
#endif      
        return MistString(L"./");
    }
    
    MistString Path::GetHome() {
#if defined(MIST_OS_WINDOWS)
        MistString result = Path::GetEnv(L"HOMEDRIVE");
        result.append(Path::GetEnv(L"HOMEPATH"));
       
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
        return string::StringToWStringFast(path);
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
        return string::StringToWStringFast(path);
#endif       
        return MistString();
    }
    
    MistString Path::GetTemp() {
#if defined(MIST_OS_WINDOWS)
        wchar_t buffer[_MAX_PATH];
        
#ifndef MIST_OS_WINRT
        DWORD n = GetTempPathW(sizeof(buffer), buffer);
        if(n == 0) {
            log_error("GetTempPathW failed");
            return MistString(L"./temp/");
        } 
#else
        auto folder = Window::Storage::ApplicationData::Current->TemporaryFolder;
        wcscpy_s(buffer, _MAX_PATH, folder->Path->Data());
        
#endif
        MistString result(buffer, n);
        if(result[n-1] != L'\\')
            result.append(L"\\");
        return result;
        
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
        return string::StringToWStringFast(path);
        
#endif    
        return MistString(L"./temp/");
    }
    
    MistString Path::GetWrittable() {
#if defined(MIST_OS_WINDOWS)
        return L"./";
        
#elif defined(MIST_OS_FAMILY_APPLE)
        return mist_apple_documents_path();
        
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

    MistString Path::GetDirectory(const MistString& path) {
        size_t rs = path.rfind(L'/');
        if(rs == MistString::npos) {
            rs = path.rfind(L'\\');
        }
        if(rs != MistString::npos) {
            return MistString(path.begin(), path.begin() + rs + 1);
        }
        return MistString(L"");
    }

    MistString Path::GetFileName(const MistString& path) {
        size_t rs = path.rfind(L'/');
        if(rs == MistString::npos) {
            rs = path.rfind(L'\\');
        }
        if(rs != MistString::npos) {
            return MistString(path.begin() + (rs+1), path.end());
        }
        return path;
    }

    MistString Path::GetFileNameWithoutExtension(const MistString& path) {
        MistString filename = Path::GetFileName(path);
        size_t rs = filename.rfind(L'.');
        if(rs != MistString::npos) {
            return MistString(filename.begin(), filename.begin() + rs);
        }
        return filename;
    }

    MistString Path::GetExtension(const MistString& path) {
        MistString filename = Path::GetFileName(path);
        size_t rs = filename.rfind(L'.');
        if(rs != MistString::npos) {
            return MistString(filename.begin() + rs + 1, filename.end());
        }
        return L"";
    }
    
} // namespace mist


#ifndef MIST_OS_WINDOWS

#include <dirent.h>

namespace mist {
    
    class DirectoryIterator::DirectoryIteratorImpl {
    public:
        DirectoryIteratorImpl(const MistString& path) {
            mDir = opendir(string::WStringToString((path+L"/")).c_str());
            
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
                    mCurrent = string::StringToWString(entry->d_name);
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
            MistString findPath = path + L"/*";
            
            mFH = FindFirstFileW(findPath.c_str(), &mFD);
            if(mFH == INVALID_HANDLE_VALUE) {
				DWORD error = GetLastError();
                if(error != ERROR_NO_MORE_FILES) {
                    
                }
            } else {
                mCurrent = mFD.cFileName;
                if(mCurrent == L"." || mCurrent == L"..")
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
                if(FindNextFileW(mFH, &mFD) != 0) {
                    mCurrent = mFD.cFileName;
                } else 
                    mCurrent.clear();
            }
            while(mCurrent == L"." || mCurrent == L"..");
            
            return mCurrent;
        }
        
    private:
        HANDLE mFH;
        WIN32_FIND_DATAW mFD;
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