//
//  FileUtil.h
//  Project Unknown
//
//  Created by Ruiwei Bu on 11/30/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_FileUtil_h
#define Project_Unknown_FileUtil_h

#include "Platform.h"

namespace ukn {
    
    UKN_API bool file_exists(const ukn_wstring& filepath);
    
    UKN_API ukn_wstring get_writtable_path(const ukn_wstring& filePath);
    
    UKN_API ukn_wstring get_application_path();
            
    
} // namespace ukn

#endif
