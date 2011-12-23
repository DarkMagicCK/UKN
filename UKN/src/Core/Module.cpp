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
    
    ModuleManager::ModuleList ModuleManager::mModules;
    
    void ModuleManager::AddModule(Module* module) {
        mModules.push_back(module);
    }
    
    void ModuleManager::DelModule(Module* module) {
        ModuleList::iterator it = mModules.begin();
        for(; it != mModules.end(); ++it) {
            if(*it == module) {
                (*it)->shutdown();
                
                mModules.erase(it);
                break;
            }
        }
    }
    
    void ModuleManager::OnUpdate(void*, NullEventArgs*) {
        ModuleList::const_iterator it = mModules.begin();
        while(it != mModules.end()) {
            (*it)->update();
            ++it;
        }
    }
    
    UKN_STATIC_RUN_CODE(Window::onGlobalUpdate() += &ModuleManager::OnUpdate);
    

     
} // namespace ukn
