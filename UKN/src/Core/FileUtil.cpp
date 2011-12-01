//
//  FileUtil.cpp
//  Project Unknown
//
//  Created by Ruiwei Bu on 11/30/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#include "UKN/Platform.h"
#include "UKN/FileUtil.h"

#ifdef UKN_OS_WINDOWS
#include <Windows.h>
#include "Shlwapi.h"
#pragma comment(lib,"shlwapi.lib")

#elif defined(UKN_OS_FAMILY_APPLE)
#include "AppleStuff.h"
#endif

#include <cstdio>
#include <fstream>

namespace ukn {
    
    UKN_API bool ukn_file_exists(const ukn_wstring& filepath) {
#ifdef UKN_OS_WINDOWS
        return PathFileExistsW(filepath.c_str())?true:false;
        
#elif defined(UKN_OS_FAMILY_APPLE)
        return ukn_apple_file_exists(filepath);
#endif
        
        std::ifstream file;
        file.open(ukn_apple_wstring_to_string(filepath).c_str());
        if(!file.is_open()) return false;
        file.close();
        return true;
    }
    
    UKN_API ukn_wstring ukn_get_writtable_path(const ukn_wstring& filePath) {
#ifndef UKN_OS_FAMILY_APPLE
        return ukn_get_application_path() + filePath;
#else
        return ukn_apple_documents_path() + filePath;
#endif
    }
    
    UKN_API ukn_wstring ukn_get_application_path() {
#ifdef UKN_OS_WINDOWS
        wchar_t buffer[MAX_PATH];
        GetCurrentDirectoryW(MAX_PATH, buffer);
        
        return filePath+L"/";
        
#elif defined(UKN_OS_FAMILY_APPLE)
        return ukn_apple_application_path() + L"/";
        
#endif
        return L"./";
    }
    
} // namespace ukn
