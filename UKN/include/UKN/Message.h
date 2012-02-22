//
//  Message.h
//  Project Unknown
//
//  Created by Ruiwei Bu on 1/9/12.
//  Copyright (c) 2012 heizi. All rights reserved.
//

#ifndef Project_Unknown_Message_h
#define Project_Unknown_Message_h

#include "UKN/Platform.h"
#include "UKN/Any.h"

namespace ukn {
    
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
    
} // namespace ukn

#endif
