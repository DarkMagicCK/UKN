//
//  ImageWriter.h
//  Project Unknown
//
//  Created by Robert Bu on 2/19/13.
//  Copyright (c) 2013 heizi. All rights reserved.
//

#ifndef Project_Unknown_ImageWriter_h
#define Project_Unknown_ImageWriter_h

#include "mist/Platform.h"

#include "UKN/PreDeclare.h"

namespace ukn {
    
    using namespace mist;
    
    class UKN_API ImageWriter {
    public:
        static Array<uint8> WritePNG(uint32* bits, uint32 width, uint32 height, uint32 pitch, bool setbgr=false, bool needAlpha=true);
    };
    
}


#endif
