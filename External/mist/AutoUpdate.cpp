//
//  AutoUpdate.cpp
//  Project Unknown
//
//  Created by Ruiwei Bu on 12/23/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#include "mist/AutoUpdate.h"
#include "mist/Module.h"

namespace mist {
    
    using namespace mist;
    
    class AutoUpdateModule: public mist::Module {
    public:
        AutoUpdateModule() {
            // register module
            ModuleManager::Instance().addModule(this);
        }
        
        const MistString& getName() const {
            static MistString name(L"AutoUpdate");
            return name;
        }
        
        bool init() {
            return true;
        }
        
        void update() {
            MIST_FOR_EACH(AutoUpdate* item, AutoListElement<AutoUpdate>::GetList()) {
                if(item->isEnabled())
                    item->onUpdate();
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
