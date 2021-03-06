//
//  Serializer.h
//  Mist
//
//  Created by Robert Bu on 12/14/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef MISTSerializer_h
#define MISTSerializer_h

#include "mist/Platform.h"
#include "mist/PreDeclare.h"
#include "mist/StringUtil.h"
#include "mist/MathUtil.h"
#include "mist/Logger.h"
#include "mist/StreamWrapper.h"
#include "mist/Basic.h"

#include <iostream>
#include <string>
#include <sstream>

namespace mist {
    
    enum SerializableTypeId {
        STI_Unknown = -1,
        
        STI_BOOL,
        
        STI_UINT16,
        STI_INT16,
        
        STI_UINT,
        
        STI_ULONG,
        STI_LONG,
        
        STI_BYTE,
        STI_CHAR,
        STI_WCHAR,
        
        STI_DOUBLE,
        
        STI_UINT64,
        STI_INT64,
        
        STI_STRING,
        
        STI_FLOAT,
        STI_FLOAT2,
        STI_FLOAT3,
        STI_FLOAT4,
        
        STI_INT,
        STI_INT2,
        STI_INT3,
        STI_INT4,
        
        STI_RAW
    };
    
    class MIST_API SerializeHelper {
    public:
        template<class T> static void               FromString(const MistString& str, T* val);
        template<class T> static T                  DefaultValue();
        
        template<class T> static MistString         ToString(T* val);
        template<class T> static SerializableTypeId GetTypeId();
    };
        
    class MIST_API IBinarySerializable: public virtual Interface {
    public:
        virtual bool serialize(const SharedPtr<StreamWriter>& serializer) = 0;
        virtual bool deserialize(const SharedPtr<StreamWriter>& serializer) = 0;
    };
    
    class MIST_API IConfigSerializable: public virtual Interface {
    public:
        virtual bool serialize(const ConfigParserPtr& config) = 0;
        virtual bool deserialize(const ConfigParserPtr& config) = 0;
    };

} // namespace mist

#endif
