//
//  Base64.h
//  Mist
//
//  Created by Ruiwei Bu on 1/10/12.
//  Copyright (c) 2012 heizi. All rights reserved.
//

#ifndef MISTBase64_h
#define MISTBase64_h

#include "mist/Platform.h"
#include "mist/Util.h"

namespace mist {
    
    MIST_API Array<uint8> base64_decode(const std::string& input);
    MIST_API std::string base64_encode(const uint8* bytes_to_encode, uint32 in_len);
    
} // namespace mist


#endif
