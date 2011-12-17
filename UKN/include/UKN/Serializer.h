//
//  Serializer.h
//  Project Unknown
//
//  Created by Ruiwei Bu on 12/14/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_Serializer_h
#define Project_Unknown_Serializer_h

#include "UKN/Platform.h"
#include "UKN/PreDeclare.h"
#include "UKN/StringUtil.h"
#include "UKN/MathUtil.h"
#include "UKN/Logger.h"

#include <iostream>
#include <string>
#include <sstream>

namespace ukn {
    
    enum SerializableTypeId {
        STI_Unknown = -1,
        
        STI_UINT = 1,
        STI_INT,
        
        STI_ULONG,
        STI_LONG,
        
        STI_CHAR,
        
        STI_FLOAT,
        STI_DOUBLE,
        
        STI_UINT64,
        STI_INT64,
        
        STI_STRING,
        STI_WSTRING,
        
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
        template<typename T> void unserialize(T* obj);
        
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
            *mSerializeStream << *obj;
        } else 
            log_error("ukn::Serializer::unserialize: invalid stream");
    }
    
    template<typename T>
    void BinarySerializer::unserialize(T* obj) {
        if(this->mSerializeStream) {
#ifdef UKN_DEBUG

#endif
            *mSerializeStream >> *obj;
        } else 
            log_error("ukn::Serializer::unserialize: invalid stream");
    }
    
    class ConfigSerializable {
    public:
        virtual bool serialize(const ConfigParserPtr& config) = 0;
        virtual bool unserialize(const ConfigParserPtr& config) = 0;
    };
    
} // namespace ukn

#endif
