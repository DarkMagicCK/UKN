//
//  StringTableLoader.h
//  Project Unknown
//
//  Created by Robert Bu on 7/22/12.
//  Copyright (c) 2012 heizi. All rights reserved.
//

#ifndef Project_Unknown_StringTableLoader_h
#define Project_Unknown_StringTableLoader_h

#include "UKN/Platform.h"

namespace ukn {
    
    class LocalizedStringTable;
    
    struct LocalizedStringTableLoader {
        static bool Load(const ukn_string& file, LocalizedStringTable& stringTable);
        static bool Save(const ukn_string& file, const LocalizedStringTable& stringTable);
    };
    
}

#endif
