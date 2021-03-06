//
//  StringUtil.cpp
//  Mist
//
//  Created by Robert Bu on 11/23/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#include "mist/StringUtil.h"
#include "mist/MemoryUtil.h"
#include "mist/MathUtil.h"

namespace mist {
    
    static size_t utf8_strlen(const char* s) {
        size_t len = 0;
        for(; *s; ) {
            if(*s <= 0x7f)
                s += 1;
            else if(*s <= 0xc0)
                s += 2;
            else 
                s += 3;
            len ++;
        }
        return len;
    }

    StringTokenlizer::StringTokenlizer() {
        
    }
    
    StringTokenlizer::StringTokenlizer(const MistString& str) {
        parse(str, MistString());
    }
    
    StringTokenlizer::StringTokenlizer(const MistString& str, const MistString& deli) {
        parse(str, deli);
    }
    
    MistString StringTokenlizer::first() {
        if(mTokens.size() != 0)
            return mTokens.front();
        return MistString();
    }
    
    MistString StringTokenlizer::last() {
        if(mTokens.size() != 0)
            return mTokens.back();
        return MistString();
    }
    
    MistString StringTokenlizer::front() {
        if(mTokens.size() != 0)
            return mTokens.front();
        return MistString();
    }
    
    MistString StringTokenlizer::back() {
        if(mTokens.size() != 0)
            return mTokens.back();
        return MistString();
        
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
    
    bool StringTokenlizer::isSpace(wchar_t c) {
        return (c == L'\n' || c == L' ' || c == L'r' || c == L'\t');
    }
    
    bool StringTokenlizer::isAlpha(wchar_t c) {
        return ((c >= L'a' && c <= L'z') || (c >= L'A' && c <= L'Z') || (c >= L'0' && c <= L'9'));
    }
    
    void StringTokenlizer::parse(const MistString& str) {
        parse(str, MistString());
    }
    
    MistString StringTokenlizer::operator[](size_t index) {
        mist_assert(index < mTokens.size());
        
        return mTokens[index];
    }
    
    void StringTokenlizer::parse(const MistString& str, const MistString& deli) {
        size_t start = 0, end = 0;
        while(isSpace(str[start]) && start < str.size())
            start++;
        end = start;
        if(deli.size() != 0) {
            while(end < str.size()) {
                while((deli.find(str[end]) == MistString::npos) && end < str.size()) { ++end; }
                if(end == str.size()) {
                    --end;
                    while(deli.find(str[end]) != MistString::npos || isSpace(str[end]))
                        --end;
                    MistString s = str.substr(start, end-start+1);
                    if(!s.empty())
                        mTokens.push_back( s );
                    break;
                }
                if(end != start)
                    mTokens.push_back( str.substr(start, end-start) );
                
                ++end; 
                while((deli.find(str[end]) != MistString::npos || isSpace(str[end])) && end < str.size()) ++end;
                
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
    
    void StringTokenlizer::operator=(const MistString& str) {
        parse(str, MistString());
    }
    
    size_t StringTokenlizer::size() const {
        return mTokens.size();
    }
    
} // namespace mist

#ifdef MIST_OS_WINDOWS
#include <windows.h>
#elif defined(MIST_OS_IOS) || defined(MIST_OS_OSX)
#include "AppleStuff.h"
#endif

#include <cstring>
#include <locale.h>

namespace mist {
    
#ifdef MIST_OS_WINDOWS
	static std::string ukn_win_wstring_to_string(const std::wstring& pwszSrc) {
		int nLen = WideCharToMultiByte(CP_ACP, 0, pwszSrc.c_str(), -1, NULL, 0, NULL, NULL);
		if (nLen<= 0) return std::string("");
		char* pszDst = new char[nLen];
		if (NULL == pszDst) return std::string("");
		WideCharToMultiByte(CP_ACP, 0, pwszSrc.c_str(), -1, pszDst, nLen, NULL, NULL);
		pszDst[nLen -1] = 0;
		std::string strTemp(pszDst);
		delete [] pszDst;
		return strTemp;
	}
    
	static std::wstring ukn_win_string_to_wstring(const std::string& pszSrc) {
		if(pszSrc.size() == 0)
			return L"\0";
		int nSize = MultiByteToWideChar(CP_ACP, 0, pszSrc.c_str(), pszSrc.size(), 0, 0);
		if(nSize <= 0) return NULL;
		WCHAR *pwszDst = new WCHAR[nSize+1];
		if( NULL == pwszDst) return NULL;
		MultiByteToWideChar(CP_ACP, 0,(LPCSTR)pszSrc.c_str(), pszSrc.size(), pwszDst, nSize);
		pwszDst[nSize] = 0;
		if( pwszDst[0] == 0xFEFF)                    // skip Oxfeff
			for(int i = 0; i < nSize; i ++)
				pwszDst[i] = pwszDst[i+1];
		std::wstring wcharString(pwszDst);
		delete pwszDst;
		return wcharString;
	}
#endif
    
    static std::string ukn_c_wstring_to_string(const std::wstring& ws) {
        std::string curLocale = setlocale(LC_ALL, NULL);        // curLocale = "C";
        setlocale(LC_ALL, "chs");
        const wchar_t* _Source = ws.c_str();
        size_t _Dsize = 2 * ws.size() + 1;
        char *_Dest = new char[_Dsize];
        memset(_Dest,0,_Dsize);
        wcstombs(_Dest,_Source,_Dsize);
        std::string result = _Dest;
        delete []_Dest;
        setlocale(LC_ALL, curLocale.c_str());
        return result;
    }
    
    static std::wstring ukn_c_string_to_wstring(const std::string& s) {
        setlocale(LC_ALL, "chs"); 
        const char* _Source = s.c_str();
        size_t _Dsize = s.size() + 1;
        wchar_t *_Dest = new wchar_t[_Dsize];
        wmemset(_Dest, 0, _Dsize);
        mbstowcs(_Dest,_Source,_Dsize);
        std::wstring result = _Dest;
        delete []_Dest;
        setlocale(LC_ALL, "C");
        return result;
    }
    
    std::string string::WStringToString(const std::wstring& str) {
#if defined(MIST_OS_WINDOWS)
        return ukn_win_wstring_to_string(str);
#elif defined(MIST_OS_IOS) || defined(MIST_OS_OSX)
        return mist_apple_wstring_to_string(str);
#else
        return ukn_c_wstring_to_string(str);
#endif
    }
    
    std::wstring string::StringToWString(const std::string& str) {
#if defined(MIST_OS_WINDOWS)
        return ukn_win_string_to_wstring(str);
#elif defined(MIST_OS_IOS) || defined(MIST_OS_OSX)
        return mist_apple_string_to_wstring(str);
#else
        return ukn_c_string_to_wstring(str);
#endif  
    }
    
    std::string string::WStringToStringFast(const std::wstring& str) {
        std::string buffer(str.length(), ' ');
		std::copy(str.begin(), str.end(), buffer.begin());
		return buffer;
    }
    
    std::wstring string::StringToWStringFast(const std::string& str) {
        MistString buffer(str.length(), L' ');
		std::copy(str.begin(), str.end(), buffer.begin());
		return buffer; 
    }

    MistString string::Strip(const MistString& str) {
        size_t begin = 0;
        while((str[begin] == L' ' ||
                str[begin] == L'\n' ||
                str[begin] == L'\r')
               && begin < str.size()) {
            begin++;
        }
        size_t end = str.size() - 1;
        while((str[end] == L' ' ||
                str[end] == L'\n' ||
                str[end] == L'\r')
               && end > begin) {
            end --;
        }
        if(end > begin)
            return str.substr(begin, end+1);
        return str;
    }

    MistString string::GetFileName(const MistString& str) {
        MistString::const_iterator it = str.end();
        it--;
        
        while(it != str.begin() && *it != '/' && *it != '\\') {
            --it;
        }
        return MistString(it, str.end());
    }

    bool string::IsSpace(char c) {
        return c == ' ' || 
               c == '\n' ||
               c == '\t' ||
               c == '\r';
    }

    bool string::IsSpace(wchar_t c) {
        return c == L' ' || 
               c == L'\n' ||
               c == L'\t' ||
               c == L'\r';
    }

    bool string::IsNumber(char c) {
        return (c >= '0' && c <= '9');
    }

    bool string::IsNumber(wchar_t c) {
        return (c >= L'0' && c <= L'9');
    }

    bool string::IsAlphabet(wchar_t c) {
        if(!string::IsAscII(c))
            return false;
        return (c >= L'A' && c <= L'Z') ||
               (c >= L'a' && c <= L'z');
    }

    bool string::IsAlphabet(char c) {
        return (c >= 'A' && c <= 'Z') ||
               (c >= 'a' && c <= 'z');
    }

    bool string::IsAscII(wchar_t c) {
        return ((c&0x80) == 0);
    }
    
    MistString string::GetFilePath(const MistString& str) {
        if(str.empty())
            return str;
        
        MistString::const_iterator it = str.end();
        it--;
        
        while(it != str.begin() && *it != '/' && *it != '\\') {
            --it;
        }
        if(it != str.begin())
            return MistString(str.begin(), it+1);
        return MistString();
    }
    
    uint16 *utf8_to_unicode(uint16 *unicode, const char *utf8, size_t len) {
        int i, j;
        uint16 ch;
        
        for ( i=0, j=0; i < len; ++i, ++j ) {
            ch = ((const unsigned char *)utf8)[i];

            if ( ch >= 0xF0 ) {
                ch  =  (uint16)(utf8[i]&0x07) << 18;
                ch |=  (uint16)(utf8[++i]&0x3F) << 12;
                ch |=  (uint16)(utf8[++i]&0x3F) << 6;
                ch |=  (uint16)(utf8[++i]&0x3F);
            } else
                if ( ch >= 0xE0 ) {
                    ch  =  (uint16)(utf8[i]&0x0F) << 12;
                    ch |=  (uint16)(utf8[++i]&0x3F) << 6;
                    ch |=  (uint16)(utf8[++i]&0x3F);
                } else
                    if ( ch >= 0xC0 ) {
                        ch  =  (uint16)(utf8[i]&0x1F) << 6;
                        ch |=  (uint16)(utf8[++i]&0x3F);
                    }
            unicode[j] = ch;
        }
        unicode[j] = 0;
        
        return unicode;
    }
    
    int unicode_strlen(const uint16 *text) {
        int size = 0;
        while ( *text++ ) {
            ++size;
        }
        return size;
    }
    
    void unicode_strcpy(uint16 *dst, const uint16 *src, size_t swap) {
        if ( swap ) {
            while ( *src ) {
                *dst = math::flip_bytes(*src);
                ++src;
                ++dst;
            }
            *dst = '\0';
        } else {
            while ( *src ) {
                *dst = *src;
                ++src;
                ++dst;
            }
            *dst = '\0';
        }
    }

    bool EndsWith(const MistString& str, const MistString& end) {
        return str.rfind(end) == str.length() - end.length();
    }

    bool StartsWith(const MistString& str, const MistString& start) {
        return str.find(start) == 0;
    }

    
} // namespace mist
