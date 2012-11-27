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
    
    template<typename _EVENT_ARGS, typename _SENDER = void*, typename _RT = void>
    class MIST_API Event: public EventBase {
    public:
        typedef _EVENT_ARGS event_args_type;
        typedef _SENDER sender_type;
        typedef Signal<_RT(_SENDER /* sender */, event_args_type&)> signal_type;
        
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
        
        _RT raise(sender_type sender, event_args_type& args) {
            return mSignal(sender, args);
        }
        
    private:
        signal_type mSignal;
    };
    
    template<typename _SENDER, typename _RT>
    class MIST_API Event<NullEventArgs, _SENDER, _RT>: public EventBase {
    public:
        typedef NullEventArgs event_args_type;
        typedef _SENDER sender_type;
        typedef Signal<_RT(_SENDER /* sender */)> signal_type;
        
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
        
        _RT raise(sender_type sender, event_args_type& args) {
            return mSignal(sender);
        }
        
    private:
        signal_type mSignal;
    };
    
} // namespace mist

#endif
