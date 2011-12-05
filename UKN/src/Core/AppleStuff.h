//
//  StringUtilApple.h
//  Project Unknown
//
//  Created by Ruiwei Bu on 11/30/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_StringUtilApple_h
#define Project_Unknown_StringUtilApple_h

#include "UKN/Platform.h"
#include "UKN/SysUtil.h"

namespace ukn {
    
    // ios and osx wrapper functions
     
    ukn_wstring ukn_apple_string_to_wstring(const ukn_string& str);
    ukn_string ukn_apple_wstring_to_string(const ukn_wstring& str);
    
    bool ukn_apple_file_exists(const ukn_wstring& file);
    ukn_wstring ukn_apple_application_path();
    ukn_wstring ukn_apple_documents_path();
    ukn_wstring ukn_apple_resource_path();
    
    uint64 ukn_apple_get_processor_speed();
    uint64 ukn_apple_get_memory_size();
    ukn_string ukn_apple_get_os_version();
    
    MessageBoxButton ukn_apple_message_box(const ukn_string& mssg, const ukn_string& title, int option);
    MessageBoxButton ukn_apple_message_box(const ukn_wstring& mssg, const ukn_wstring& title, int option);
    
#ifdef UKN_OS_OSX
    int ukn_get_os_osx_version();
#endif
    
} // namespace ukn


#endif
