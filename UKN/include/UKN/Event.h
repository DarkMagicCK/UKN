//
//  Event.h
//  Project Unknown
//
//  Created by Robert Bu on 11/21/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_Event_h
#define Project_Unknown_Event_h

#include "UKN/Platform.h"
#include "UKN/Class.h"
#include "UKN/Any.h"

#include "UKN/Signal.h"
#include <vector>

namespace ukn {
    
    class EventHandler;
    
    class UKN_API Event {
    public:
        Event(const ukn_string& name):
        mName(name),
        mConsumed(false) {
            
        }
        
        UKN_DEF_CLASS(Event);
        
    private:
        ukn_string mName;
        
        bool mConsumed;
    };
    
    /**
     * Event object class that overrides operator +=
     * Similar to EventHandler in c#
     * Would disconnect every connection when destructs
     **/
    template<typename SIG>
    class UKN_API EventObject {
    public:
        typedef Signal<SIG> SignalType;
        
        EventObject() { }
        EventObject(const EventObject& rhs) {
            
        }
        
        ~EventObject() {
            std::vector<Connection>::iterator it = mConnectedEvent.begin();
            while(it != mConnectedEvent.end()) {
                it->disconnect();
                ++it;
            }
        }
        
        template<typename F>
        Connection connect(const F& func) {
            Connection conn = mEvent.connect(func);
            mConnectedEvent.push_back(conn);
            return conn;
        }
        
        template<typename F>
        EventObject& operator += (const F& func) {
            Connection conn = mEvent.connect(func);
            mConnectedEvent.push_back(conn);
            return *this;
        }
        
        template<typename F>
        Connection operator + (const F& func) {
            return connect(func);
        }
        
        const SignalType& getEvent() const {
            return mEvent;
        }
        
        EventObject& operator=(const EventObject& rhs) {
            return *this;
        }
        
    private:
        SignalType mEvent;
        
        std::vector<Connection> mConnectedEvent;
    };
    
} // namespace ukn

#endif
