//
//  Convert.h
//  Mist
//
//  Created by Ruiwei Bu on 1/22/12.
//  Copyright (c) 2012 heizi. All rights reserved.
//

#ifndef MISTConvert_h
#define MISTConvert_h

#include "mist/Platform.h"
#include "mist/Util.h"
#include "mist/Serializer.h"

namespace mist {
    
    class Convert {
    public:
        template<typename T>
        static MistString ToString(T t);
        
        static bool     ToBoolean(const MistString& str);
        
        static int16    ToInt16(const MistString& str);
        static int32    ToInt32(const MistString& str);
        static int64    ToInt64(const MistString& str);
        
        static uint16   ToUInt16(const MistString& str);
        static uint32   ToUInt32(const MistString& str);
        static uint64   ToUInt64(const MistString& str);
        
        static uint8    ToByte(const MistString& str);
        static double   ToDouble(const MistString& str);
    };
    
    template<typename T>
    MistString Convert::ToString(T t) {
        return String::AnyToWString(t);
    }
    
} // namespace mist


#endif
