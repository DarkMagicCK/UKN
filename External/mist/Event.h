//
//  Event.h
//  Mist
//
//  Created by Robert Bu on 11/21/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef MISTEvent_h
#define MISTEvent_h

#include "mist/Platform.h"
#include "mist/Any.h"
#include "mist/PreDeclare.h"
#include "mist/Signal.h"
#include <vector>

namespace mist {
  
    class EventBase {
    public:
        virtual ~EventBase() { };
    };
    
    static struct NullEventArgs {
        
    } _NullEventArgs;
    
    template<typename EventArgs>
    class MIST_API Event: public EventBase {
    public:
        typedef EventArgs event_args_type;
        typedef Signal<void(void* /* sender */, EventArgs&)> signal_type;
        
        Event() { }
        
        ~Event() { }
        
        template<typename F>
        Connection connect(const F& func) {
            return mSignal.connect(func);
        }
        
        template<typename F>
        void operator += (const F& func) {
            mSignal.connect(func);
        }
        
        template<typename F>
        void operator -= (const F& func) {
            mSignal.disconnect(func);
        }
        
        const signal_type& getSignal() const {
            return mSignal;
        }
        
        void raise(void* sender, EventArgs& args) {
            mSignal(sender, args);
        }
        
    private:
        signal_type mSignal;
    };
    
} // namespace mist

#endif
