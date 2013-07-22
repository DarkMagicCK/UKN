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
#include "mist/StringUtil.h"

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
        ModuleList::iterator it = std::find(mModules.begin(),
                                            mModules.end(),
                                            module);
        if(it != mModules.end()) {
            this->delModule(it);
            return true;
        }
        return false;
    }
    
    bool ModuleManager::addModuleFromDll(const MistString& name) {
        DllLoader* loader = new DllLoader();
        if(loader->open(string::WStringToString(get_lib_name(name)).c_str())) {
            typedef Module* (*ModuleCreateFunc)();
            ModuleCreateFunc func = (ModuleCreateFunc)loader->getProc("CreateModule");
            
            Module* module = func();
            if(module) {
                this->addModule(module);
                
                mModuleLoaders.insert(std::make_pair(module, loader));
            } else {
                delete loader;
            }
        } else {
            delete loader;
        }
        return false;
    }
    
    bool ModuleManager::delModule(const MistString& name) {
#ifdef MIST_CPP11
        ModuleList::iterator it = std::find_if(mModules.begin(),
                                               mModules.end(),
                                               [&](Module* module)->bool { return module->getName() == name; });
        if(it != mModules.end()) {
            this->delModule(it);
            return true;
        }
        return false;
#else
        ModuleList::iterator it = mModules.begin();
        for(; it != mModules.end(); ++it) {
            if((*it)->getName() == name) {
                return true;
            }
        }
        return false;
#endif
    }
    
    void ModuleManager::delModule(ModuleList::iterator it) {
        (*it)->shutdown();
        
        ModuleLoaderMap::iterator itLoader = mModuleLoaders.find((*it));
        if(itLoader != mModuleLoaders.end()) {
            delete itLoader->second;
            mModuleLoaders.erase(itLoader);
        }
        
        mModules.erase(it);
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
