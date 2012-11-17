//
//  Message.h
//  Mist
//
//  Created by Ruiwei Bu on 1/9/12.
//  Copyright (c) 2012 heizi. All rights reserved.
//

#ifndef MISTMessage_h
#define MISTMessage_h

#include "mist/Platform.h"
#include "mist/Any.h"

namespace mist {
    
    // defines MessageEventArgs
    
    struct MessageEventArgs {
        // message id
        int32 message_id;
        
        // message data
        Any    message_data;
        
        MessageEventArgs(int32 mid = 0):
        message_id(mid) {
            
        }
        
        template<typename T>
        MessageEventArgs(int32 mid, T t):
        message_id(mid),
        message_data(t) {
            
        }
    };
    
    static MessageEventArgs EmptyMessageEventArgs(0);
    
} // namespace mist

#endif
