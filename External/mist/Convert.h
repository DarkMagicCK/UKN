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
#include "mist/MathUtil.h"
#include "mist/Lazy.h"

#include <algorithm>

namespace mist {
    
    template<typename T>
    struct ToStringTrait {
        static inline MistString ToString(const T& t) {
            return string::AnyToWString(t);
        }
    };
    
    template<>
    struct ToStringTrait<std::string> {
        static inline MistString ToString(const std::string& t) {
            return string::StringToWString(t);
        }
    };
    
    class MIST_API Convert {
    public:
        template<typename T>
        static inline MistString ToString(const T& t) {
            return ToStringTrait<T>::ToString(t);
        }
        
        static bool     ToBoolean(const MistString& str);
        
        static int16    ToInt16(const MistString& str);
        static int32    ToInt32(const MistString& str);
        static int64    ToInt64(const MistString& str);
        
        static uint16   ToUInt16(const MistString& str);
        static uint32   ToUInt32(const MistString& str);
        static uint64   ToUInt64(const MistString& str);
        
        static uint8    ToByte(const MistString& str);
        static double   ToDouble(const MistString& str);
        
        /* space separated values */
        static float3   ToFloat3(const MistString& str);
        static float4   ToFloat4(const MistString& str);
        static float2   ToFloat2(const MistString& str);
        
        template<typename T1, typename T2>
        union _ReinterpretConvertUnion {
            T1 t1;
            T2 t2;
        };
        
        template<typename T1, typename T2>
        static T2 ReinterpretConvert(T1 t1) {
            _ReinterpretConvertUnion<T1, T2> t;
            t.t1 = t1;
            return t.t2;
        }
    };
    
    struct MIST_API FormatString {
        FormatString(const MistString& str);

        operator MistString();
        
        template<typename T>
        FormatString& operator,(const T& t);
        
    private:
        MistString doReplace();

        MistString mStr;
        std::vector<MistString> mItemsToReplace;
        Lazy<MistString> mConvertTask;
    };

    template<typename T>
    FormatString& FormatString::operator,(const T& t) {
        this->mItemsToReplace.push_back(Convert::ToString(t));
        return *this;
    }
    
    
    
} // namespace mist


#endif
