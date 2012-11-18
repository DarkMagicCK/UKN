//
//  Module.cpp
//  Mist
//
//  Created by Ruiwei Bu on 12/23/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#include "mist/Module.h"
#include "mist/Preprocessor.h"
#include "mist/DllLoader.h"

namespace mist {
    
    ModuleManager::ModuleManager() {
        
    }
    
    ModuleManager::~ModuleManager() {
        ModuleList::const_iterator it = mModules.begin();
        while(it != mModules.end()) {
            (*it)->shutdown();
            ++it;
        }
    }
    
    bool ModuleManager::addModule(Module* module) {
        if(module->init()) {
            mModules.push_back(module);
            return true;
        }
        return false;
    }
    
    bool ModuleManager::delModule(Module* module) {
        ModuleList::iterator it = mModules.begin();
        for(; it != mModules.end(); ++it) {
            if(*it == module) {
                (*it)->shutdown();
                
                mModules.erase(it);
                return true;
            }
        }
        return false;
    }
    
    bool ModuleManager::addModuleFromDll(const MistString& name) {
        return false;
    }
    
    bool ModuleManager::delModule(const MistString& name) {
        ModuleList::iterator it = mModules.begin();
        for(; it != mModules.end(); ++it) {
            if((*it)->getName() == name) {
                (*it)->shutdown();
                
                mModules.erase(it);
                return true;
            }
        }
        return false;
    }
    
    void ModuleManager::onUpdate(void*, NullEventArgs&) {
        ModuleList::const_iterator it = mModules.begin();
        while(it != mModules.end()) {
            (*it)->update();
            ++it;
        }
    }
    
    namespace {
        static ModuleManager* _module_manager;
    }
    
    ModuleManager& ModuleManager::Instance() {
        if(!_module_manager) {
            _module_manager = new ModuleManager();
        }
        return *_module_manager;
    }
    
    void ModuleManager::Destroy() {
        if(_module_manager) {
            delete _module_manager;
            _module_manager = 0;
        }
    }
    
    

     
} // namespace mist
