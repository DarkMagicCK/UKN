//
//  Hashing.h
//  Mist
//
//  Created by Robert Bu on 12/5/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef MISTHashing_h
#define MISTHashing_h

#include "mist/Platform.h"
#include "mist/PreDeclare.h"

namespace mist {
    
    class Hash {
    public:
        static uint64 Crc32(const char *buf, size_t size);
        static uint64 Crc32(const wchar_t *buf, size_t size);
        static uint32 Adler32(const char* buf);
        static uint32 Adler32(const char* buf, size_t size);
        
        static std::string GetMD5(const StreamPtr stream);
    };
    
} // namespace mist


#endif
