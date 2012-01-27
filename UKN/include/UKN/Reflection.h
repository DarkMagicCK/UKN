//
//  Class.h
//  Project Unknown
//
//  Created by Robert Bu on 12/5/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_Class_h
#define Project_Unknown_Class_h

#include "UKN/Platform.h"

#include "UKN/Hashing.h"
#include "UKN/Preprocessor.h"
#include "UKN/Interfaces.h"
#include "UKN/Basic.h"

#include "detail/TypeTraits.h"

#include <map>
#include <vector>

namespace ukn {
        
    // support POD only
    
    namespace reflection {
        
        // to support custom type,
        // implement TypeNameRetriever with a static function 
        //      static const char* Name()
        // and register it with typedb
        template<typename TYPE>
        struct TypeNameRetriever;
        
        template<> struct TypeNameRetriever<int64>{ 
            static const char* Name() { return "int64"; }
        };
        
        template<> struct TypeNameRetriever<uint64>{ 
            static const char* Name() { return "uint64"; }
        };
        
        template<> struct TypeNameRetriever<int32>{ 
            static const char* Name() { return "int32"; }
        };
        
        template<> struct TypeNameRetriever<uint32>{ 
            static const char* Name() { return "uint32"; }
        };
        
        template<> struct TypeNameRetriever<int16>{ 
            static const char* Name() { return "int16"; }
        };
        
        template<> struct TypeNameRetriever<uint16>{ 
            static const char* Name() { return "uint16"; }
        };
        
        template<> struct TypeNameRetriever<int8>{ 
            static const char* Name() { return "int8"; }
        };
        
        template<> struct TypeNameRetriever<uint8>{ 
            static const char* Name() { return "uint8"; }
        };
        
        template<> struct TypeNameRetriever<float>{ 
            static const char* Name() { return "float"; }
        };
        
        template<> struct TypeNameRetriever<double>{ 
            static const char* Name() { return "double"; }
        };
        
        template<> struct TypeNameRetriever<ukn_string>{ 
            static const char* Name() { return "string"; }
        };
        
        template<> struct TypeNameRetriever<ukn_wstring>{ 
            static const char* Name() { return "wstring"; }
        };
        

        template<typename TYPE>
        static uint64 GetTypeId() {
            static uint64 type_id = Hash::Crc32(TypeNameRetriever<TYPE>::Name());
            return type_id;
        }
        
        struct Name {
            uint64      hash;
            const char* text;
            
            Name():
            hash(0),
            text(0) {
                
            }
            
            Name(const char* _name):
            text(_name) {
                hash = Hash::Crc32(_name);
            }
            
            Name(const char* _name, uint64 _hash):
            text(_name),
            hash(_hash){
                
            }
            
            bool operator == (const Name& rhs) const {
                return this->hash == rhs.hash;
            }
            
            bool operator < (const Name& rhs) const {
                return this->hash < rhs.hash;
            }
        };
        
        typedef void (*ConstructObjectFunc)(void*);
        typedef void (*DestructObjectFunc)(void*);
        
        struct Type;
        
        enum AttributeFlags {
            // pointer ?
            AF_Pointer      = 1UL << 0,
            
            // ignore during serialization ?
            AF_Transient    = 1UL << 1,
            
            // readonly data ?
            AF_ReadOnly     = 1UL << 2,
            
            // volatie ?
            AF_Volatile     = 1UL << 3,
            
            // mutable ?
            AF_Mutable      = 1UL << 4
        };
    
        // enumerations
        struct EnumConst {
            EnumConst(Name _name, int32 _value):
            name(_name),
            value(_value) {
                
            }
            
            Name  name;
            int32 value;
        };
        
        struct Field {
            template<typename TYPE, typename FIELD_TYPE>
            Field(Name _name, FIELD_TYPE (TYPE::*field), size_t _offset):
            name(_name),
            type_name(TypeNameRetriever<typename traits::strip_pointer<FIELD_TYPE>::type >::Name()),
            type(0),
            is_pointer(traits::is_pointer<FIELD_TYPE>::value),
            is_pod(traits::is_pod<FIELD_TYPE>::value),
            offset(_offset),
            size(sizeof(FIELD_TYPE)),
            flags(0) {
                
                
            }
            
            // extra attributes
            // attribute flags
            Field& defFlags(uint32 f) {
                this->flags = f;
                return *this;
            }
            
            Field& defDesc(const char* desc) {
                this->description = desc;
                return *this;
            }
            
            Field& defGroup(const char* group) {
                this->group = group;
                return *this;
            }
            
            // get the actual pointer to the field
            void* getPtr(void* object) {
                return (char*)object + offset;
            }
            
            // will check the type with TypeDB
            template<typename T>
            T* getCheckedPtr(void* object);
            
            Name    name;
            Name    type_name;
            Name    description;
            Name    group;
            
            Type*   type;
            
            bool    is_pointer;
            bool    is_pod;
            size_t  offset;
            uint32  size;
            
            uint32  flags;
        };

        struct Type {
            // parent type database
            class TypeDB* type_db;
            
            Name    name;
            // sizeof(TYPE);
            size_t  size;
            
            ConstructObjectFunc constructor;
            DestructObjectFunc  destructor;
            
            // fields
            template<int SIZE>
            Type& defFields(const Field (&init_fields)[SIZE]);
            
            Field* getField(Name name) {
                std::vector<Field>::iterator it = fields.begin();
                for(; it != fields.end(); ++it) {
                    if(it->name == name) {
                        return &*it;
                    }
                }
                if(base_type)
                    return base_type->getField(name);
                
                return 0;
            }
            
            std::vector<Field> fields;
            
            // enumerations
            
            template<int SIZE>
            Field& EnumConstants(EnumConst (&input_enum_consts)) {
                for(int i = 0; i < SIZE; ++i) {
                    enum_constants.push_back(input_enum_consts);
                }
            }
    
            std::vector<EnumConst> enum_constants;
            
            // base
            template<typename T>
            Type& defBase();
            
            Type* base_type;
        };
                
        // placement new
        template<typename T> void ConstructObject(void* obj) {
            new (obj) T;
        }
        
        template<typename T> void DestructObject(void* obj) {
            ((T*)obj)->T::~T();
        }
    
        class TypeDB: public virtual IEnumerable<Type> {
        private:
            TypeDB();
            ~TypeDB();
            
        public:
            static TypeDB& Instance();
            
            template<typename T>
            Type& createType();
            
            template<typename T, size_t N>
            void setTypeFields(const Field (&init_fields)[N]);
            
            Type* getType(Name name) const;
            
        public:
            // enumerator
            IEnumerator<Type>* createEnumerator() const;
            
        private:
            friend class TypeDBEnumerator;
            
            typedef std::map<Name, Type*> TypeMap;
            TypeMap mTypes;
        };
        
        template<typename T>
        Type& TypeDB::createType() {
            Name tname(TypeNameRetriever<T>::Name(), GetTypeId<T>());
            
            Type* type = 0;
            TypeMap::const_iterator itType = mTypes.find(tname);
            if(itType == mTypes.end()) {
                type = new Type;
            } else {
                type = itType->second;
            }
            
            type->name          = tname;
            type->type_db       = this;
            type->size          = sizeof(T);
            type->constructor   = ConstructObject<T>;
            type->destructor    = DestructObject<T>;
            
            this->mTypes[tname] = type;
            
            return *type;
        }
        
        template<typename T, size_t N>
        void TypeDB::setTypeFields(const Field (&init_fields)[N]) {
            Type* type = this->getType(TypeNameRetriever<T>::Name());
            if(type != 0) {
                type->defFields<N>(init_fields);
            }
        }
        
        template<int SIZE>
        Type& Type::defFields(const Field (&init_fields)[SIZE]) {
            for(int i = 0; i < SIZE; ++i) {
                Field f = init_fields[i];
                
                f.type = type_db->getType(f.type_name);
                this->fields.push_back(f);
            }
            return *this;
        }
        
        template<typename T>
        Type& Type::defBase() {
            this->base_type = type_db->getType(TypeNameRetriever<T>::Name());
            return *this;
        }
        
        template<typename T>
        T* Field::getCheckedPtr(void* object) {
            Type* db_type = type->type_db->getType(TypeNameRetriever<T>::Name());
            if(this->type != db_type)
                return 0;
            return (T*)((char*)object+offset);
        }
        
#define UKN_RF_BEGIN_TYPE_FIELDS(type) \
        struct TypeFields {    \
            typedef type Type; \
            \
            static void Set() { \
                ukn::reflection::Field fields[] = { \

#define UKN_RF_TYPE_FIELD(name) ukn::reflection::Field(#name, &Type::name, offsetof(Type, name))
#define UKN_RF_TYPE_FIELD_2(name, flags) ukn::reflection::Field(#name, &Type::name, offsetof(Type, name)).defFlags(flags)
#define UKN_RF_TYPE_FIELD_3(name, flags, desc) ukn::reflection::Field(#name, &Type::name, offsetof(Type, name)).defFlags(flags).defDesc(desc)
        
#define UKN_RF_END_TYPE_FIELDS() \
                }; \
                ukn::reflection::TypeDB& type_db = ukn::reflection::TypeDB::Instance(); \
                type_db.setTypeFields<Type>(fields); \
            } \
            \
            TypeFields() { \
            } \
        }; \
        static TypeFields Fields; \
        
    } // namespace detail
        
} // namespace ukn

#endif