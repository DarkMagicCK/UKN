//
//  FileUtil.h
//  Mist
//
//  Created by Robert Bu on 11/30/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef MISTFileUtil_h
#define MISTFileUtil_h

#include "mist/Platform.h"
#include "mist/StringUtil.h"

#include <vector>

namespace mist {

#undef DeleteFile
#undef DeleteDirectory
#undef CreateDirectory
#undef CopyFile
#undef CopyDirectory
#undef FileExists
#undef PathExists
    
    class MIST_API File {
    public:
        static bool FileExists(const MistString& filepath);
        static bool PathExists(const MistString& path);
        
        static bool CopyFile(const MistString& src, const MistString& dst);
        static bool CopyDirectory(const MistString& src, const MistString& dst);
        
        static bool MoveFile(const MistString& src, const MistString& dst);
        static bool MoveDirectory(const MistString& src, const MistString& dst);
        
        static bool DeleteFile(const MistString& src);
        static bool DeleteDirectory(const MistString& src);
        
        static bool CreateDirectory(const MistString& dir);
        
        struct FileInfo {
            uint64 accessed_time; /* read etc */
            uint64 modified_time; /* write etc */
            uint64 creation_time; /* time since created */
            
            uint64 size; /* file size */
            
            uint32 num_links; /* number of links */
            uint64 serial; /* unique identifier */
        };
        FileInfo GetFileInfo(const MistString& src);
    };

    
    class MIST_API Path {
    public:
        static MistString GetEnv(const MistString& env);
        static MistString GetCurrent();
        static MistString GetHome();
        static MistString GetRoot();
        static MistString GetTemp();
        static MistString GetFont();
        static MistString GetWrittable();
        static MistString GetResource();
        static MistString GetApplicationPath();
        
        // check and get path of a font
        // priority follows current path, resource path, system font folder, asset resource name
        static MistString CheckAndGetFontPath(const MistString& name);
        
        static void ListRoots(std::vector<MistString>& arr);
        
        static MistString ExpandPath(const MistString& path);
    };
            
    class MIST_API DirectoryIterator {
    public:
        DirectoryIterator();
        DirectoryIterator(const DirectoryIterator& iterator);
        explicit DirectoryIterator(const MistString& path);
        ~DirectoryIterator();
        
        const MistString& file() const;
        const MistString& path() const;
        
        bool isEnd() const;
        
        DirectoryIterator& operator=(const DirectoryIterator& it);
        
        DirectoryIterator& operator++();
        
        bool operator == (const DirectoryIterator& iterator) const;
        bool operator != (const DirectoryIterator& iterator) const;
        
    private:
        MistString mPath;
        MistString mFile;
        bool mIsEnd;
        
        class DirectoryIteratorImpl;

        DirectoryIteratorImpl* mImpl;        
    };
    
    inline const MistString& DirectoryIterator::path() const {
        return mPath;
    }
    
    inline bool DirectoryIterator::isEnd() const {
        return mIsEnd;
    }
    
    inline const MistString& DirectoryIterator::file() const {
        return mFile;
    }
    
    inline bool DirectoryIterator::operator ==(const DirectoryIterator& rhs) const {
        return file() == rhs.file();
    }
    
    inline bool DirectoryIterator::operator !=(const DirectoryIterator& rhs) const {
        return file() != rhs.file();
    }
    
    
} // namespace mist

#endif
