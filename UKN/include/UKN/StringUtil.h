//
//  StringUtil.h
//  Project Unknown
//
//  Created by Robert Bu on 11/23/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_StringUtil_h
#define Project_Unknown_StringUtil_h

#include "Platform.h"

#include <vector>

namespace ukn {
    
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
    
    UKN_API ukn_string ukn_wstring_to_string(const ukn_wstring& str);
    UKN_API ukn_wstring ukn_string_to_wstring(const ukn_string& str);
    
    // by direct data copy
    UKN_API ukn_string ukn_wstring_to_string_fast(const ukn_wstring& str);
    UKN_API ukn_wstring ukn_string_to_wstring_fast(const ukn_string& str);
    
} // namespace ukn

#endif
