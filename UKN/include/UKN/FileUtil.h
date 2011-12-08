//
//  FileUtil.h
//  Project Unknown
//
//  Created by Ruiwei Bu on 11/30/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_FileUtil_h
#define Project_Unknown_FileUtil_h

#include "UKN/Platform.h"

namespace ukn {
    
    UKN_API bool file_exists(const ukn_wstring& filepath);
    UKN_API bool path_exists(const ukn_wstring& path);
    
    UKN_API ukn_wstring get_writtable_path(const ukn_wstring& filePath);
    
    UKN_API ukn_wstring get_application_path();
            
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
