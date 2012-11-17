//
//  Signal.cpp
//  Mist
//
//  Created by Robert Bu on 12/8/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#include "mist/Signal.h"

namespace mist {

#if 1
    
    Connection::Connection():
    isControlling(false) {
        
    }

    Connection::~Connection() {
        if(isControlling) {
            disconnect();
        }
    }

    bool Connection::isValid() const {
        if(con.get())
            return con->isValid();
        return false;
    }

    void Connection::disconnect() {
        if(con.get()) {
            isControlling = false;
            con->disconnect();
        }
    }

    bool Connection::operator ==(const Connection &rhs) {
        mist_assert(con.get());
        return con.get() == rhs.con.get();
    }

    detail::ConnectionBase* Connection::get_con_base() const {
        return con.get();
    }

    Connection& Connection::operator=(const Connection& rhs) {
        if(this != &rhs) {
            isControlling = false;
            con = rhs.con;
        }
        return *this;
    }

    void Connection::setControl(bool flag) {
        isControlling = flag;
    }

    void Connection::reset(detail::ConnectionBase* _con) {
        con.reset(_con);
    }

    namespace detail {

        void SignalImpl::clearConnection() {
            IteratorType it = mConnections.begin();
            while(it != mConnections.end()) {
                it->second.setControl(false);
                ++it;
            }
            mConnections.clear();
        }
        
        void SignalImpl::disconnect(void* con, void* data) {
            if(!in_distribute) {
                std::auto_ptr<IteratorType> slot(reinterpret_cast<IteratorType*>(data));
                mConnections.erase(*slot);
                
            } else {
                __to_delete.push_back(data);
                
                ((ConnectionBase*)con)->__in_delete_list = true;
            }
        }

    } // namespace detail
    
#endif

}   // namespace mist
