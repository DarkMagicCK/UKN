//
//  Class.h
//  Mist
//
//  Created by Robert Bu on 12/5/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef MISTrf_TypeDatabase_h
#define MISTrf_TypeDatabase_h

#include "mist/Platform.h"

#include "mist/Preprocessor.h"
#include "mist/Interfaces.h"
#include "mist/Basic.h"
#include "mist/reflection/Type.h"

#include "mist/Event.h"
#include "mist/Singleton.h"

#include <map>
#include <vector>

namespace mist {
        
    // support POD only
    
    namespace reflection {
        
        class TypeDB: public virtual IEnumerable<Type>, Uncopyable {
        public:
            static TypeDB& Default();

            TypeDB();
            ~TypeDB();
            
            friend struct SingletonClassInstantiator<TypeDB>;
            
            template<typename T>
            Type& createType();
            
            template<typename T>
            Type* getType() const;
            
            template<typename T, size_t N>
            void setTypeFields(const FieldInfo (&init_fields)[N]);
            
            Type* getType(const Name& name) const;
            Type* getType(const TypeInfo& info) const;
            
        public:
            // enumerator
            IEnumerator<Type>* createEnumerator() const;
            
        private:
            friend class TypeDBEnumerator;
                        
            typedef std::map<uint32, Type*> TypeMap;
            TypeMap mTypes;
        };
        
        template<typename T>
        Type& TypeDB::createType() {
            Name tname(GetTypeName<T>(), GetTypeId<T>());
            
            Type* type = 0;
            TypeMap::const_iterator itType = mTypes.find(tname.hash);
            if(itType == mTypes.end()) {
                type = new Type(TypeInfo::Create<T>());

                this->mTypes[tname.hash] = type;

            } else {
                type = itType->second;
            }            
            return *type;
        }
        
        template<typename T>
        Type* TypeDB::getType() const {
            Name tname = TypeNameRetriever<typename traits::strip_pointer<T>::type>::Name();
            
            return this->getType(tname);
        }
        
        template<typename T, size_t N>
        void TypeDB::setTypeFields(const FieldInfo (&init_fields)[N]) {
            Type* type = this->getType(GetTypeName<T>());
            if(type != 0) {
                type->defFields(&init_fields[0], N);
            }
        }
        
        template<typename T>
        inline T* FieldCast(const Field* field, void* object) {
            Type* type = TypeDB::Default().getType<T>();
            if(type == field->type) {
                return reinterpret_cast<T*>((char*)object+field->offset);
            }
            return 0;
        }
        
        template<typename T>
        inline T* FieldCast(const Field* field, void* object, const TypeDB& db) {
            Type* type = db.getType<T>();
            if(type == field->type) {
                return reinterpret_cast<T*>((char*)object+field->offset);
            }
            return 0;
        }
        
        template<typename T>
        inline T* UnsafeFieldCast(const Field* field, void* object) {
            return reinterpret_cast<T*>((char*)object+field->offset);
        }
        
#define MIST_RF_BEGIN_TYPE_FIELDS(type) \
        struct TypeFields {    \
            typedef type Type; \
            \
            static void Set() { \
                mist::reflection::FieldInfo fields[] = { \

#define MIST_RF_TYPE_FIELD(name) mist::reflection::FieldInfo(#name, &Type::name, offsetof(Type, name))
#define MIST_RF_TYPE_FIELD_F(name, flags) mist::reflection::FieldInfo(#name, &Type::name, offsetof(Type, name)).defAttr(flags)
#define MIST_RF_TYPE_FIELD_F_D(name, flags, desc) mist::reflection::FieldInfo(#name, &Type::name, offsetof(Type, name)).defAttr(flags).defDesc(desc)
        
#define MIST_RF_TYPE_FIELD_2(type, name) mist::reflection::FieldInfo(#name, &type::name, offsetof(type, name))
#define MIST_RF_TYPE_FIELD_2_F(type, name, flags) mist::reflection::FieldInfo(#name, &type::name, offsetof(type, name)).defAttr(flags)
#define MIST_RF_TYPE_FIELD_2_F_D(type, name, flags, desc) mist::reflection::FieldInfo(#name, &type::name, offsetof(type, name)).defAttr(flags).defDesc(desc)
        
#define MIST_RF_END_TYPE_FIELDS() \
                }; \
                mist::reflection::TypeDB& type_db = mist::reflection::TypeDB::Instance(); \
                type_db.setTypeFields<Type>(fields); \
            } \
            \
            TypeFields() { \
            } \
        }; \
        static TypeFields Fields; \
        
    } // namespace detail
        
} // namespace mist

#endif