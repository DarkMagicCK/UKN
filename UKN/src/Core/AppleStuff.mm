//
//  StringUtilApple.mm
//  Project Unknown
//
//  Created by Ruiwei Bu on 11/30/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#include "AppleStuff.h"

#include "UKN/StringUtil.h"
#include "UKN/Common.h"

#ifdef UKN_OS_FAMILY_APPLE

#include <CoreFoundation/CoreFoundation.h>
#include <Foundation/Foundation.h>

#ifdef UKN_OS_OSX
#include <sys/sysctl.h>

#else

#endif

namespace ukn {
    
    ukn_wstring ukn_apple_string_to_wstring(const ukn_string& str) {   
        NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

		NSString* nsstr = [[NSString alloc] initWithUTF8String: str.c_str()];
        NSStringEncoding pEncode    =   CFStringConvertEncodingToNSStringEncoding ( kCFStringEncodingUTF32LE );   
		NSData* pSData              =   [ nsstr dataUsingEncoding : pEncode ];    
		
        std::wstring result = std::wstring ( (wchar_t*) [ pSData bytes ], [ pSData length] / sizeof ( wchar_t ) ); 
        [nsstr release];
        
        [pool drain];
        return result;
	}
	
	ukn_string ukn_apple_wstring_to_string(const ukn_wstring& str) { 
        NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

        NSString* pString = [ [ NSString alloc ]    
							 initWithBytes : (char*)str.data()   
							 length : str.size() * sizeof(wchar_t)   
                             encoding : CFStringConvertEncodingToNSStringEncoding ( kCFStringEncodingUTF32LE ) ];   
        
        std::string result = [pString UTF8String];
        [pString release];
        
        [pool drain];
		return result;
	}
    
    bool ukn_apple_file_exists(const ukn_wstring& file) {        
		NSString* nsPath = [[NSString alloc] initWithUTF8String:wstring_to_string(file).c_str()];
		bool result = [[NSFileManager defaultManager] fileExistsAtPath:nsPath];
        [nsPath release];
        
        return result;
    }
    
    ukn_wstring ukn_apple_application_path() {
        return string_to_wstring_fast([[[NSBundle mainBundle] bundlePath] UTF8String]) + L"/";
    }
    
    ukn_wstring ukn_apple_documents_path() {
        NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
		NSString *basePath = ([paths count] > 0) ? [paths objectAtIndex:0] : nil;
		ukn_wstring docPath = string_to_wstring_fast([basePath UTF8String]) + L"/";
        return docPath;
    }
    
#ifdef UKN_OS_OSX
    
    uint64 ukn_apple_get_processor_speed() {
        int mib[2] = { CTL_HW, HW_CPU_FREQ };
        u_int namelen = sizeof(mib) / sizeof(mib[0]);
        uint64_t freq = 0;
        size_t len = sizeof(freq);
        
        sysctl(mib, namelen, &freq, &len, NULL, 0);
        return freq / 1000000;
    }
    
    uint64 ukn_apple_get_memory_size() {
        int mib[2] = { CTL_HW, HW_MEMSIZE };
        u_int namelen = sizeof(mib) / sizeof(mib[0]);
        uint64_t size = 0;
        size_t len = sizeof(size);
        
        sysctl(mib, namelen, &size, &len, NULL, 0);
        return size;
    }
    
    ukn_string ukn_apple_get_os_version() {
        SInt32 versionMajor = 0;
        SInt32 versionMinor = 0;
        Gestalt( gestaltSystemVersionMajor, &versionMajor );
        Gestalt( gestaltSystemVersionMinor, &versionMinor );
        return format_string("Mac OS X Version %d.%d", 
                             versionMajor, 
                             versionMinor);
    }
    
    inline int32 mb_option_to_kCFNotificationLevel(int32 option) {
		if(option & MBO_IconError)
			return kCFUserNotificationCautionAlertLevel;

		else if(option & MBO_IconInfo) 
			return kCFUserNotificationNoteAlertLevel;
        
		else if(option & MBO_IconWarning)
			return kCFUserNotificationPlainAlertLevel;
		
		return kCFUserNotificationPlainAlertLevel;
	}
    
    MessageBoxButton ukn_apple_message_box(const ukn_string& mssg, const ukn_string& title, MessageBoxOption option) {
        CFStringRef header_ref   = CFStringCreateWithCString(NULL, title.c_str(), (uint32)title.size());
		CFStringRef message_ref  = CFStringCreateWithCString(NULL, mssg.c_str(), (uint32)mssg.size());
		
		CFOptionFlags result;
		int32 level = mb_option_to_kCFNotificationLevel(option);
		CFUserNotificationDisplayAlert(0,
									   level,
									   NULL,
									   NULL,
									   NULL,
									   header_ref,
									   message_ref,
									   NULL, // default "ok"
									   (option & MBO_OKCancel)?CFSTR("Cancel"):NULL, // alternate button
									   NULL, //other button title
									   &result
									   );
		
		CFRelease( header_ref );
		CFRelease( message_ref );
		
		switch(result) {
			case kCFUserNotificationDefaultResponse: return MBB_OK;
			case kCFUserNotificationCancelResponse: return MBB_Cancel;
			case kCFUserNotificationAlternateResponse: return MBB_Cancel;
			case kCFUserNotificationOtherResponse:
            default:
                return MBB_Cancel;
		}
    }
    
    MessageBoxButton ukn_apple_message_box(const ukn_wstring& mssg, const ukn_wstring& title, MessageBoxOption option) {
        return ukn_apple_message_box(wstring_to_string(mssg),
                                     wstring_to_string(title),
                                     option);
    }
    
    
    int ukn_get_os_osx_version() {
        return MAC_OS_X_VERSION_MAX_ALLOWED;
    }
    
#elif defined(UKN_OS_IOS)
    
    uint32 ukn_apple_get_processor_speed() {
        return 0;
    }
    
    uint64 ukn_apple_get_memory_size() {
        int mib[2] = { CTL_HW, HW_MEMSIZE };
        u_int namelen = sizeof(mib) / sizeof(mib[0]);
        uint64_t size = 0;
        size_t len = sizeof(size);
        
        sysctl(mib, namelen, &size, &len, NULL, 0);
        return 0;
    }
    
    ukn_string ukn_apple_get_os_version() {
        NSString*  systemVersion=[[UIDevice currentDevice] systemVersion];
        NSString*  model=[[UIDevice currentDevice] model];
        
        return format_string("iOS %s %s", 
                             [systemVersion UTF8String],
                             [model UTF8String]);
    }
    
    MessageBoxButton ukn_apple_message_box(const ukn_string& mssg, const ukn_string& title, MessageBoxOption option) {
        NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
        
        NSString* nsMessage = [[NSString alloc] initWithUTF8String:sMessage.c_str()];
		NSString* nsTitle = [[NSString alloc] initWithUTF8String:sTitle.c_str()];
		
		UIAlertView* alert = [[UIAlertView alloc]
							  initWithTitle:nsTitle
							  message:nsMessage
							  delegate:nil 
							  cancelButtonTitle:(option & MBO_OKCancel)?@"Cancel":nil
							  otherButtonTitles:nil];
		[alert show];
        
        // release
        [alert release];
        [nsMessage release];
        [nsTitle release];
        
        [pool drain];
    
		return 1;
    }
    
    MessageBoxButton ukn_apple_message_box(const ukn_wstring& mssg, const ukn_wstring& title, MessageBoxOption option) {
        return ukn_apple_message_box(wstring_to_string(mssg),
                                     wstring_to_string(title),
                                     option);
    }
    
#endif
    
    
} // namespace ukn

#endif // UKN_OS_FAMILY_APPLE
