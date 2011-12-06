//
//  Class.h
//  Project Unknown
//
//  Created by Ruiwei Bu on 12/5/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_Class_h
#define Project_Unknown_Class_h

#include "Platform.h"

#include "Hashing.h"
#include "Ptr.h"
#include "Preprocessor.h"

#include <map>
#include <vector>

namespace ukn {
    
    namespace detail {
        
        static uint32 NextPropertyId() {
            static uint32 curr_id = 0;
            return curr_id++;
        }
        
        typedef std::map<ukn_string, uint32> PropertyNameIdMap;
        static PropertyNameIdMap& get_registered_property_map()  {
            static PropertyNameIdMap* g_map = new PropertyNameIdMap;
            return *g_map;
        }
    }
    
    template<typename T>
    struct RegisteredProperty;
    
#define UKN_REGISTER_PROPERTY(type) \
    template<> struct RegisteredProperty<type> { \
        operator ukn_string() { \
            return detail::get_registered_property_map().find(#type)->first; \
        } \
    }; \
    static struct UKN_JOIN(_register_ukn_property_impl_, type) { \
        UKN_JOIN(_register_ukn_property_impl_, type)() { \
            detail::PropertyNameIdMap::iterator it = detail::get_registered_property_map().find(#type); \
            if(it == detail::get_registered_property_map().end()) { \
                detail::get_registered_property_map().insert(std::make_pair(#type, detail::NextPropertyId()));\
            }\
        } \
    } UKN_JOIN(_register_ukn_property_impl_instance_, type); \
    struct UKN_JOIN(_ukn_is_property_registered, type) { \
        operator bool() { \
            return detail::get_registered_property_map().find(#type) != detail::get_registered_property_map().end(); \
        } \
    };
    
    UKN_REGISTER_PROPERTY(int);
    UKN_REGISTER_PROPERTY(ukn_string);
   
#define UKN_IS_PROPERTY_TYPE_REGISTERD(type) \
    UKN_JOIN(_ukn_is_property_registered, type)()
    
#define UKN_IS_PROPRTY_TYPE_OF(prop, type) \
    prop->getTypeName() == #type
    
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
    class Property: public PropertyBase {
    public:
        Property(const ukn_string& name):
        PropertyBase(RegisteredProperty<T>()),
        mIsDefault(true),
        mName(name) {
            
        }
        
        Property(const ukn_string& name, const T& val):
        PropertyBase(RegisteredProperty<T>()),
        mIsDefault(false),
        mName(name),
        mValue(val) {
            
        }
        
        Property(const Property<T>& rhs):
        PropertyBase(RegisteredProperty<T>()),
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
        }
        
        ukn_string  toString() const { 
            return ukn_string();
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
        PropertyBuilder(PropertyBase* prop)
        {
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
            mNameHash = crc32(name);
        }
        
        Class(const ukn_string& name, PropertyBuilder proplist) {
            std::vector<PropertyBase*>::const_iterator it = proplist.mProperties.begin();
            while(it != proplist.mProperties.end()) {
                mProperties.insert(std::make_pair((*it)->getName(), *it));
                
                ++it;
            }
        }
        
        const ukn_string& name() const { 
            return mName;
        }
        
        typedef std::map<ukn_string, PropertyPtr> PropertyMap;
        const PropertyMap& getPropertyMap() const {
            return mProperties;
        }
        
        PropertyPtr operator[](const ukn_string& name) {
            PropertyMap::iterator it = mProperties.find(name);
            if(it != mProperties.end()) {
                return it->second;
            }
            return PropertyPtr();
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
