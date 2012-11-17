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
    
} // namespace mist
