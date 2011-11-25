//
//  StringUtil.cpp
//  Project Unknown
//
//  Created by Ruiwei Bu on 11/23/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#include "UKN/StringUtil.h"

namespace ukn {
    
    StringTokenlizer::StringTokenlizer() {
        
    }
    
    StringTokenlizer::StringTokenlizer(const ukn_string& str) {
        parse(str, ukn_string());
    }
    
    StringTokenlizer::StringTokenlizer(const ukn_string& str, const ukn_string& deli) {
        parse(str, deli);
    }
    
    ukn_string StringTokenlizer::first() {
        if(mTokens.size() != 0)
            return mTokens.front();
        return ukn_string();
    }
    
    ukn_string StringTokenlizer::last() {
        if(mTokens.size() != 0)
            return mTokens.back();
        return ukn_string();
    }
    
    ukn_string StringTokenlizer::front() {
        if(mTokens.size() != 0)
            return mTokens.front();
        return ukn_string();
    }
    
    ukn_string StringTokenlizer::back() {
        if(mTokens.size() != 0)
            return mTokens.back();
        return ukn_string();
        
    }
    
    StringTokenlizer::iterator StringTokenlizer::begin() {
        return mTokens.begin();
    }
    
    StringTokenlizer::iterator StringTokenlizer::end() {
        return mTokens.end();
    }
    
    const StringTokenlizer::TokenContainer& StringTokenlizer::getTokens() const {
        return mTokens;
    }
    
    bool StringTokenlizer::isSpace(char c) {
        return (c == '\n' || c == ' ' || c == 'r' || c == '\t');
    }
    
    bool StringTokenlizer::isAlpha(char c) {
        return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'));
    }
    
    void StringTokenlizer::parse(const ukn_string& str) {
        parse(str, ukn_string());
    }
    
    ukn_string StringTokenlizer::operator[](size_t index) {
        ukn_assert(index < mTokens.size());
        
        return mTokens[index];
    }
    
    void StringTokenlizer::parse(const ukn_string& str, const ukn_string& deli) {
        size_t start = 0, end = 0;
        if(deli.size() != 0) {
            while(end < str.size()) {
                while((deli.find(str[end]) == ukn_string::npos) && end < str.size()) { ++end; }
                if(end == str.size()) {
                    while(deli.find(str[end]) != ukn_string::npos)
                        --end;
                    mTokens.push_back( str.substr(start, end-start) );
                    break;
                }
                if(end != start)
                    mTokens.push_back( str.substr(start, end-start) );
                
                ++end; 
                while((deli.find(str[end]) != ukn_string::npos || isSpace(str[end])) && end < str.size()) ++end;
                
                start = end;
            }
        } else {
            while(end < str.size()) {
                while(isAlpha(str[end]) && end < str.size()) { ++end; }
                if(end == str.size()) {
                    mTokens.push_back( str.substr(start) );
                    break;
                }
                mTokens.push_back( str.substr(start, end-start) );
                
                ++end; 
                while(!isAlpha(str[end]) && end < str.size()) {
                    if(str[end] == '"') {
                        ++end;
                        start = end;
                        while(str[end] != '"' && end < str.size())
                            ++end;
                        mTokens.push_back( str.substr(start, end-start) );
                    }
                    ++end;
                    
                }
                start = end;
            }
        }
    }
    
    void StringTokenlizer::operator=(const ukn_string& str) {
        parse(str, ukn_string());
    }
    
    void StringTokenlizer::operator=(const char* str) {
        parse(str, ukn_string());
    }
    
    size_t StringTokenlizer::size() const {
        return mTokens.size();
    }
    
} // namespace ukn
