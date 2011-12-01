//
//  StringUtilApple.mm
//  Project Unknown
//
//  Created by Ruiwei Bu on 11/30/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#include "AppleStuff.h"

#include "UKN/StringUtil.h"

#include <CoreFoundation/CoreFoundation.h>
#include <Foundation/Foundation.h>

namespace ukn {
    
    ukn_wstring ukn_apple_string_to_wstring(const ukn_string& str) {        
		NSString* nsstr = [[NSString alloc] initWithUTF8String: str.c_str()];
        NSStringEncoding pEncode    =   CFStringConvertEncodingToNSStringEncoding ( kCFStringEncodingUTF32LE );   
		NSData* pSData              =   [ nsstr dataUsingEncoding : pEncode ];    
		
        std::wstring result = std::wstring ( (wchar_t*) [ pSData bytes ], [ pSData length] / sizeof ( wchar_t ) ); 
        //   [pSData release];
        [nsstr release];
        
        return result;
	}
	
	ukn_string ukn_apple_wstring_to_string(const ukn_wstring& str) {        
        NSString* pString = [ [ NSString alloc ]    
							 initWithBytes : (char*)str.data()   
							 length : str.size() * sizeof(wchar_t)   
                             encoding : CFStringConvertEncodingToNSStringEncoding ( kCFStringEncodingUTF32LE ) ];   
        
        std::string result = [pString UTF8String];
        [pString release];
        
		return result;
	}
    
    bool ukn_apple_file_exists(const ukn_wstring& file) {        
		NSString* nsPath = [[NSString alloc] initWithUTF8String:ukn_wstring_to_string(file).c_str()];
		bool result = [[NSFileManager defaultManager] fileExistsAtPath:nsPath];
        [nsPath release];
        
        return result;
    }
    
    ukn_wstring ukn_apple_application_path() {
        return ukn_string_to_wstring_fast([[[NSBundle mainBundle] bundlePath] UTF8String]) + L"/";
    }
    
    ukn_wstring ukn_apple_documents_path() {
        NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
		NSString *basePath = ([paths count] > 0) ? [paths objectAtIndex:0] : nil;
		ukn_wstring docPath = ukn_string_to_wstring_fast([basePath UTF8String]) + L"/";
        [basePath release];
        return docPath;
    }
    
    ukn_wstring ukn_apple_resource_path() {
        return ukn_apple_application_path();
    }
    
} // namespace ukn
