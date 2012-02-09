//
//  Module.cpp
//  Project Unknown
//
//  Created by Ruiwei Bu on 12/23/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#include "UKN/Module.h"
#include "UKN/Preprocessor.h"
#include "UKN/Window.h"

namespace ukn {
    
    ModuleManager::ModuleManager() {
        
    }
    
    ModuleManager::~ModuleManager() {
        ModuleList::const_iterator it = mModules.begin();
        while(it != mModules.end()) {
            (*it)->shutdown();
            ++it;
        }
    }
    
    void ModuleManager::addModule(Module* module) {
        mModules.push_back(module);
        
        module->init();
    }
    
    void ModuleManager::delModule(Module* module) {
        ModuleList::iterator it = mModules.begin();
        for(; it != mModules.end(); ++it) {
            if(*it == module) {
                (*it)->shutdown();
                
                mModules.erase(it);
                break;
            }
        }
    }
    
    void ModuleManager::onUpdate(void*, NullEventArgs&) {
        ModuleList::const_iterator it = mModules.begin();
        while(it != mModules.end()) {
            (*it)->update();
            ++it;
        }
    }
    
    ModuleManager& ModuleManager::Instance() {
        static ModuleManager instance;
        return instance;
    }
    
    UKN_STATIC_RUN_CODE(Window::OnGlobalUpdate() += Bind(&ModuleManager::Instance(), &ModuleManager::onUpdate));
    

     
} // namespace ukn
