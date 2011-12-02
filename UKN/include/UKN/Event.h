//
//  Event.h
//  Project Unknown
//
//  Created by Robert Bu on 11/21/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_Event_h
#define Project_Unknown_Event_h

#include "Platform.h"
#include "Any.h"

namespace ukn {
    
    class EventHandler;
    
    class Event {
    public:
        Event(const ukn_string& name):
        mName(name);
        
        
        
    private:
        ukn_string mName;
        
        bool mConsumed;
        
        EventHandler* mSender;
        EventHandler* mReceiver;
    };
    
} // namespace ukn

#endif
