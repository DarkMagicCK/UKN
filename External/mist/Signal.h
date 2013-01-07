//
//  Signal.h
//  Sora
//
//  Created by Robert Bu on 8/2/11.
//  Copyright 2011 Robert Bu(Project Hoshizora). All rights reserved.
//

#ifndef UKN_Signal_h
#define UKN_Signal_h

#include "mist/Platform.h"
#include "mist/Any.h"
#include "mist/Ptr.h"
#include "mist/Function.h"

#include <map>
#include <memory>
#include <utility>
#include <vector>

namespace mist {
    
#if 0
  
    template<typename T> struct Signal;
    template<typename RT, typename ...ARGS> struct Signal<RT(ARGS...)> {
        typedef typename std::decay<RT(ARGS...)>::type func_type;
        typedef Signal<RT(ARGS...)> self_type;
        
        self_type& operator+=(func_type f) {
            mSlots.push_back(f); return *this;
        }
        
        self_type& operator-=(func_type f) {
            mSlots.erase(std::remove(mSlots.begin(),
                                     mSlots.end(),
                                     f),
                         mSlots.end());
            return *this;
        }
        
        RT operator()(ARGS... args) {
            RT result;
            for(func_type f: mSlots) {
                result = f(args...);
            }
            return result;
        }
        
    private:
        typedef std::vector<func_type> SlotVector;
        SlotVector mSlots;
    };
    
    template<typename ...ARGS> struct Signal<void(ARGS...)> {
        typedef typename std::decay<void(ARGS...)>::type func_type;
        typedef Signal<void(ARGS...)> self_type;
        
        self_type& operator+=(func_type f) {
            mSlots.push_back(f); return *this;
        }
        
        self_type& operator-=(func_type f) {
            mSlots.erase(std::remove(mSlots.begin(),
                                     mSlots.end(),
                                     f),
                         mSlots.end());
            return *this;
        }
        
        void operator()(ARGS... args) {
            for(func_type f: mSlots) {
                f(args...);
            }
        }
        
    private:
        typedef std::vector<func_type> SlotVector;
        SlotVector mSlots;
    };

#else
    
    /**
     * A Simple Signal Implementation
     **/
    
    namespace detail {
        class ConnectionBase;
        class SignalImpl;
    }
    
    class Connection {
    public:
        friend class detail::SignalImpl;
        
        Connection();
        virtual ~Connection();
        
        bool isValid() const;
        void disconnect() ;
        
        bool operator ==(const Connection &rhs);
        
        detail::ConnectionBase* get_con_base() const ;
        
        Connection& operator=(const Connection& rhs);
        void setControl(bool flag) ;
        
        void reset(detail::ConnectionBase* _con) ;
        
    private:
        bool isControlling;
        SharedPtr<detail::ConnectionBase> con;
    };
    
    namespace detail {
        
        class SignalImpl {
        public:
            typedef std::multimap<int, Connection> ConnectionMap;
            typedef std::multimap<int, Connection>::iterator IteratorType;
            ConnectionMap mConnections;
            
            SignalImpl() {
                in_distribute = false;
            }
            
            // set controlling flag to false to avoid repeat destruction in disconnect
            virtual ~SignalImpl() {
                clearConnection();
            }
            
            void clearConnection();
            
            void disconnect(void* con, void* data);
            
        protected:
            typedef std::vector<void*> ToDelete;
            ToDelete __to_delete;
            bool in_distribute;
        };
        
        template<typename SIG>
        class Slot {
        public:
            bool isValid() {
                return fn;
            }
            
            typedef Function<SIG> FuncType;
            FuncType fn;
        };
        
        class ConnectionBase {
        public:
            ConnectionBase() {
                __in_delete_list = false;
            }
            virtual ~ConnectionBase() { }
            
            virtual bool isValid() = 0;
            virtual void disconnect() = 0;
            
            SignalImpl* signal;
            SignalImpl::IteratorType* iter;
            bool __in_delete_list;
            
            typedef void (SignalImpl::*DisconnectFunc)(void*, void*);
            DisconnectFunc signal_disconnet;
        };
        
        template<typename SIG>
        class BasicConnection: public ConnectionBase {
        public:
            typedef typename Slot<SIG>::FuncType FuncType;
            
            SharedPtr<Slot<SIG> > slot;
            
            bool isValid() {
                return slot.get() && signal_disconnet && signal;
            }
            
            void disconnect() {
                if(isValid()) {
                    (signal->*signal_disconnet)(this, iter);
                    signal = NULL;
                    signal_disconnet = NULL;
                }
            }
            
            bool operator ==(const BasicConnection &rhs) {
                return (slot.get()->fn == rhs.slot.get()->fn && signal == rhs.signal);
            }
        };
        
    } // namespace detail
    
    template<typename SIG>
    class SignalBase: public detail::SignalImpl {
    public:
        typedef Connection ConnectionType;
        typedef typename detail::BasicConnection<SIG>::FuncType SlotType;
        
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
        
        template<typename F>
        void disconnect(const F& f) {
            IteratorType iter = mConnections.begin();
            
            IteratorType iter_to_disconn = mConnections.end();
            while(iter != mConnections.end()) {
                detail::BasicConnection<SIG>* con = static_cast<detail::BasicConnection<SIG>* >(iter->second.get_con_base());
                if(con && con->slot->fn == f) {
                    iter_to_disconn = iter;
                    break;
                }
                
                ++iter;
            }
            
            if(iter_to_disconn != mConnections.end()) {
                detail::BasicConnection<SIG>* con = static_cast<detail::BasicConnection<SIG>* >(iter_to_disconn->second.get_con_base());
                con->disconnect();
            }
        }
        
        template<typename F>
        SignalBase& operator+=(const F& f) {
            this->connect(0, f);
            return *this;
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
            
            typename base_type::ConnectionMap::const_iterator itConnection = base_type::mConnections.begin();
            
            detail::SignalImpl::in_distribute = true;
            
            RT rtVal;
            while(itConnection != base_type::mConnections.end()) {
                detail::BasicConnection<RT()>* con = static_cast<detail::BasicConnection<RT()>* >(itConnection->second.get_con_base());
                rtVal = con->slot.get()->fn();
                ++itConnection;
            }
            
            detail::SignalImpl::in_distribute = false;
            
            return rtVal;
        }
        
        RT operator()() const { sig(); }
    };
    
    // signal 1 void
    template<>
    class Signal<void(void)>: public SignalBase<void(void)> {
    public:
        void sig() {
            typedef SignalBase<void(void)> base_type;
            base_type::ConnectionMap::const_iterator itConnection = base_type::mConnections.begin();
            
            detail::SignalImpl::in_distribute = true;

            while(itConnection != base_type::mConnections.end()) {
                detail::BasicConnection<void()>* con = static_cast<detail::BasicConnection<void()>* >(itConnection->second.get_con_base());
                con->slot.get()->fn();
                ++itConnection;
            }
            
            detail::SignalImpl::in_distribute = false;

        }
        
        void operator()() { sig(); }
    };
    
    template<typename RT, typename T0>
    class Signal<RT(T0)>: public SignalBase<RT(T0)> {
    public:
        RT sig(T0 a0) {
            typedef SignalBase<RT(T0)> base_type;
            typename base_type::ConnectionMap::const_iterator itConnection = base_type::mConnections.begin();
            
            detail::SignalImpl::in_distribute = true;

            RT rtVal;
            while(itConnection != base_type::mConnections.end()) {
                detail::BasicConnection<RT(T0)>* con =
                static_cast<detail::BasicConnection<RT(T0)>* >(itConnection->second.get_con_base());
                
                if(!con->__in_delete_list)
                    rtVal = con->slot.get()->fn(a0);
                ++itConnection;
            }
            detail::SignalImpl::in_distribute = false;

            return rtVal;
        }
        
        RT operator()(T0 a0) { sig(a0); }
    };
    
    template<typename T0>
    class Signal<void(T0)>: public SignalBase<void(T0)> {
    public:
        void sig(T0 a0) {
            typedef SignalBase<void(T0)> base_type;
            typename base_type::ConnectionMap::const_iterator itConnection = base_type::mConnections.begin();
            
            detail::SignalImpl::in_distribute = true;

            while(itConnection != base_type::mConnections.end()) {
                detail::BasicConnection<void(T0)>* con =
                static_cast<detail::BasicConnection<void(T0)>* >(itConnection->second.get_con_base());
                
                if(!con->__in_delete_list)
                    con->slot.get()->fn(a0);
                ++itConnection;
            }
            detail::SignalImpl::in_distribute = false;

        }
        
        void operator()(T0 a0) { sig(a0); }
    };
    
    template<typename RT, typename T0, typename T1>
    class Signal<RT(T0, T1)>: public SignalBase<RT(T0, T1)> {
    public:
        RT sig(T0 a0, T1 a1) {
            typedef SignalBase<RT(T0, T1)> base_type;
            typename base_type::ConnectionMap::const_iterator itConnection = base_type::mConnections.begin();
            
            detail::SignalImpl::in_distribute = true;

            RT rtVal;
            while(itConnection != base_type::mConnections.end()) {
                detail::BasicConnection<RT(T0, T1)>* con =
                static_cast<detail::BasicConnection<RT(T0, T1)>* >(itConnection->second.get_con_base());
                
                if(!con->__in_delete_list)
                    rtVal = con->slot.get()->fn(a0, a1);
                ++itConnection;
            }
            detail::SignalImpl::in_distribute = false;

            return rtVal;
        }
        
        RT operator()(T0 a0, T1 a1) { sig(a0, a1); }
    };
    
    template<typename T0, typename T1>
    class Signal<void(T0, T1)>: public SignalBase<void(T0, T1)> {
    public:
        void sig(T0 a0, T1 a1) {
            typedef SignalBase<void(T0, T1)> base_type;
            typename base_type::ConnectionMap::const_iterator itConnection = base_type::mConnections.begin();
            
            detail::SignalImpl::in_distribute = true;

            while(itConnection != base_type::mConnections.end()) {
                detail::BasicConnection<void(T0, T1)>* con =
                static_cast<detail::BasicConnection<void(T0, T1)>* >(itConnection->second.get_con_base());
                
                if(!con->__in_delete_list)
                    con->slot.get()->fn(a0, a1);
                ++itConnection;
            }
            detail::SignalImpl::in_distribute = false;

        }
        
        void operator()(T0 a0, T1 a1) { sig(a0, a1); }
    };
    
    template<typename RT, typename T0, typename T1, typename T2>
    class Signal<RT(T0, T1, T2)>: public SignalBase<RT(T0, T1, T2)> {
    public:
        RT sig(T0 a0, T1 a1, T2 a2) {
            typedef SignalBase<RT(T0, T1, T2)> base_type;
            typename base_type::ConnectionMap::const_iterator itConnection = base_type::mConnections.begin();
            
            detail::SignalImpl::in_distribute = true;

            RT rtVal;
            while(itConnection != base_type::mConnections.end()) {
                detail::BasicConnection<RT(T0, T1, T2)>* con =
                static_cast<detail::BasicConnection<RT(T0, T1, T2)>* >(itConnection->second.get_con_base());
                
                if(!con->__in_delete_list)
                    rtVal = con->slot.get()->fn(a0, a1, a2);
                ++itConnection;
            }
            detail::SignalImpl::in_distribute = false;

            return rtVal;
        }
        
        RT operator()(T0 a0, T1 a1, T2 a2) { sig(a0, a1, a2); }
    };
    
    template<typename T0, typename T1, typename T2>
    class Signal<void(T0, T1, T2)>: public SignalBase<void(T0, T1, T2)> {
    public:
        void sig(T0 a0, T1 a1, T2 a2) {
            typedef SignalBase<void(T0, T1, T2)> base_type;
            typename base_type::ConnectionMap::const_iterator itConnection = base_type::mConnections.begin();
            
            detail::SignalImpl::in_distribute = true;

            while(itConnection != base_type::mConnections.end()) {
                detail::BasicConnection<void(T0, T1, T2)>* con =
                static_cast<detail::BasicConnection<void(T0, T1, T2)>* >(itConnection->second.get_con_base());
                
                if(!con->__in_delete_list)
                    con->slot.get()->fn(a0, a1, a2);
                ++itConnection;
            }
            detail::SignalImpl::in_distribute = false;

        }
        
        void operator()(T0 a0, T1 a1, T2 a2) { sig(a0, a1, a2); }
    };
    
    template<typename RT, typename T0, typename T1, typename T2, typename T3>
    class Signal<RT(T0, T1, T2, T3)>: public SignalBase<RT(T0, T1, T2, T3)> {
    public:
        RT sig(T0 a0, T1 a1, T2 a2, T3 a3) {
            typedef SignalBase<RT(T0, T1, T2, T3)> base_type;
            typename base_type::ConnectionMap::const_iterator itConnection = base_type::mConnections.begin();
            
            detail::SignalImpl::in_distribute = true;

            RT rtVal;
            while(itConnection != base_type::mConnections.end()) {
                detail::BasicConnection<RT(T0, T1, T2, T3)>* con =
                static_cast<detail::BasicConnection<RT(T0, T1, T2, T3)>* >(itConnection->second.get_con_base());
                
                if(!con->__in_delete_list)
                    rtVal = con->slot.get()->fn(a0, a1, a2, a3);
                ++itConnection;
            }
            detail::SignalImpl::in_distribute = false;

            return rtVal;
        }
        
        RT operator()(T0 a0, T1 a1, T2 a2, T3 a3) { sig(a0, a1, a2, a3); }
    };
    
    template<typename T0, typename T1, typename T2, typename T3>
    class Signal<void(T0, T1, T2, T3)>: public SignalBase<void(T0, T1, T2, T3)> {
    public:
        void sig(T0 a0, T1 a1, T2 a2, T3 a3) {
            typedef SignalBase<void(T0, T1, T2, T3)> base_type;
            typename base_type::ConnectionMap::const_iterator itConnection = base_type::mConnections.begin();
            
            detail::SignalImpl::in_distribute = true;

            while(itConnection != base_type::mConnections.end()) {
                detail::BasicConnection<void(T0, T1, T2, T3)>* con =
                static_cast<detail::BasicConnection<void(T0, T1, T2, T3)>* >(itConnection->second.get_con_base());
                
                if(!con->__in_delete_list)
                    con->slot.get()->fn(a0, a1, a2, a3);
                ++itConnection;
            }
            detail::SignalImpl::in_distribute = false;

        }
        
        void operator()(T0 a0, T1 a1, T2 a2, T3 a3) { sig(a0, a1, a2, a3); }
    };
    
    template<typename RT, typename T0, typename T1, typename T2, typename T3, typename T4>
    class Signal<RT(T0, T1, T2, T3, T4)>: public SignalBase<RT(T0, T1, T2, T3, T4)> {
    public:
        RT sig(T0 a0, T1 a1, T2 a2, T3 a3, T4 a4) {
            typedef SignalBase<RT(T0, T1, T2, T3, T4)> base_type;
            typename base_type::ConnectionMap::const_iterator itConnection = base_type::mConnections.begin();
            
            detail::SignalImpl::in_distribute = true;

            RT rtVal;
            while(itConnection != base_type::mConnections.end()) {
                detail::BasicConnection<RT(T0, T1, T2, T3, T4)>* con =
                static_cast<detail::BasicConnection<RT(T0, T1, T2, T3, T4)>* >(itConnection->second.get_con_base());
                
                if(!con->__in_delete_list)
                    rtVal = con->slot.get()->fn(a0, a1, a2, a3, a4);
                ++itConnection;
            }
            detail::SignalImpl::in_distribute = false;

            return rtVal;
        }
        
        RT operator()(T0 a0, T1 a1, T2 a2, T3 a3, T4 a4) { sig(a0, a1, a2, a3, a4); }
    };
    
    template<typename T0, typename T1, typename T2, typename T3, typename T4>
    class Signal<void(T0, T1, T2, T3, T4)>: public SignalBase<void(T0, T1, T2, T3, T4)> {
    public:
        void sig(T0 a0, T1 a1, T2 a2, T3 a3, T4 a4) {
            typedef SignalBase<void(T0, T1, T2, T3, T4)> base_type;
            typename base_type::ConnectionMap::const_iterator itConnection = base_type::mConnections.begin();
            
            detail::SignalImpl::in_distribute = true;

            while(itConnection != base_type::mConnections.end()) {
                detail::BasicConnection<void(T0, T1, T2, T3, T4)>* con =
                static_cast<detail::BasicConnection<void(T0, T1, T2, T3, T4)>* >(itConnection->second.get_con_base());
                
                if(!con->__in_delete_list)
                    con->slot.get()->fn(a0, a1, a2, a3, a4);
                ++itConnection;
            }
            detail::SignalImpl::in_distribute = false;

        }
        
        void operator()(T0 a0, T1 a1, T2 a2, T3 a3, T4 a4) { sig(a0, a1, a2, a3, a4); }
    };
    
    template<typename RT, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5>
    class Signal<RT(T0, T1, T2, T3, T4, T5)>: public SignalBase<RT(T0, T1, T2, T3, T4, T5)> {
    public:
        RT sig(T0 a0, T1 a1, T2 a2, T3 a3, T4 a4, T5 a5) {
            typedef SignalBase<RT(T0, T1, T2, T3, T4, T5)> base_type;
            typename base_type::ConnectionMap::const_iterator itConnection = base_type::mConnections.begin();
            
            detail::SignalImpl::in_distribute = true;

            RT rtVal;
            while(itConnection != base_type::mConnections.end()) {
                detail::BasicConnection<RT(T0, T1, T2, T3, T4, T5)>* con =
                static_cast<detail::BasicConnection<RT(T0, T1, T2, T3, T4, T5)>* >(itConnection->second.get_con_base());
                
                if(!con->__in_delete_list)
                    rtVal = con->slot.get()->fn(a0, a1, a2, a3, a4, a5);
                ++itConnection;
            }
            detail::SignalImpl::in_distribute = false;

            return rtVal;
        }
        
        RT operator()(T0 a0, T1 a1, T2 a2, T3 a3, T4 a4, T5 a5) { sig(a0, a1, a2, a3, a4, a5); }
    };
    
    template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5>
    class Signal<void(T0, T1, T2, T3, T4, T5)>: public SignalBase<void(T0, T1, T2, T3, T4, T5)> {
    public:
        void sig(T0 a0, T1 a1, T2 a2, T3 a3, T4 a4, T5 a5) {
            typedef SignalBase<void(T0, T1, T2, T3, T4, T5)> base_type;
            typename base_type::ConnectionMap::const_iterator itConnection = base_type::mConnections.begin();
            
            detail::SignalImpl::in_distribute = true;

            while(itConnection != base_type::mConnections.end()) {
                detail::BasicConnection<void(T0, T1, T2, T3, T4, T5)>* con =
                static_cast<detail::BasicConnection<void(T0, T1, T2, T3, T4, T5)>* >(itConnection->second.get_con_base());
                
                if(!con->__in_delete_list)
                    con->slot.get()->fn(a0, a1, a2, a3, a4, a5);
                ++itConnection;
            }
            detail::SignalImpl::in_distribute = false;

        }
        
        void operator()(T0 a0, T1 a1, T2 a2, T3 a3, T4 a4, T5 a5) { sig(a0, a1, a2, a3, a4, a5); }
    };
    
    template<typename RT, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
    class Signal<RT(T0, T1, T2, T3, T4, T5, T6)>: public SignalBase<RT(T0, T1, T2, T3, T4, T5, T6)> {
    public:
        RT sig(T0 a0, T1 a1, T2 a2, T3 a3, T4 a4, T5 a5, T6 a6) {
            typedef SignalBase<RT(T0, T1, T2, T3, T4, T5, T6)> base_type;
            typename base_type::ConnectionMap::const_iterator itConnection = base_type::mConnections.begin();
            
            detail::SignalImpl::in_distribute = true;

            RT rtVal;
            while(itConnection != base_type::mConnections.end()) {
                detail::BasicConnection<RT(T0, T1, T2, T3, T4, T5, T6)>* con =
                static_cast<detail::BasicConnection<RT(T0, T1, T2, T3, T4, T5, T6)>* >(itConnection->second.get_con_base());
                
                if(!con->__in_delete_list)
                    rtVal = con->slot.get()->fn(a0, a1, a2, a3, a4, a5, a6);
                ++itConnection;
            }
            detail::SignalImpl::in_distribute = false;

            return rtVal;
        }
        
        RT operator()(T0 a0, T1 a1, T2 a2, T3 a3, T4 a4, T5 a5, T6 a6) { sig(a0, a1, a2, a3, a4, a5, a6); }
    };
    
    template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
    class Signal<void(T0, T1, T2, T3, T4, T5, T6)>: public SignalBase<void(T0, T1, T2, T3, T4, T5, T6)> {
    public:
        void sig(T0 a0, T1 a1, T2 a2, T3 a3, T4 a4, T5 a5, T6 a6) {
            typedef SignalBase<void(T0, T1, T2, T3, T4, T5, T6)> base_type;
            typename base_type::ConnectionMap::const_iterator itConnection = base_type::mConnections.begin();
            
            detail::SignalImpl::in_distribute = true;

            while(itConnection != base_type::mConnections.end()) {
                detail::BasicConnection<void(T0, T1, T2, T3, T4, T5, T6)>* con =
                static_cast<detail::BasicConnection<void(T0, T1, T2, T3, T4, T5, T6)>* >(itConnection->second.get_con_base());
                
                if(!con->__in_delete_list)
                    con->slot.get()->fn(a0, a1, a2, a3, a4, a5, a6);
                ++itConnection;
            }
            detail::SignalImpl::in_distribute = false;

        }
        
        void operator()(T0 a0, T1 a1, T2 a2, T3 a3, T4 a4, T5 a5, T6 a6) { sig(a0, a1, a2, a3, a4, a5, a6); }
    };
    
    template<typename RT, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
    class Signal<RT(T0, T1, T2, T3, T4, T5, T6, T7)>: public SignalBase<RT(T0, T1, T2, T3, T4, T5, T6, T7)> {
    public:
        RT sig(T0 a0, T1 a1, T2 a2, T3 a3, T4 a4, T5 a5, T6 a6, T7 a7) {
            typedef SignalBase<RT(T0, T1, T2, T3, T4, T5, T6, T7 a7)> base_type;
            typename base_type::ConnectionMap::const_iterator itConnection = base_type::mConnections.begin();
            
            detail::SignalImpl::in_distribute = true;

            RT rtVal;
            while(itConnection != base_type::mConnections.end()) {
                detail::BasicConnection<RT(T0, T1, T2, T3, T4, T5, T6, T7)>* con =
                static_cast<detail::BasicConnection<RT(T0, T1, T2, T3, T4, T5, T6, T7)>* >(itConnection->second.get_con_base());
                
                if(!con->__in_delete_list)
                    rtVal = con->slot.get()->fn(a0, a1, a2, a3, a4, a5, a6, a7);
                ++itConnection;
            }
            detail::SignalImpl::in_distribute = false;

            return rtVal;
        }
        
        RT operator()(T0 a0, T1 a1, T2 a2, T3 a3, T4 a4, T5 a5, T6 a6, T7 a7) { sig(a0, a1, a2, a3, a4, a5, a6, a7); }
    };
    
    template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
    class Signal<void(T0, T1, T2, T3, T4, T5, T6, T7)>: public SignalBase<void(T0, T1, T2, T3, T4, T5, T6, T7)> {
    public:
        void sig(T0 a0, T1 a1, T2 a2, T3 a3, T4 a4, T5 a5, T6 a6, T7 a7) {
            typedef SignalBase<void(T0, T1, T2, T3, T4, T5, T6, T7)> base_type;
            typename base_type::ConnectionMap::const_iterator itConnection = base_type::mConnections.begin();
            
            detail::SignalImpl::in_distribute = true;

            while(itConnection != base_type::mConnections.end()) {
                detail::BasicConnection<void(T0, T1, T2, T3, T4, T5, T6, T7)>* con =
                static_cast<detail::BasicConnection<void(T0, T1, T2, T3, T4, T5, T6, T7)>* >(itConnection->second.get_con_base());
                
                if(!con->__in_delete_list)
                    con->slot.get()->fn(a0, a1, a2, a3, a4, a5, a6, a7);
                ++itConnection;
            }
            detail::SignalImpl::in_distribute = false;

        }
        
        void operator()(T0 a0, T1 a1, T2 a2, T3 a3, T4 a4, T5 a5, T6 a6, T7 a7) { sig(a0, a1, a2, a3, a4, a5, a6, a7); }
    };
    
    template<typename RT, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
    class Signal<RT(T0, T1, T2, T3, T4, T5, T6, T7, T8)>: public SignalBase<RT(T0, T1, T2, T3, T4, T5, T6, T7, T8)> {
    public:
        RT sig(T0 a0, T1 a1, T2 a2, T3 a3, T4 a4, T5 a5, T6 a6, T7 a7, T8 a8) {
            typedef SignalBase<RT(T0, T1, T2, T3, T4, T5, T6, T7 a7, T8 a8)> base_type;
            typename base_type::ConnectionMap::const_iterator itConnection = base_type::mConnections.begin();
            
            detail::SignalImpl::in_distribute = true;

            RT rtVal;
            while(itConnection != base_type::mConnections.end()) {
                detail::BasicConnection<RT(T0, T1, T2, T3, T4, T5, T6, T7, T8)>* con =
                static_cast<detail::BasicConnection<RT(T0, T1, T2, T3, T4, T5, T6, T7, T8)>* >(itConnection->second.get_con_base());
                
                if(!con->__in_delete_list)
                    rtVal = con->slot.get()->fn(a0, a1, a2, a3, a4, a5, a6, a7, a8);
                ++itConnection;
            }
            detail::SignalImpl::in_distribute = false;

            return rtVal;
        }
        
        RT operator()(T0 a0, T1 a1, T2 a2, T3 a3, T4 a4, T5 a5, T6 a6, T7 a7, T8 a8) { sig(a0, a1, a2, a3, a4, a5, a6, a7, a8); }
    };
    
    template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
    class Signal<void(T0, T1, T2, T3, T4, T5, T6, T7, T8)>: public SignalBase<void(T0, T1, T2, T3, T4, T5, T6, T7, T8)> {
    public:
        void sig(T0 a0, T1 a1, T2 a2, T3 a3, T4 a4, T5 a5, T6 a6, T7 a7, T8 a8) {
            typedef SignalBase<void(T0, T1, T2, T3, T4, T5, T6, T7, T8)> base_type;
            typename base_type::ConnectionMap::const_iterator itConnection = base_type::mConnections.begin();
            
            detail::SignalImpl::in_distribute = true;

            while(itConnection != base_type::mConnections.end()) {
                detail::BasicConnection<void(T0, T1, T2, T3, T4, T5, T6, T7, T8)>* con =
                static_cast<detail::BasicConnection<void(T0, T1, T2, T3, T4, T5, T6, T7, T8)>* >(itConnection->second.get_con_base());
                
                if(!con->__in_delete_list)
                    con->slot.get()->fn(a0, a1, a2, a3, a4, a5, a6, a7, a8);
                ++itConnection;
            }
            detail::SignalImpl::in_distribute = false;

        }
        
        void operator()(T0 a0, T1 a1, T2 a2, T3 a3, T4 a4, T5 a5, T6 a6, T7 a7, T8 a8) { sig(a0, a1, a2, a3, a4, a5, a6, a7, a8); }
    };
        
#endif // MIST_CPP11
    
} // namespace mist


#endif