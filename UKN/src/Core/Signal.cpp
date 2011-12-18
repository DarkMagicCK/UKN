//
//  Signal.cpp
//  Project Unknown
//
//  Created by Robert Bu on 12/8/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#include "UKN/Signal.h"

namespace ukn {

    Connection::Connection():
    isControlling(false) {
        
    }

    Connection::~Connection() {
        if(isControlling) {
            disconnect();
        }
    }

    bool Connection::isValid() {
        if(con.get())
            return con->isValid();
        return false;
    }

    void Connection::disconnect() {
        if(con.get())
            con->disconnect();
    }

    bool Connection::operator ==(const Connection &rhs) {
        ukn_assert(con.get());
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

    } // namespace detail

}   // namespace ukn
