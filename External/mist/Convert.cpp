//
//  Convert.cpp
//  Mist
//
//  Created by Ruiwei Bu on 1/22/12.
//  Copyright (c) 2012 heizi. All rights reserved.
//

#include "mist/Convert.h"
#include "mist/Serializer.h"
#include "mist/StringUtil.h"

namespace mist {
    
    bool Convert::ToBoolean(const MistString& str) {
        bool result;
        SerializeHelper::FromString(str, &result);
        return result;
    }
    
    int16 Convert::ToInt16(const MistString& str) {
        int16 result;
        SerializeHelper::FromString(str, &result);
        return result;
    }
    
    int32 Convert::ToInt32(const MistString& str) {
        int32 result;
        SerializeHelper::FromString(str, &result);
        return result;
    }
    
    int64 Convert::ToInt64(const MistString& str) {
        int64 result;
        SerializeHelper::FromString(str, &result);
        return result;
    }
    
    uint16 Convert::ToUInt16(const MistString& str) {
        uint16 result;
        SerializeHelper::FromString(str, &result);
        return result;
    }
    
    uint32 Convert::ToUInt32(const MistString& str) {
        uint32 result;
        SerializeHelper::FromString(str, &result);
        return result;
    }
    
    uint64 Convert::ToUInt64(const MistString& str) {
        uint64 result;
        SerializeHelper::FromString(str, &result);
        return result;
    }
    
    uint8 Convert::ToByte(const MistString& str) {
        uint8 result;
        SerializeHelper::FromString(str, &result);
        return result;
    }

    float3 Convert::ToFloat3(const MistString& str) {
        MistString buffer;
        size_t pos = 0;
        float3 result;
        while(str[pos] == L' ') 
            pos ++;
        for(int i=0; i<3; ++i) {
            while(str[pos] != ' ' && pos < str.size()) {
                buffer.push_back(str[pos]);
                pos ++;
            }
            result.value[i] = (float)Convert::ToDouble(buffer);
            buffer.clear();

            while(str[pos] == L' ' && pos < str.size()) 
                pos ++;
        }
        return result;
    }

    float4 Convert::ToFloat4(const MistString& str) {
        MistString buffer;
        size_t pos = 0;
        float4 result;
        while(str[pos] == L' ') 
            pos ++;
        for(int i=0; i<4; ++i) {
            while(str[pos] != ' ' && pos < str.size()) {
                buffer.push_back(str[pos]);
                pos ++;
            }
            result.value[i] = (float)Convert::ToDouble(buffer);
            buffer.clear();

            while(str[pos] == L' ' && pos < str.size()) 
                pos ++;
        }
        return result;
    }

    float2 Convert::ToFloat2(const MistString& str) {
        MistString buffer;
        size_t pos = 0;
        float2 result;
        while(str[pos] == L' ') 
            pos ++;
        for(int i=0; i<2; ++i) {
            while(str[pos] != ' ' && pos < str.size()) {
                buffer.push_back(str[pos]);
                pos ++;
            }
            result.value[i] = (float)Convert::ToDouble(buffer);
            buffer.clear();

            while(str[pos] == L' ' && pos < str.size()) 
                pos ++;
        }
        return result;
    }

    /*
    Array<uint16> Convert::ToUnicode(const MistString& str) {
        uint16* buffer = ukn_malloc_t(uint16, str.size()+1);
        utf8_to_unicode(buffer, str.c_str(), str.size());
        return Array<uint16>(buffer, str.size()+1);
    }
    */
    double Convert::ToDouble(const MistString& str) {
        double result;
        SerializeHelper::FromString(str, &result);
        return result;
    }
    
    FormatString::FormatString(const MistString& str):
    mStr(str),
    mConvertTask(Bind(this, &FormatString::doReplace)) {

    }

    FormatString::operator MistString() {
        return mConvertTask.value();
    }
    
    MistString FormatString::doReplace() {
        MistString::iterator it = mStr.begin();
        while(it != mStr.end()) {
            if(*it == L'{') {
                MistString numBuffer;

                MistString::iterator end = it;
                ++end;
                while(string::IsNumber(*end)) {
                    numBuffer.push_back(*end);
                    ++end;
                }
                if(*end == L'}') {
                    uint32 index = Convert::ToUInt32(numBuffer);
                    if(index < mItemsToReplace.size()) {
                        size_t cindex = it - mStr.begin();

                        mStr.replace(it, end+1, mItemsToReplace[index]);
                        
                        it = mStr.begin() + (mItemsToReplace[index].size() + cindex);
                        if(it >= mStr.end())
                            break;
                    }
                }
            }
            ++it;
        }
        return mStr;
    }

    
} // namespace mist
