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
        static bool FileExists(const ukn_string& filepath);
        static bool PathExists(const ukn_string& path);
    };

    
    class UKN_API Path {
    public:
        static ukn_string GetEnv(const ukn_string& env);
        static ukn_string GetCurrent();
        static ukn_string GetHome();
        static ukn_string GetRoot();
        static ukn_string GetTemp();
        static ukn_string GetFont();
        static ukn_string GetWrittable();
        static ukn_string GetResource();
        static ukn_string GetApplicationPath();
        
        // check and get path of a font
        // priority follows current path, resource path, system font folder, asset resource name
        static ukn_string CheckAndGetFontPath(const ukn_string& name);
        
        static void ListRoots(std::vector<ukn_string>& arr);
        
        static ukn_string ExpandPath(const ukn_string& path);
    };
            
    class UKN_API DirectoryIterator {
    public:
        DirectoryIterator();
        DirectoryIterator(const DirectoryIterator& iterator);
        explicit DirectoryIterator(const ukn_string& path);
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
        
        class DirectoryIteratorImpl;

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
    
    
} // namespace ukn

#endif
