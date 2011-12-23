//
//  AutoUpdate.cpp
//  Project Unknown
//
//  Created by Ruiwei Bu on 12/23/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#include "UKN/AutoUpdate.h"
#include "UKN/Module.h"

namespace ukn {
    
    class AutoUpdateModule: public Module {
    public:
        AutoUpdateModule() {
            // register module
            ModuleManager::AddModule(this);
        }
        
        const String& getName() const {
            static String name("AutoUpdate");
            return name;
        }
        
        void init() {
            
        }
        
        void update() {
            AutoListElement<AutoUpdate>::ListType::const_iterator it = AutoListElement<AutoUpdate>::GetList().begin();
            while(it != AutoListElement<AutoUpdate>::GetList().end()) {
                (*it)->onUpdate();
                
                ++it;
            }
        }
        
        void shutdown() {
            
        }
    };
    
    static AutoUpdateModule g_autoupdate_module;
    
    AutoUpdate::AutoUpdate():
    mEnabled(true) {
        
    }
    
    AutoUpdate::~AutoUpdate() {
        
    }
    
    void AutoUpdate::enable() {
        mEnabled = true;
    }
    
    void AutoUpdate::disable() {
        mEnabled = false;
    }
    
    bool AutoUpdate::isEnabled() const {
        return mEnabled;
    }
    
    
} // namespace ukn
