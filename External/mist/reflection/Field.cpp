//
//  Field.cpp
//  Mist
//
//  Created by Ruiwei Bu on 1/27/12.
//  Copyright (c) 2012 heizi. All rights reserved.
//

#include "mist/reflection/Field.h"
#include "mist/reflection/TypeDatabase.h"

namespace mist {
    
    namespace reflection {
        
        Field::Field():
        type(0),
        offset(0),
        attributes(0) {
            
        }
        
        Field::Field(const FieldInfo& info, class TypeDB& db):
        name(info.name),
        description(info.description),
        type(db.getType(info.type_info.name)),
        is_pointer(info.is_pointer),
        offset(info.offset),
        attributes(info.attributes),
        container(info.container) {
            if(container) {
                if(info.container_value_info != TypeInfo()) {
                    container->value_type = db.getType(info.container_value_info);
                    container->value_is_pointer = info.container_value_info.is_pointer;
                }
            }
        }
        
        Field& Field::defAttr(FieldAttribute f) {
            this->attributes = f;
            return *this;
        }
        
        Field& Field::defDesc(const char* desc) {
            this->description = desc;
            return *this;
        }
        
        Field& Field::defGroup(const char* group) {
            this->group = group;
            return *this;
        }
        
        // get the actual pointer to the field
        void* Field::getPtr(void* object) const {
            return (char*)object + offset;
        }
        
        
        bool Field::isContainer() const {
            return container != 0;
        }
        
        typedef SharedPtr<IReadIterator, SharedPtrFreeReleasePolicy<IReadIterator> > ReadIteratorPtr;
        typedef SharedPtr<IWriteIterator, SharedPtrFreeReleasePolicy<IWriteIterator> > WriteIteratorPtr;
        
        ReadIteratorPtr Field::getReadIterator(void* object) const {
            void* buffer = mist_malloc(container->getReadIteratorSize());
            return ReadIteratorPtr(container->newReadIterator(buffer, this->getPtr(object)));
        }
        
        WriteIteratorPtr Field::getWriteIterator(void* object) const {
            void* buffer = mist_malloc(container->getWriteIteratorSize());
            return WriteIteratorPtr(container->newWriteIterator(buffer, this->getPtr(object)));
        }
    }
    
} // namespace mist
