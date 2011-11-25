//
//  Factory.h
//  Project Unknown
//
//  Created by Ruiwei Bu on 11/22/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_Factory_h
#define Project_Unknown_Factory_h

#include "Common.h"
#include "Exception.h"

#include <map>

namespace ukn {
    
    template<typename T>
    class ClassInstantiator {
    public:
        virtual ~ClassInstantiator() { }
        
        virtual T* instantiate() const = 0;
    };
    
    template<typename Base, typename Derived>
    class DefaultClassInstantiator: public ClassInstantiator<Base> {
    public:  
        virtual ~DefaultClassInstantiator() { }
        
        virtual Base* instantiate() const {
            return new Derived();
        }
    };
    
    template<typename Base>
    class Factory {
    public:
        typedef std::map<ukn_string, ClassInstantiator<Base>* > instantiator_map;
        typedef typename instantiator_map::iterator iterator;
        typedef typename instantiator_map::const_iterator const_iterator;
        
        Factory() { }
        
        ~Factory() {
            for_each(this->mIntantiators.begin(), 
                     this->mInstantiators.end(), 
                     DeleteSTLPtr());
        }
        
        template<typename C>
        void registerClass(const ukn_string& name, const ClassInstantiator<Base>* instantiator) {
            if(mInstantiators.find(name) == mInstantiators.end())
                mInstantiators.insert(std::make_pair(name, instantiator));
            else
                UKN_THROW_EXCEPTION(format_string("Factory::registerClass: class with name %s alreay registered", name.c_str()));
        }
        
        template<typename C>
        void registerClassCtor(const ukn_string& name) {
            if(mInstantiators.find(name) == mInstantiators.end())
                mInstantiators.insert(std::make_pair(name, new DefaultClassInstantiator<Base, C>()));
            else
                UKN_THROW_EXCEPTION(format_string("Factory::registerClass: class with name %s alreay registered", name.c_str()));
        }
        
        Base* createClass(const ukn_string& name) {
            iterator it;
            if((it = mInstantiators.find(name)) != mInstantiators.end()) {
                return it->second->instantiate();
            }
            return 0;
        }
        
    private:
        instantiator_map mInstantiators;
    };
    
} // namespace ukn

#endif
