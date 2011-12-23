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
        virtual const ukn_string& getName() const = 0;
        
        virtual void init() = 0;
        
        virtual void update() = 0;
        
        virtual void shutdown() = 0;
    };
        
    class ModuleManager: public AutoUpdate  {
    public:
        static void AddModule(Module* module);
        static void DelModule(Module* module);
        
        static void OnUpdate(void*, NullEventArgs*);
        
    private:
        typedef std::vector<Module*> ModuleList;
        static ModuleList mModules;
    };
    
} // namespace ukn


#endif
