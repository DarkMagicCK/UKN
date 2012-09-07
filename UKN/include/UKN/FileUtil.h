//
//  FileUtil.h
//  Project Unknown
//
//  Created by Robert Bu on 11/30/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_FileUtil_h
#define Project_Unknown_FileUtil_h

#include "UKN/Platform.h"
#include "UKN/StringUtil.h"

#include <vector>

namespace ukn {
    
    class File {
    public:
        static bool FileExists(const UknString& filepath);
        static bool PathExists(const UknString& path);
    };

    
    class UKN_API Path {
    public:
        static UknString GetEnv(const UknString& env);
        static UknString GetCurrent();
        static UknString GetHome();
        static UknString GetRoot();
        static UknString GetTemp();
        static UknString GetFont();
        static UknString GetWrittable();
        static UknString GetResource();
        static UknString GetApplicationPath();
        
        // check and get path of a font
        // priority follows current path, resource path, system font folder, asset resource name
        static UknString CheckAndGetFontPath(const UknString& name);
        
        static void ListRoots(std::vector<UknString>& arr);
        
        static UknString ExpandPath(const UknString& path);
    };
            
    class UKN_API DirectoryIterator {
    public:
        DirectoryIterator();
        DirectoryIterator(const DirectoryIterator& iterator);
        explicit DirectoryIterator(const UknString& path);
        ~DirectoryIterator();
        
        const UknString& file() const;
        const UknString& path() const;
        
        bool isEnd() const;
        
        DirectoryIterator& operator=(const DirectoryIterator& it);
        
        DirectoryIterator& operator++();
        
        bool operator == (const DirectoryIterator& iterator) const;
        bool operator != (const DirectoryIterator& iterator) const;
        
    private:
        UknString mPath;
        UknString mFile;
        bool mIsEnd;
        
        class DirectoryIteratorImpl;

        DirectoryIteratorImpl* mImpl;        
    };
    
    inline const UknString& DirectoryIterator::path() const {
        return mPath;
    }
    
    inline bool DirectoryIterator::isEnd() const {
        return mIsEnd;
    }
    
    inline const UknString& DirectoryIterator::file() const {
        return mFile;
    }
    
    inline bool DirectoryIterator::operator ==(const DirectoryIterator& rhs) const {
        return file() == rhs.file();
    }
    
    inline bool DirectoryIterator::operator !=(const DirectoryIterator& rhs) const {
        return file() != rhs.file();
    }
    
    
} // namespace ukn

#endif
