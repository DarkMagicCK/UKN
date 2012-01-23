//
//  Serializer.h
//  Project Unknown
//
//  Created by Robert Bu on 12/14/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_Serializer_h
#define Project_Unknown_Serializer_h

#include "UKN/Platform.h"
#include "UKN/PreDeclare.h"
#include "UKN/StringUtil.h"
#include "UKN/MathUtil.h"
#include "UKN/Logger.h"
#include "UKN/Stream.h"
#include "UKN/Basic.h"

#include <iostream>
#include <string>
#include <sstream>

namespace ukn {
    
    enum SerializableTypeId {
        STI_Unknown = -1,
        
        STI_BOOL,
        
        STI_UINT16,
        STI_INT16,
        
        STI_UINT,
        STI_INT,
        
        STI_ULONG,
        STI_LONG,
        
        STI_BYTE,
        STI_CHAR,
        
        STI_FLOAT,
        STI_DOUBLE,
        
        STI_UINT64,
        STI_INT64,
        
        STI_STRING,
        
        STI_RAW
    };
    
    class SerializeHelper {
    public:
        template<class T> static void               FromString(const ukn_string& str, T* val);
        template<class T> static T                  DefaultValue();
        
        template<class T> static ukn_string         ToString(T* val);
        template<class T> static SerializableTypeId GetTypeId();
    };
    
    class BinarySerializer: Uncopyable {
    public:
        BinarySerializer(const StreamPtr& stream = StreamPtr());
        
        ~BinarySerializer() { }
        
        template<typename T> void serialize(T* obj);
        template<typename T> void deserialize(T* obj);
        
        const StreamPtr& getStream() const {
            return this->mSerializeStream;
        }
        
    private:
        StreamPtr mSerializeStream;
    };
    
    template<typename T>
    void BinarySerializer::serialize(T* obj) {
        if(this->mSerializeStream) {
#ifdef UKN_DEBUG
          //  *mSerializeStream << SerializeHelper::GetTypeId<T>();
#endif
            mSerializeStream->write(obj, sizeof(T));
        } else 
            log_error("ukn::Serializer::deserialize: invalid stream");
    }
    
    template<typename T>
    void BinarySerializer::deserialize(T* obj) {
        if(this->mSerializeStream) {
#ifdef UKN_DEBUG

#endif
            mSerializeStream->read(obj, sizeof(T));
        } else 
            log_error("ukn::Serializer::deserialize: invalid stream");
    }
    
    class IBinarySerializable: public virtual Interface {
    public:
        virtual bool serialize(BinarySerializer& serializer) = 0;
        virtual bool deserialize(BinarySerializer& serializer) = 0;
    };
    
    class IConfigSerializable: public virtual Interface {
    public:
        virtual bool serialize(const ConfigParserPtr& config) = 0;
        virtual bool deserialize(const ConfigParserPtr& config) = 0;
    };
    
} // namespace ukn

#endif
