//
//  Convert.h
//  Project Unknown
//
//  Created by Ruiwei Bu on 1/22/12.
//  Copyright (c) 2012 heizi. All rights reserved.
//

#ifndef Project_Unknown_Convert_h
#define Project_Unknown_Convert_h

#include "UKN/Platform.h"
#include "UKN/Util.h"
#include "UKN/Serializer.h"

namespace ukn {
    
    class Convert {
    public:
        template<typename T>
        static std::string ToString(T t);
        
        static int16    ToInt16(const std::string& str);
        static int32    ToInt32(const std::string& str);
        static int64    ToInt64(const std::string& str);
        
        static uint16   ToUInt16(const std::string& str);
        static uint32   ToUInt32(const std::string& str);
        static uint64   ToUInt64(const std::string& str);
        
        static uint8    ToByte(const std::string& str);
        static double   ToDouble(const std::string& str);
        
        static Array<uint16> ToUnicode(const std::string& str);
    };
    
    template<typename T>
    std::string Convert::ToString(T t) {
        return String::AnyToString(t);
    }
    
} // namespace ukn


#endif
