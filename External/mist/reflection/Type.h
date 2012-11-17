//
//  Type.h
//  Mist
//
//  Created by Ruiwei Bu on 1/27/12.
//  Copyright (c) 2012 heizi. All rights reserved.
//

#ifndef MISTrf_Type_h
#define MISTrf_Type_h

#include "mist/reflection/Field.h"

#include <map>
#include <vector>

namespace mist {
    
    namespace reflection {
        
        // enumerations
        struct EnumConst {
            EnumConst(Name _name, int32 _value):
            name(_name),
            value(_value) {
                
            }
            
            Name  name;
            int32 value;
        };
        
        struct Type {
            Type(const TypeInfo& info);
            
            Name    name;
            size_t  size;
            bool    is_pointer;
            bool    is_pod;
            
            ConstructObjectFunc constructor;
            DestructObjectFunc  destructor;

            // fields
            const Field* getField(const Name& name) const;
            
            template<size_t SIZE>
            Type& defFields(const FieldInfo (&fields)[SIZE], TypeDB& db);
            
            // base
            Type& defBase(Type* type);
            Type* getBase() const;
            
        protected:
            friend class TypeDB;
            Type& defFields(const FieldInfo* fields, size_t size, TypeDB& db);
            
            typedef std::map<uint32, Field> FieldMap;
            FieldMap mFields;

            Type* mBaseTypes;
        };
        
        template<size_t SIZE>
        Type& Type::defFields(const FieldInfo (&fields)[SIZE], TypeDB& db) {
            return this->defFields(&fields[0], SIZE, db);
        }
        
    } // namespace reflection
    
} // namespace mist

#endif
