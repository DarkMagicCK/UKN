//
//  Serial.h
//  Project Mist
//
//  Created by Robert Bu on 12/2/12.
//  Copyright (c) 2012 heizi. All rights reserved.
//

#ifndef __Project_Unknown__Serial__
#define __Project_Unknown__Serial__

#include "mist/Platform.h"

/* *nix code based on 
 * Tod E. Kurt's Arduino-serial sample code
 * Copyleft (c) 2006, Tod E. Kurt, tod@todbot.com
 * http://todbot.com/blog/
 *
 * Windows & Other Platforms to do
 */

namespace mist {
    
    class Serial {
    public:
        Serial();
        Serial(const std::string& port, int baudRate);
        ~Serial();
        
        Serial& port(const std::string& port);
        Serial& baudRate(int rate);

        const std::string& port() const;
        int baudRate() const;
        
        bool        open();
        void        close();
        int         write(const char* data, size_t len);
        std::string read(char until);
        
    private:
        void init();
        
        class SerialImpl;
        SerialImpl* mImpl;
        
        std::string mPort;
        int mBaudRate;
    };

}


#endif /* defined(__Project_Unknown__Serial__) */
