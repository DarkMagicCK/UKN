//
//  Hashing.h
//  Project Unknown
//
//  Created by Robert Bu on 12/5/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_Hashing_h
#define Project_Unknown_Hashing_h

#include "UKN/Platform.h"
#include "UKN/PreDeclare.h"

namespace ukn {
    
    uint64 crc32(const char *buf, size_t size);
    uint64 crc32(const wchar_t *buf, size_t size);
    uint64 crc32(const ukn_string& str);
    uint64 crc32(const ukn_wstring& str);
    
    ukn_string stream_get_md5(const StreamPtr stream);
    ukn_string resource_get_md5(const ResourcePtr resource);
    
} // namespace ukn


#endif
