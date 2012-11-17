//
//  Factory.h
//  Mist
//
//  Created by Robert Bu on 11/22/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef MISTFactory_h
#define MISTFactory_h

#include "mist/Common.h"
#include "mist/Exception.h"
#include "mist/Logger.h"

#include <map>

namespace mist {
    
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
        typedef std::map<MistString, ClassInstantiator<Base>* > instantiator_map;
        typedef typename instantiator_map::iterator iterator;
        typedef typename instantiator_map::const_iterator const_iterator;
        
        Factory() { }
        
        ~Factory() {
            for_each(this->mIntantiators.begin(), 
                     this->mInstantiators.end(), 
                     DeleteSTLPtr());
        }
        
        template<typename C>
        void registerClass(const MistString& name, const ClassInstantiator<Base>* instantiator) {
            if(mInstantiators.find(name) == mInstantiators.end())
                mInstantiators.insert(std::make_pair(name, instantiator));
            else
                log_error(format_string("mist::Factory::registerClass: class with name %s alreay registered", name.c_str()));
        }
        
        template<typename C>
        void registerClassCtor(const MistString& name) {
            if(mInstantiators.find(name) == mInstantiators.end())
                mInstantiators.insert(std::make_pair(name, new DefaultClassInstantiator<Base, C>()));
            else
                log_error(format_string("mist::Factory::registerClass: class with name %s alreay registered", name.c_str()));
        }
        
        Base* createClass(const MistString& name) {
            iterator it;
            if((it = mInstantiators.find(name)) != mInstantiators.end()) {
                return it->second->instantiate();
            }
            return 0;
        }
        
    private:
        instantiator_map mInstantiators;
    };
    
} // namespace mist

#endif
