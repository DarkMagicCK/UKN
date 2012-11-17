//
//  StringUtil.h
//  Mist
//
//  Created by Robert Bu on 11/23/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef MISTStringUtil_h
#define MISTStringUtil_h

#include "mist/Platform.h"

#include <vector>
#include <string>
#include <sstream>

namespace mist {
    
    class MIST_API String {
    public:
        static std::string WStringToString(const std::wstring& str);
        static std::wstring StringToWString(const std::string& str);
        
        // by direct data copy
        static std::string WStringToStringFast(const std::wstring& str);
        static std::wstring StringToWStringFast(const std::string& str);
        
        static MistString GetFileName(const MistString& str);
        static MistString GetFilePath(const MistString& str);
        
        template<typename T>
        static std::string AnyToString(const T& val) {
            std::ostringstream sstr;
            sstr << val;
            return sstr.str();
        }
        
        template<typename T>
        static std::wstring AnyToWString(const T& val) {
            std::wostringstream sstr;
            sstr << val;
            return sstr.str();
        }
    };
    
    class MIST_API StringTokenlizer {
    public:
        typedef std::vector<MistString> TokenContainer;
        typedef TokenContainer::iterator Iterator;
        typedef Iterator iterator;
        
        StringTokenlizer();
        StringTokenlizer(const MistString& str);
        StringTokenlizer(const MistString& str, const MistString& deli);
        
        void parse(const MistString& str);
        void parse(const MistString& str, const MistString& deli);
        
        MistString first();
        MistString last();
        
        MistString front();
        MistString back();
        
        iterator begin();
        iterator end();
        
        size_t size() const;
        
        void operator=(const MistString& str);
        
        const TokenContainer& getTokens() const;
        
        MistString operator[](size_t index);
        
    protected:
        bool isSpace(char c);
        bool isAlpha(char c);
        
        TokenContainer mTokens;
    };
    
    MIST_API uint16 *utf8_to_unicode(uint16 *unicode, const char *utf8, size_t len);
    MIST_API int unicode_strlen(const uint16 *text);
    MIST_API void unicode_strcpy(uint16 *dst, const uint16 *src, size_t swap);

    
} // namespace mist

#endif
