//
//  Signal.h
//  Sora
//
//  Created by Robert Bu on 8/2/11.
//  Copyright 2011 Robert Bu(Project Hoshizora). All rights reserved.
//

#ifndef UKN_Signal_h
#define UKN_Signal_h

#include "Platform.h"
#include "Any.h"
#include "SharedPtr.h"
#include "Function.h"

#include <map>

namespace ukn {
    
    /**
     * A Simple Signal Implementatiob
     **/
    
    class Connection;
    
    namespace detail {
        
        class SignalImpl {
        public:                        
            typedef std::multimap<int, Connection> ConnectionMap;
            typedef std::multimap<int, Connection>::iterator IteratorType;
            ConnectionMap mConnections;
            
            // set controlling flag to false to avoid repeat destruction in disconnect
            virtual ~SignalImpl() {
                clearConnection();
            }
            
            void clearConnection();
            
            void disconnect(void* sig, void* data) {                
                std::auto_ptr<IteratorType> slot(reinterpret_cast<IteratorType*>(data));
                mConnections.erase(*slot);
            }
        };
        
        template<typename SIG>
        class Slot {
        public:
            bool isValid() {
                return fn;
            }
            
            typedef ukn::Function<SIG> FuncType;
            FuncType fn;
        };
        
        class ConnectionBase {
        public:
            virtual bool isValid() = 0;
            virtual void disconnect() = 0;
            
            SignalImpl* signal;
            SignalImpl::IteratorType* iter;
            
            typedef void (SignalImpl::*DisconnectFunc)(void*, void*);
            DisconnectFunc signal_disconnet;
        };
        
        template<typename SIG>
        class BasicConnection: public ConnectionBase {
        public:
            typedef typename Slot<SIG>::FuncType FuncType;
            
            ukn::SharedPtr<Slot<SIG> > slot;
            
            bool isValid() {
                return slot.get() && signal_disconnet && signal;
            }
            
            void disconnect() {
                if(isValid()) {
                    (signal->*signal_disconnet)(signal, iter);
                    signal = NULL;
                    signal_disconnet = NULL;
                }
            }
            
            bool operator ==(const BasicConnection &rhs) {
                return (slot.get()->fn == rhs.slot.get()->fn && signal == rhs.signal);
            }
        };
        
    } // namespace detail
    
    class Connection {
    public:
        friend class detail::SignalImpl;
        
        Connection():
        isControlling(false) {
            
        }
        
        ~Connection() {
            if(isControlling) {
                disconnect();
            }
        }
        
        bool isValid() {
            if(con.get())
                return con->isValid();
            return false;
        }
        
        void disconnect() {
            if(con.get())
                con->disconnect();
        }
        
        bool operator ==(const Connection &rhs) {
            ukn_assert(con.get());
            return con.get() == rhs.con.get();
        }
        
        detail::ConnectionBase* get_con_base() {
            return con.get();
        }
        
        Connection& operator=(const Connection& rhs) {
            if(this != &rhs) {
                isControlling = false;
                con = rhs.con;
            }
            return *this;
        }
            
    private:
        
        void setControl(bool flag) {
            isControlling = flag;
        }
        
        void reset(detail::ConnectionBase* _con) {
            con.reset(_con);
        }
        
        bool isControlling;
        SharedPtr<detail::ConnectionBase> con;
    };
    
    namespace detail {
        
        inline void SignalImpl::clearConnection() {
            IteratorType it = mConnections.begin();
            while(it != mConnections.end()) {
                it->second.setControl(false);
                ++it;
            }
            mConnections.clear();
        }
        
    } // namespace detail
    
    template<typename SIG>
    class SignalBase: public detail::SignalImpl {
    public:
        typedef Connection ConnectionType; 
        typedef typename detail::BasicConnection<SIG>::FuncType slot_type;
        
        template<typename F>
        ConnectionType connect(const F& f) {
            return connect(0, f);
        }
        
        template<typename F>
        ConnectionType connect(int32 priority, const F& f) {
            detail::BasicConnection<SIG>* con = new detail::BasicConnection<SIG>;
            Connection slotcon;
            slotcon.reset(con);
            
            std::auto_ptr<IteratorType> saved_iter(new IteratorType());
            IteratorType iter = mConnections.insert(std::make_pair(priority, slotcon));
            *saved_iter = iter;
            
            // connection controller
            iter->second.setControl(true);
            
            con->slot.reset(new detail::Slot<SIG>);
            con->slot->fn = f;
            con->signal = this;
            con->signal_disconnet = &detail::SignalImpl::disconnect;
            con->iter = saved_iter.release();
            
            return slotcon;
        }
    };
    
    template<typename SIG>
    class Signal;
    
    // signal 1
    template<typename RT>
    class Signal<RT(void)>: public SignalBase<RT(void)> {
    public:
        RT sig() {
            typedef SignalBase<RT(void)> base_type;
            
            typename base_type::ConnectionMap::iterator itConnection = base_type::mConnections.begin();
            
            RT rtVal;
            while(itConnection != base_type::mConnections.end()) {
                detail::BasicConnection<RT()>* con = static_cast<detail::BasicConnection<RT()>* >(itConnection->second.get_con_base());
                rtVal = con->slot.get()->fn();
                ++itConnection;
            }
            return rtVal;
        }
        
        RT operator()() { sig(); }
    };
    
    // signal 1 void
    template<>
    class Signal<void(void)>: public SignalBase<void(void)> {
    public:
        void sig() {
            typedef SignalBase<void(void)> base_type;
            base_type::ConnectionMap::iterator itConnection = base_type::mConnections.begin();
            
            while(itConnection != base_type::mConnections.end()) {
                detail::BasicConnection<void()>* con = static_cast<detail::BasicConnection<void()>* >(itConnection->second.get_con_base());
                con->slot.get()->fn();
                ++itConnection;
            }
        }
        
        void operator()() { sig(); }
    };
    
    template<typename RT, typename T0>
    class Signal<RT(T0)>: public SignalBase<RT(T0)> {
    public:
        RT sig(T0 a0) {
            typedef SignalBase<RT(T0)> base_type;
            typename base_type::ConnectionMap::iterator itConnection = base_type::mConnections.begin();
            
            RT rtVal;
            while(itConnection != base_type::mConnections.end()) {
                detail::BasicConnection<RT(T0)>* con = 
                static_cast<detail::BasicConnection<RT(T0)>* >(itConnection->second.get_con_base());
                
                rtVal = con->slot.get()->fn(a0);
                ++itConnection;
            }
            return rtVal;
        }
        
        RT operator()(T0 a0) { sig(a0); }
    };
    
    template<typename T0>
    class Signal<void(T0)>: public SignalBase<void(T0)> {
    public:
        void sig(T0 a0) {
            typedef SignalBase<void(T0)> base_type;
            typename base_type::ConnectionMap::iterator itConnection = base_type::mConnections.begin();
            
            while(itConnection != base_type::mConnections.end()) {
                detail::BasicConnection<void(T0)>* con = 
                static_cast<detail::BasicConnection<void(T0)>* >(itConnection->second.get_con_base());
                
                con->slot.get()->fn(a0);
                ++itConnection;
            }
        }
        
        void operator()(T0 a0) { sig(a0); }
    };
    
    template<typename RT, typename T0, typename T1>
    class Signal<RT(T0, T1)>: public SignalBase<RT(T0, T1)> {
    public:
        RT sig(T0 a0, T1 a1) {
            typedef SignalBase<RT(T0, T1)> base_type;
            typename base_type::ConnectionMap::iterator itConnection = base_type::mConnections.begin();
            
            RT rtVal;
            while(itConnection != base_type::mConnections.end()) {
                detail::BasicConnection<RT(T0, T1)>* con = 
                static_cast<detail::BasicConnection<RT(T0, T1)>* >(itConnection->second.get_con_base());
                
                rtVal = con->slot.get()->fn(a0, a1);
                ++itConnection;
            }
            return rtVal;
        }
        
        RT operator()(T0 a0, T1 a1) { sig(a0, a1); }
    };
    
    template<typename T0, typename T1>
    class Signal<void(T0, T1)>: public SignalBase<void(T0, T1)> {
    public:
        void sig(T0 a0, T1 a1) {
            typedef SignalBase<void(T0, T1)> base_type;
            typename base_type::ConnectionMap::iterator itConnection = base_type::mConnections.begin();
            
            while(itConnection != base_type::mConnections.end()) {
                detail::BasicConnection<void(T0, T1)>* con = 
                static_cast<detail::BasicConnection<void(T0, T1)>* >(itConnection->second.get_con_base());
                
                con->slot.get()->fn(a0, a1);
                ++itConnection;
            }
        }
        
        void operator()(T0 a0, T1 a1) { sig(a0, a1); }
    };
    
    template<typename RT, typename T0, typename T1, typename T2>
    class Signal<RT(T0, T1, T2)>: public SignalBase<RT(T0, T1, T2)> {
    public:
        RT sig(T0 a0, T1 a1, T2 a2) {
            typedef SignalBase<RT(T0, T1, T2)> base_type;
            typename base_type::ConnectionMap::iterator itConnection = base_type::mConnections.begin();
            
            RT rtVal;
            while(itConnection != base_type::mConnections.end()) {
                detail::BasicConnection<RT(T0, T1, T2)>* con = 
                static_cast<detail::BasicConnection<RT(T0, T1, T2)>* >(itConnection->second.get_con_base());
                
                rtVal = con->slot.get()->fn(a0, a1, a2);
                ++itConnection;
            }
            return rtVal;
        }
        
        RT operator()(T0 a0, T1 a1, T2 a2) { sig(a0, a1, a2); }
    };
    
    template<typename T0, typename T1, typename T2>
    class Signal<void(T0, T1, T2)>: public SignalBase<void(T0, T1, T2)> {
    public:
        void sig(T0 a0, T1 a1, T2 a2) {
            typedef SignalBase<void(T0, T1, T2)> base_type;
            typename base_type::ConnectionMap::iterator itConnection = base_type::mConnections.begin();
            
            while(itConnection != base_type::mConnections.end()) {
                detail::BasicConnection<void(T0, T1, T2)>* con = 
                static_cast<detail::BasicConnection<void(T0, T1, T2)>* >(itConnection->second.get_con_base());
                
                con->slot.get()->fn(a0, a1, a2);
                ++itConnection;
            }
        }
        
        void operator()(T0 a0, T1 a1, T2 a2) { sig(a0, a1, a2); }
    };
    
    template<typename RT, typename T0, typename T1, typename T2, typename T3>
    class Signal<RT(T0, T1, T2, T3)>: public SignalBase<RT(T0, T1, T2, T3)> {
    public:
        RT sig(T0 a0, T1 a1, T2 a2, T3 a3) {
            typedef SignalBase<RT(T0, T1, T2, T3)> base_type;
            typename base_type::ConnectionMap::iterator itConnection = base_type::mConnections.begin();
            
            RT rtVal;
            while(itConnection != base_type::mConnections.end()) {
                detail::BasicConnection<RT(T0, T1, T2, T3)>* con = 
                static_cast<detail::BasicConnection<RT(T0, T1, T2, T3)>* >(itConnection->second.get_con_base());
                
                rtVal = con->slot.get()->fn(a0, a1, a2, a3);
                ++itConnection;
            }
            return rtVal;
        }
        
        RT operator()(T0 a0, T1 a1, T2 a2, T3 a3) { sig(a0, a1, a2, a3); }
    };
    
    template<typename T0, typename T1, typename T2, typename T3>
    class Signal<void(T0, T1, T2, T3)>: public SignalBase<void(T0, T1, T2, T3)> {
    public:
        void sig(T0 a0, T1 a1, T2 a2, T3 a3) {
            typedef SignalBase<void(T0, T1, T2, T3)> base_type;
            typename base_type::ConnectionMap::iterator itConnection = base_type::mConnections.begin();
            
            while(itConnection != base_type::mConnections.end()) {
                detail::BasicConnection<void(T0, T1, T2, T3)>* con = 
                static_cast<detail::BasicConnection<void(T0, T1, T2, T3)>* >(itConnection->second.get_con_base());
                
                con->slot.get()->fn(a0, a1, a2, a3);
                ++itConnection;
            }
        }
        
        void operator()(T0 a0, T1 a1, T2 a2, T3 a3) { sig(a0, a1, a2, a3); }
    };
    
    template<typename RT, typename T0, typename T1, typename T2, typename T3, typename T4>
    class Signal<RT(T0, T1, T2, T3, T4)>: public SignalBase<RT(T0, T1, T2, T3, T4)> {
    public:
        RT sig(T0 a0, T1 a1, T2 a2, T3 a3, T4 a4) {
            typedef SignalBase<RT(T0, T1, T2, T3, T4)> base_type;
            typename base_type::ConnectionMap::iterator itConnection = base_type::mConnections.begin();
            
            RT rtVal;
            while(itConnection != base_type::mConnections.end()) {
                detail::BasicConnection<RT(T0, T1, T2, T3, T4)>* con = 
                static_cast<detail::BasicConnection<RT(T0, T1, T2, T3, T4)>* >(itConnection->second.get_con_base());
                
                rtVal = con->slot.get()->fn(a0, a1, a2, a3, a4);
                ++itConnection;
            }
            return rtVal;
        }
        
        RT operator()(T0 a0, T1 a1, T2 a2, T3 a3, T4 a4) { sig(a0, a1, a2, a3, a4); }
    };
    
    template<typename T0, typename T1, typename T2, typename T3, typename T4>
    class Signal<void(T0, T1, T2, T3, T4)>: public SignalBase<void(T0, T1, T2, T3, T4)> {
    public:
        void sig(T0 a0, T1 a1, T2 a2, T3 a3, T4 a4) {
            typedef SignalBase<void(T0, T1, T2, T3, T4)> base_type;
            typename base_type::ConnectionMap::iterator itConnection = base_type::mConnections.begin();
            
            while(itConnection != base_type::mConnections.end()) {
                detail::BasicConnection<void(T0, T1, T2, T3, T4)>* con = 
                static_cast<detail::BasicConnection<void(T0, T1, T2, T3, T4)>* >(itConnection->second.get_con_base());
                
                con->slot.get()->fn(a0, a1, a2, a3, a4);
                ++itConnection;
            }
        }
        
        void operator()(T0 a0, T1 a1, T2 a2, T3 a3, T4 a4) { sig(a0, a1, a2, a3, a4); }
    };
    
    template<typename RT, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5>
    class Signal<RT(T0, T1, T2, T3, T4, T5)>: public SignalBase<RT(T0, T1, T2, T3, T4, T5)> {
    public:
        RT sig(T0 a0, T1 a1, T2 a2, T3 a3, T4 a4, T5 a5) {
            typedef SignalBase<RT(T0, T1, T2, T3, T4, T5)> base_type;
            typename base_type::ConnectionMap::iterator itConnection = base_type::mConnections.begin();
            
            RT rtVal;
            while(itConnection != base_type::mConnections.end()) {
                detail::BasicConnection<RT(T0, T1, T2, T3, T4, T5)>* con = 
                static_cast<detail::BasicConnection<RT(T0, T1, T2, T3, T4, T5)>* >(itConnection->second.get_con_base());
                
                rtVal = con->slot.get()->fn(a0, a1, a2, a3, a4, a5);
                ++itConnection;
            }
            return rtVal;
        }
        
        RT operator()(T0 a0, T1 a1, T2 a2, T3 a3, T4 a4, T5 a5) { sig(a0, a1, a2, a3, a4, a5); }
    };
    
    template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5>
    class Signal<void(T0, T1, T2, T3, T4, T5)>: public SignalBase<void(T0, T1, T2, T3, T4, T5)> {
    public:
        void sig(T0 a0, T1 a1, T2 a2, T3 a3, T4 a4, T5 a5) {
            typedef SignalBase<void(T0, T1, T2, T3, T4, T5)> base_type;
            typename base_type::ConnectionMap::iterator itConnection = base_type::mConnections.begin();
            
            while(itConnection != base_type::mConnections.end()) {
                detail::BasicConnection<void(T0, T1, T2, T3, T4, T5)>* con = 
                static_cast<detail::BasicConnection<void(T0, T1, T2, T3, T4, T5)>* >(itConnection->second.get_con_base());
                
                con->slot.get()->fn(a0, a1, a2, a3, a4, a5);
                ++itConnection;
            }
        }
        
        void operator()(T0 a0, T1 a1, T2 a2, T3 a3, T4 a4, T5 a5) { sig(a0, a1, a2, a3, a4, a5); }
    };
    
    template<typename RT, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
    class Signal<RT(T0, T1, T2, T3, T4, T5, T6)>: public SignalBase<RT(T0, T1, T2, T3, T4, T5, T6)> {
    public:
        RT sig(T0 a0, T1 a1, T2 a2, T3 a3, T4 a4, T5 a5, T6 a6) {
            typedef SignalBase<RT(T0, T1, T2, T3, T4, T5, T6)> base_type;
            typename base_type::ConnectionMap::iterator itConnection = base_type::mConnections.begin();
            
            RT rtVal;
            while(itConnection != base_type::mConnections.end()) {
                detail::BasicConnection<RT(T0, T1, T2, T3, T4, T5, T6)>* con = 
                static_cast<detail::BasicConnection<RT(T0, T1, T2, T3, T4, T5, T6)>* >(itConnection->second.get_con_base());
                
                rtVal = con->slot.get()->fn(a0, a1, a2, a3, a4, a5, a6);
                ++itConnection;
            }
            return rtVal;
        }
        
        RT operator()(T0 a0, T1 a1, T2 a2, T3 a3, T4 a4, T5 a5, T6 a6) { sig(a0, a1, a2, a3, a4, a5, a6); }
    };
    
    template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
    class Signal<void(T0, T1, T2, T3, T4, T5, T6)>: public SignalBase<void(T0, T1, T2, T3, T4, T5, T6)> {
    public:
        void sig(T0 a0, T1 a1, T2 a2, T3 a3, T4 a4, T5 a5, T6 a6) {
            typedef SignalBase<void(T0, T1, T2, T3, T4, T5, T6)> base_type;
            typename base_type::ConnectionMap::iterator itConnection = base_type::mConnections.begin();
            
            while(itConnection != base_type::mConnections.end()) {
                detail::BasicConnection<void(T0, T1, T2, T3, T4, T5, T6)>* con = 
                static_cast<detail::BasicConnection<void(T0, T1, T2, T3, T4, T5, T6)>* >(itConnection->second.get_con_base());
                
                con->slot.get()->fn(a0, a1, a2, a3, a4, a5, a6);
                ++itConnection;
            }
        }
        
        void operator()(T0 a0, T1 a1, T2 a2, T3 a3, T4 a4, T5 a5, T6 a6) { sig(a0, a1, a2, a3, a4, a5, a6); }
    };
    
    template<typename RT, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
    class Signal<RT(T0, T1, T2, T3, T4, T5, T6, T7)>: public SignalBase<RT(T0, T1, T2, T3, T4, T5, T6, T7)> {
    public:
        RT sig(T0 a0, T1 a1, T2 a2, T3 a3, T4 a4, T5 a5, T6 a6, T7 a7) {
            typedef SignalBase<RT(T0, T1, T2, T3, T4, T5, T6, T7 a7)> base_type;
            typename base_type::ConnectionMap::iterator itConnection = base_type::mConnections.begin();
            
            RT rtVal;
            while(itConnection != base_type::mConnections.end()) {
                detail::BasicConnection<RT(T0, T1, T2, T3, T4, T5, T6, T7)>* con = 
                static_cast<detail::BasicConnection<RT(T0, T1, T2, T3, T4, T5, T6, T7)>* >(itConnection->second.get_con_base());
                
                rtVal = con->slot.get()->fn(a0, a1, a2, a3, a4, a5, a6, a7);
                ++itConnection;
            }
            return rtVal;
        }
        
        RT operator()(T0 a0, T1 a1, T2 a2, T3 a3, T4 a4, T5 a5, T6 a6, T7 a7) { sig(a0, a1, a2, a3, a4, a5, a6, a7); }
    };
    
    template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
    class Signal<void(T0, T1, T2, T3, T4, T5, T6, T7)>: public SignalBase<void(T0, T1, T2, T3, T4, T5, T6, T7)> {
    public:
        void sig(T0 a0, T1 a1, T2 a2, T3 a3, T4 a4, T5 a5, T6 a6, T7 a7) {
            typedef SignalBase<void(T0, T1, T2, T3, T4, T5, T6, T7)> base_type;
            typename base_type::ConnectionMap::iterator itConnection = base_type::mConnections.begin();
            
            while(itConnection != base_type::mConnections.end()) {
                detail::BasicConnection<void(T0, T1, T2, T3, T4, T5, T6, T7)>* con = 
                static_cast<detail::BasicConnection<void(T0, T1, T2, T3, T4, T5, T6, T7)>* >(itConnection->second.get_con_base());
                
                con->slot.get()->fn(a0, a1, a2, a3, a4, a5, a6, a7);
                ++itConnection;
            }
        }
        
        void operator()(T0 a0, T1 a1, T2 a2, T3 a3, T4 a4, T5 a5, T6 a6, T7 a7) { sig(a0, a1, a2, a3, a4, a5, a6, a7); }
    };
    
    template<typename RT, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
    class Signal<RT(T0, T1, T2, T3, T4, T5, T6, T7, T8)>: public SignalBase<RT(T0, T1, T2, T3, T4, T5, T6, T7, T8)> {
    public:
        RT sig(T0 a0, T1 a1, T2 a2, T3 a3, T4 a4, T5 a5, T6 a6, T7 a7, T8 a8) {
            typedef SignalBase<RT(T0, T1, T2, T3, T4, T5, T6, T7 a7, T8 a8)> base_type;
            typename base_type::ConnectionMap::iterator itConnection = base_type::mConnections.begin();
            
            RT rtVal;
            while(itConnection != base_type::mConnections.end()) {
                detail::BasicConnection<RT(T0, T1, T2, T3, T4, T5, T6, T7, T8)>* con = 
                static_cast<detail::BasicConnection<RT(T0, T1, T2, T3, T4, T5, T6, T7, T8)>* >(itConnection->second.get_con_base());
                
                rtVal = con->slot.get()->fn(a0, a1, a2, a3, a4, a5, a6, a7, a8);
                ++itConnection;
            }
            return rtVal;
        }
        
        RT operator()(T0 a0, T1 a1, T2 a2, T3 a3, T4 a4, T5 a5, T6 a6, T7 a7, T8 a8) { sig(a0, a1, a2, a3, a4, a5, a6, a7, a8); }
    };
    
    template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
    class Signal<void(T0, T1, T2, T3, T4, T5, T6, T7, T8)>: public SignalBase<void(T0, T1, T2, T3, T4, T5, T6, T7, T8)> {
    public:
        void sig(T0 a0, T1 a1, T2 a2, T3 a3, T4 a4, T5 a5, T6 a6, T7 a7, T8 a8) {
            typedef SignalBase<void(T0, T1, T2, T3, T4, T5, T6, T7, T8)> base_type;
            typename base_type::ConnectionMap::iterator itConnection = base_type::mConnections.begin();
            
            while(itConnection != base_type::mConnections.end()) {
                detail::BasicConnection<void(T0, T1, T2, T3, T4, T5, T6, T7, T8)>* con = 
                static_cast<detail::BasicConnection<void(T0, T1, T2, T3, T4, T5, T6, T7, T8)>* >(itConnection->second.get_con_base());
                
                con->slot.get()->fn(a0, a1, a2, a3, a4, a5, a6, a7, a8);
                ++itConnection;
            }
        }
        
        void operator()(T0 a0, T1 a1, T2 a2, T3 a3, T4 a4, T5 a5, T6 a6, T7 a7, T8 a8) { sig(a0, a1, a2, a3, a4, a5, a6, a7, a8); }
    };
    
} // namespace ukn


#endif
