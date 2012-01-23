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
#include "UKN/Ptr.h"
#include "UKN/Preprocessor.h"
#include "UKN/Serializer.h"

#include <map>
#include <vector>

#include "UKN/TypeInfo.h"

namespace ukn {
    
    // requires rtti
    
    namespace detail {
        
        struct type_name_map {
        protected:
            type_name_map() { }
        
        public:
            static type_name_map& Instance() {
                static type_name_map instance;
                return instance;
            }
            
            std::map<TypeInfo, ukn_string>& get_name_map() {
                return name_map;
            }
            
        private:
            std::map<TypeInfo, ukn_string> name_map;
        };
        
    } // namespace detail

    
    template<typename T>
    struct RegisteredType;
    
#define UKN_REGISTER_TYPE(type) \
    static struct UKN_JOIN(__ukn_type_registration_, type) { \
        UKN_JOIN(__ukn_type_registration_, type)() {\
            detail::type_name_map::Instance().get_name_map()[TypeInfo(typeid(type))] = #type; \
        }\
    } UKN_JOIN(__ukn_type_registration_instance_, type); \
    template<> struct RegisteredType<type> {    \
        operator ukn_string() { \
            return detail::type_name_map::Instance().get_name_map()[TypeInfo(typeid(type))]; \
        } \
    };
    
    template<typename T>
    inline ukn_string get_type_name() {
        return detail::type_name_map::Instance().get_name_map()[TypeInfo(typeid(T))];
    }
    
    class PropertyBase {
    public:  
        PropertyBase() {
            
        }
        
        PropertyBase(const ukn_string& typeName):
        mPropertyTypeName(typeName) {
            
        }
        
        virtual ~PropertyBase() { }
        
        virtual bool isNull() const = 0;
        virtual ukn_string getName() const = 0;
        
        virtual void fromString(const ukn_string& str) = 0;
        virtual ukn_string  toString() const = 0;
        
        const ukn_string& getTypeName() const {
            return mPropertyTypeName;
        }
        
    protected:
        friend class PropertyBuilder;
        
        ukn_string mPropertyTypeName;
    };
    
    template<typename T>
    class PropertyRef: public PropertyBase {
    public:
        PropertyRef(const ukn_string& name, T* prop_ptr):
        PropertyBase(RegisteredType<T>()),
        mName(name),
        mPropPtr(prop_ptr()) {
            
        }
        
        T* getProperty() const {
            return mPropPtr;
        }
        
        ukn_string getName() const { 
            return this->mName;
        }
        
        bool isNull() const {
            return mPropPtr != 0;
        }
        
        void fromString(const ukn_string& str) {
            *mPropPtr = SerializeHelper::FromString<T>(str);
        }
        
        ukn_string toString() const { 
            return SerializeHelper::ToString<T>(*mPropPtr);
        }
        
    private:
        ukn_string mName;
        T* mPropPtr;
    };
    
    template<typename T>
    class Property: public PropertyBase {
    public:
        Property(const ukn_string& name):
        PropertyBase(RegisteredType<T>()),
        mIsDefault(true),
        mName(name) {
            
        }
        
        Property(const ukn_string& name, const T& val):
        PropertyBase(RegisteredType<T>()),
        mIsDefault(false),
        mName(name),
        mValue(val) {
            
        }
        
        Property(const Property<T>& rhs):
        PropertyBase(RegisteredType<T>()),
        mIsDefault(rhs.mIsDefault),
        mName(rhs.mName),
        mValue(rhs.mValue) {
            
        }
        
        ukn_string getName() const { 
            return this->mName;
        }
        
        bool isNull() const {
            return mIsDefault;
        }
        
        void fromString(const ukn_string& str) {
            mValue = SerializeHelper::FromString<T>(str);
        }
        
        ukn_string toString() const { 
            return SerializeHelper::ToString<T>(mValue);
        }
        
        template<typename Y>
        Property& operator=(const Property<Y>& rhs) {
            if(this != &rhs) {
                this->mIsDefault = false;
                this->mName = rhs.mName;
                this->mValue = rhs.mValue;
            }
        }
        
        template<typename Y>
        Property& operator=(const Y& val) {
            this->mValue = val;
        }
        
    private:
        bool mIsDefault;
        T mValue;
        ukn_string mName;
    };
    
    typedef SharedPtr<PropertyBase> PropertyPtr;
    
    class PropertyBuilder {
    public:  
        PropertyBuilder(PropertyBase* prop) {
            mProperties.push_back(prop);
        }
        
        PropertyBuilder& operator, (const PropertyBuilder& rhs) {
            std::vector<PropertyBase*>::const_iterator it = rhs.mProperties.begin();
            while(it != rhs.mProperties.end()) {
                mProperties.push_back(*it);
                
                ++it;
            }
            return *this;
        }
        
    private:
        std::vector<PropertyBase*> mProperties;
        
        friend class Class;
    };
    
#define UKN_DEF_PROP(name, type, defaultVal) ukn::PropertyBuilder(new ukn::Property<type>(name, defaultVal))
    
    class Class {
    public:
        Class(const ukn_string& name):
        mName(name) {
            this->mNameHash = crc32(name);
        }
        
        Class(const ukn_string& name, PropertyBuilder proplist) {
            std::vector<PropertyBase*>::const_iterator it = proplist.mProperties.begin();
            while(it != proplist.mProperties.end()) {
                this->mProperties.insert(std::make_pair((*it)->getName(), *it));
                
                ++it;
            }
        }
        
        const ukn_string& name() const { 
            return this->mName;
        }
        
        bool operator==(const Class& rhs) const {
            return this->mName == rhs.mName;
        }
        
        typedef std::map<ukn_string, PropertyPtr> PropertyMap;
        const PropertyMap& getPropertyMap() const {
            return this->mProperties;
        }
        
        PropertyPtr operator[](const ukn_string& name) {
            PropertyMap::iterator it = this->mProperties.find(name);
            if(it != this->mProperties.end()) {
                return it->second;
            }
            return PropertyPtr();
        }
        
        void addProperty(const PropertyPtr& prop) {
            this->mProperties.insert(std::make_pair(prop->getName(), prop));
        }
        
    private:
        ukn_string mName;
        uint64     mNameHash;
        
        PropertyMap mProperties;
    };
    
#define UKN_DEF_CLASS(class) \
    public: \
        static ukn::Class& GetClass() { \
        static ukn::Class _class_instance(#class); \
            return _class_instance; \
        }
    
#define UKN_DEF_CLASS_2(class, prop) \
    public: \
        static ukn::Class& GetClass() { \
        static ukn::Class _class_instance(#class, prop); \
            return _class_instance; \
        }
    
} // namespace ukn

#endif