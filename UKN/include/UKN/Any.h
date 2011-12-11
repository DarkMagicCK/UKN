//
//  Any.h
//  Sora
//
//  Created by Robert Bu on 7/8/11.
//  Copyright 2011 Robert Bu(Project Hoshizora). All rights reserved.
//

#ifndef ukn_Any_h
#define ukn_Any_h

#include "UKN/Platform.h"
#include "UKN/Exception.h"

#include <algorithm>
#include <typeinfo>

namespace ukn {
    
    /**
     * A class that holds a general type
     * Code from Boost Library and Poco Library, Original Copyright by Kevlin Henney and Applied Infomatics
     * Modified for Hoshizora by Griffin Bu
     **/
    
    class Any {
    public:
        Any():
        mContent(0) {
        }
        
        template<typename ValueType>
        explicit Any(const ValueType& value):
        mContent(new Holder<ValueType>(value)) {
        }
        
        Any(const Any& other):
        mContent(other.mContent ? other.mContent->clone(): 0) {
        }
        
        ~Any() {
            if(mContent)
                delete mContent;
        }
        
        Any& swap(Any& rhs) {
            std::swap(mContent, rhs.mContent);
            return *this;
        }
        
        template<typename ValueType>
        Any& operator=(const ValueType& rhs) {
            Any(rhs).swap(*this);
            return *this;
        }
        
        bool operator()() const {
            return mContent != 0;
        }
        
        bool empty() const {
            return !mContent;
        }
        
        const std::type_info& type() const {
            return mContent ? mContent->type(): typeid(void);
        }
        
    private:
        class Placeholder {
        public:
            virtual ~Placeholder() {
                
            }
            
            virtual const std::type_info& type() const = 0;
            virtual Placeholder* clone() const = 0;
        };
        
        template<typename ValueType>
        class Holder: public Placeholder {
        public:
            Holder(const ValueType& value):
            mHeld(value) {
                
            }
            
            virtual const std::type_info& type() const {
                return typeid(ValueType);
            }
            
            virtual Placeholder* clone() const {
                return new Holder(mHeld);
            }
            
            ValueType mHeld;
        };
        
    private:
        template<typename ValueType>
        friend ValueType* AnyCast(Any*);
        
        template<typename ValueType>
        friend ValueType* UnsafeAnyCast(Any*);
        
        Placeholder* mContent;
    };
    
    template<typename ValueType>
    inline ValueType* AnyCast(Any* operand) {
        return operand && operand->type() == typeid(ValueType) ?
        &static_cast<Any::Holder<ValueType>*>(operand->mContent)->mHeld
        : 0;
    }
    
    template<typename ValueType>
    inline const ValueType* AnyCast(const Any* operand) {
        return AnyCast<ValueType>(const_cast<Any*>(operand));
    }
    
    template<typename ValueType>
    inline ValueType AnyCast(const Any& operand) {
        ValueType* result = AnyCast<ValueType>(const_cast<Any*>(&operand));
        if(!result)
            UKN_THROW_EXCEPTION("ukn::AnyCast: Faild to conver between const any types");
        return *result;
    }
    
    template<typename ValueType>
    inline ValueType AnyCast(Any& operand) {
        ValueType* result = AnyCast<ValueType>(&operand);
        if(!result)
            UKN_THROW_EXCEPTION("ukn::AnyCast: Faild to conver between const any types");
        return *result;
    }
    
    template<typename ValueType>
    inline const ValueType& RefAnyCast(const Any& operand) {
        ValueType* result = AnyCast<ValueType>(const_cast<Any*>(&operand));
        if(!result)
            UKN_THROW_EXCEPTION("ukn::RefAnyCast: Faild to conver between const any types");
        return *result;
    }
    
    template<typename ValueType>
    inline ValueType* UnsafeAnyCast(Any* operand) {
        return &static_cast<Any::Holder<ValueType>*>(operand->mContent)->mHeld;
    }
    
    template<typename ValueType>
    inline const ValueType* UnsafeAnyCast(const Any* operand) {
        return AnyCast<ValueType>(const_cast<Any*>(operand));
    }

    template<typename T>
    inline bool IsAnyType(const Any& any) {
        return any.type() == typeid(T);
    }
    
    template<typename T>
    inline const char* GetTypeName() {
        return typeid(T).name();
    }
} // namespace ukn

#endif
