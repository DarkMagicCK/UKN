//
//  Module.h
//  Project Unknown
//
//  Created by Ruiwei Bu on 12/22/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_Module_h
#define Project_Unknown_Module_h

#include "UKN/Platform.h"
#include "UKN/Uncopyable.h"
#include "UKN/AutoUpdate.h"
#include "UKN/Event.h"

#include <vector>

namespace ukn {
    
    class Module: Uncopyable {
    public:
        virtual const String& getName() const = 0;
        
        virtual void init() = 0;
        
        virtual void update() = 0;
        
        virtual void shutdown() = 0;
    };
        
    class ModuleManager: Uncopyable {
    protected:
        ModuleManager();
        ~ModuleManager();
        
    public:
        static ModuleManager& Instance();
        
        void addModule(Module* module);
        void delModule(Module* module);
        
        void onUpdate(void*, NullEventArgs&);
        
    private:
        typedef std::vector<Module*> ModuleList;
        ModuleList mModules;
    };
    
} // namespace ukn


#endif
