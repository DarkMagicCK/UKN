//
//  StringUtilApple.h
//  Project Unknown
//
//  Created by Ruiwei Bu on 11/30/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_StringUtilApple_h
#define Project_Unknown_StringUtilApple_h

#include "Platform.h"

namespace ukn {
    
    // ios and osx wrapper functions
     
    ukn_wstring ukn_apple_string_to_wstring(const ukn_string& str);
    ukn_string ukn_apple_wstring_to_string(const ukn_wstring& str);
    
    bool ukn_apple_file_exists(const ukn_wstring& file);
    ukn_wstring ukn_apple_application_path();
    ukn_wstring ukn_apple_documents_path();
    ukn_wstring ukn_apple_resource_path();
    
} // namespace ukn


#endif
