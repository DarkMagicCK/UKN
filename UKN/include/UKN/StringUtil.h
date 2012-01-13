//
//  StringUtil.h
//  Project Unknown
//
//  Created by Robert Bu on 11/23/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_StringUtil_h
#define Project_Unknown_StringUtil_h

#include "UKN/Platform.h"

#include <vector>
#include <string>
#include <sstream>

namespace ukn {
    
    enum StringFormat {
        SF_ANSI,
        SF_UTF8,
        SF_Unicode,
    };
    
    class UKN_API String: public std::wstring {
    public:
        String();
        String(const char* cstr, StringFormat format = SF_ANSI);
        String(const wchar_t* wcstr);
        String(const std::string& str, StringFormat format = SF_ANSI);
        String(const std::wstring& wstr);
        
        bool convert(const char* str, StringFormat format = SF_ANSI);
        
        StringFormat format() const;
        
        const char* ansi_str();
        const char* utf8_str();
        
        friend std::ostream& operator<<(std::ostream& os, const String& val);
        
    protected:
        StringFormat mFormat;
        
    private:
        char* mStrBuff;
        size_t mStrBuffSize;
    };
    
    class UKN_API StringTokenlizer {
    public:
        typedef std::vector<ukn_string> TokenContainer;
        typedef TokenContainer::iterator Iterator;
        typedef Iterator iterator;
        
        StringTokenlizer();
        StringTokenlizer(const ukn_string& str);
        StringTokenlizer(const ukn_string& str, const ukn_string& deli);
        
        void parse(const ukn_string& str);
        void parse(const ukn_string& str, const ukn_string& deli);
        
        ukn_string first();
        ukn_string last();
        
        ukn_string front();
        ukn_string back();
        
        iterator begin();
        iterator end();
        
        size_t size() const;
        
        void operator=(const ukn_string& str);
        void operator=(const char* str);
        
        const TokenContainer& getTokens() const;
        
        ukn_string operator[](size_t index);
        
    protected:
        bool isSpace(char c);
        bool isAlpha(char c);
        
        TokenContainer mTokens;
    };
    
    UKN_API ukn_string wstring_to_string(const ukn_wstring& str);
    UKN_API ukn_wstring string_to_wstring(const ukn_string& str);
    
    // by direct data copy
    UKN_API ukn_string wstring_to_string_fast(const ukn_wstring& str);
    UKN_API ukn_wstring string_to_wstring_fast(const ukn_string& str);
    
    UKN_API ukn_string get_file_name(const ukn_string& str);
    UKN_API ukn_wstring get_file_name(const ukn_wstring& str);
    
    UKN_API ukn_string get_file_path(const ukn_string& str);
    UKN_API ukn_wstring get_file_path(const ukn_wstring& str);

    template<typename T>
    ukn_string any_to_string(const T& val) {
        std::ostringstream sstr;
        sstr << val;
        return sstr.str();
    }
    
} // namespace ukn

#endif
