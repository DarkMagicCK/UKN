//
//  Event.cpp
//  Project Unknown
//
//  Created by Robert Bu on 12/18/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#include "UKN/Event.h"
#include "UKN/Singleton.h"

namespace ukn {
    
    EventWorld* EventHandler::getEventWorld() const {
        if(mEventWorld)
            return mEventWorld.get();
        return 0;
    }
    
    Event::Event(const ukn_string& name):
    mName(name),
    mConsumed(false) {
        
    }
    
    Event::~Event() {
        
    }
    
    bool Event::isConsumed() const { 
        return mConsumed; 
    }
    
    void Event::comsume() {
        mConsumed = true;
    }
    
    void Event::setData(Any t) { 
        mData = t;
    }
    
    Any Event::getData() const {
        return mData;
    }
    
    Event* Event::clone() {
        Event* ev = new Event(this->mName);
        ev->mData = this->mData;
        
        return ev;
    }
    
    namespace {
        static SingletonHolder<EventWorld, void> instance;
    }
    
    EventWorld::EventWorld() {
        
    }
    
    EventWorld::~EventWorld() {
        
    }
    
    EventWorld& EventWorld::DefaultObject() {
        return *instance.get();
    }
    
    void EventWorld::enterHandler(EventHandler* handler) {
        mEventHandlers.push_back(handler);
    }
    
    void EventWorld::leaveHandler(EventHandler* handler) {
        EventHandlerList::iterator it = mEventHandlers.begin();
        while(it != mEventHandlers.end()) {
            if(*it == handler) {
                mEventHandlers.erase(it);
                break;
            }
            ++it;
        }
    }

    
} // namespace ukn
