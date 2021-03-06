//
//  ZipUtil.h
//  Project Unknown
//
//  Created by Ruiwei Bu on 1/10/12.
//  Copyright (c) 2012 heizi. All rights reserved.
//

#ifndef Project_Unknown_ZipUtil_h
#define Project_Unknown_ZipUtil_h

#include "mist/Platform.h"
#include "mist/Util.h"

namespace mist {
    
    // utility functions for compress & decompress zlib/gzip data
    // returns a memory stream which contains the compressed/decompressed data
    
    enum ZlibCompressionMethod {
        ZCM_Gzip,
        ZCM_Zlib,
    };
    
    MIST_API Array<uint8> zlib_compress(uint8* input, uint32 input_length, ZlibCompressionMethod method = ZCM_Gzip);
    MIST_API Array<uint8> zlib_decompress(uint8* input, uint32 input_length);
    
    
} // namespace ukn


#endif
