//
//  MistStringUtilApple.mm
//  Mist
//
//  Created by Robert Bu on 11/30/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#include "AppleStuff.h"

#include "mist/StringUtil.h"
#include "mist/Common.h"

#include "mist/Logger.h"

#ifdef MIST_OS_FAMILY_APPLE

#include <CoreFoundation/CoreFoundation.h>
#include <Foundation/Foundation.h>

#ifdef MIST_OS_OSX
#include <sys/sysctl.h>

#else

#import <UIKit/UIkit.h>
#import <UIKit/UIDevice.h>
#import <AudioToolbox/AudioToolbox.h>


#endif

namespace mist {
    
    inline NSString* WStringToNSString(const MistString& str) {
#if !__has_feature(objc_arc)
        NSString* pString = [[ [ NSString alloc ]
							 initWithBytes : (char*)str.data()
							 length : str.size() * sizeof(wchar_t)
                             encoding : CFStringConvertEncodingToNSStringEncoding ( kCFStringEncodingUTF32LE ) ] autorelease];
        return pString;
#else
        NSString* pString = [ [ NSString alloc ]
                              initWithBytes : (char*)str.data()
                              length : str.size() * sizeof(wchar_t)
                              encoding : CFStringConvertEncodingToNSStringEncoding ( kCFStringEncodingUTF32LE ) ];
        return pString;
#endif
    }
    
    std::wstring mist_apple_string_to_wstring(const std::string& str) {
#if !__has_feature(objc_arc)
		NSString* nsstr = [[[NSString alloc] initWithUTF8String: str.c_str()] autorelease];
#else
        NSString* nsstr = [[NSString alloc] initWithUTF8String: str.c_str()];

#endif
        NSStringEncoding pEncode    =   CFStringConvertEncodingToNSStringEncoding ( kCFStringEncodingUTF32LE );
		NSData* pSData              =   [ nsstr dataUsingEncoding : pEncode ];    
		
        std::wstring result = std::wstring ( (wchar_t*) [ pSData bytes ], [ pSData length] / sizeof ( wchar_t ) );         
        return result;
	}
	
    
	std::string mist_apple_wstring_to_string(const std::wstring& str) {
        return [WStringToNSString(str) UTF8String];
	}
    
    bool mist_apple_file_exists(const MistString& file) {        
		NSString* nsPath = [[NSString alloc] initWithUTF8String:string::WStringToString(file).c_str()];
		bool result = [[NSFileManager defaultManager] fileExistsAtPath:nsPath];
        
        return result;
    }
    
    MistString mist_apple_application_path() {
        return string::StringToWStringFast([[[NSBundle mainBundle] bundlePath] UTF8String]) + L"/";
    }
    
    MistString mist_apple_documents_path() {
        NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
		NSString *basePath = ([paths count] > 0) ? [paths objectAtIndex:0] : nil;
		MistString docPath = string::StringToWStringFast([basePath UTF8String]) + L"/";
        return docPath;
    }
    
    
    bool mist_apple_copyitem(const MistString& src, const MistString& dst) {
        NSFileManager* fm = [NSFileManager defaultManager];
        
        NSError* error = nil;
        [fm copyItemAtPath:WStringToNSString(src) toPath:WStringToNSString(dst) error:&error];

        if(error != nil) {
            log_error([[error localizedDescription] UTF8String]);
            return false;
        }
        return true;
    }
    
    bool mist_apple_moveitem(const MistString& src, const MistString& dst) {
        NSFileManager* fm = [NSFileManager defaultManager];
        
        NSError* error = nil;
        [fm moveItemAtPath:WStringToNSString(src) toPath:WStringToNSString(dst) error:&error];
        
        if(error != nil) {
            log_error([[error localizedDescription] UTF8String]);
            return false;
        }
        return true;
    }
    
    bool mist_apple_deleteitem(const MistString& src) {
        NSFileManager* fm = [NSFileManager defaultManager];
        
        NSError* error = nil;
        [fm removeItemAtPath:WStringToNSString(src) error:&error];
        
        if(error != nil) {
            log_error([[error localizedDescription] UTF8String]);
            return false;
        }
        return true;
    }
    
    bool mist_apple_createdirectory(const MistString& src) {
        NSFileManager* fm = [NSFileManager defaultManager];
        
        NSError* error = nil;
        [fm createDirectoryAtPath:WStringToNSString(src)
      withIntermediateDirectories:YES
                       attributes:nil
                            error:&error];
        if(error != nil) {
            log_error([[error localizedDescription] UTF8String]);
            return false;
        }
        return true;
    }
    
    void mist_apple_run_in_mainthread(const Function<void(void)>& f) {
        __block Function<void(void)> _f = f;
        dispatch_async(
                       dispatch_get_current_queue(), ^{
                           _f();
                       });
    }

    
#ifdef MIST_OS_OSX
   
    MistString mist_apple_get_os_version() {
        SInt32 versionMajor = 0;
        SInt32 versionMinor = 0;
        Gestalt( gestaltSystemVersionMajor, &versionMajor );
        Gestalt( gestaltSystemVersionMinor, &versionMinor );
        return string::StringToWString(format_string("Mac OS X Version %d.%d",
                                                     versionMajor,
                                                     versionMinor));
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
    
    MessageBoxButton mist_apple_message_box(const std::wstring& mssg, const std::wstring& title, int option) {
        CFStringRef header_ref   = CFStringCreateWithCString(NULL, string::WStringToString(mssg).c_str(), (uint32)title.size());
		CFStringRef message_ref  = CFStringCreateWithCString(NULL, string::WStringToString(title).c_str(), (uint32)mssg.size());
		
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
									   (option & MBO_OKCancel)?CFSTR("Cancel") :
                                       NULL, // alternate button
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
    
    int mist_get_os_osx_version() {
        return MAC_OS_X_VERSION_MAX_ALLOWED;
    }
    
    static SystemInformation::DesktopMode vidmodeFromCGDisplayMode(NSDictionary* mode) {
        SystemInformation::DesktopMode result;
        result.width    = [[mode objectForKey:(id)kCGDisplayWidth] unsignedIntValue];;
        result.height   = [[mode objectForKey:(id)kCGDisplayHeight] unsignedIntValue];;
        result.bpp      = [[mode objectForKey:(id)kCGDisplayBitsPerPixel] unsignedIntValue];;
        return result;
    }
    
    void mist_apple_enum_desktop_modes(Array<SystemInformation::DesktopMode>& arr) {
#if MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_6
        CGDisplayModeRef cgmode = CGDisplayCopyDisplayMode(kCGDirectMainDisplay);
        SystemInformation::DesktopMode mode;
        mode.width  = (uint32)CGDisplayModeGetWidth(cgmode);
        mode.height = (uint32)CGDisplayModeGetHeight(cgmode);
        
        CFStringRef pixEnc = CGDisplayModeCopyPixelEncoding(cgmode);
        if(CFStringCompare(pixEnc, CFSTR(IO32BitDirectPixels), kCFCompareCaseInsensitive) == kCFCompareEqualTo)
            mode.bpp = 32;
        else if(CFStringCompare(pixEnc, CFSTR(IO16BitDirectPixels), kCFCompareCaseInsensitive) == kCFCompareEqualTo)
            mode.bpp = 16;
        else if(CFStringCompare(pixEnc, CFSTR(IO8BitIndexedPixels), kCFCompareCaseInsensitive) == kCFCompareEqualTo)
            mode.bpp = 8;
        
        arr.push_back(mode);
        
        CFRelease(pixEnc);
        
        CGDisplayModeRelease(cgmode);
        
#else
        NSDictionary *mode = (NSDictionary*)CGDisplayCurrentMode(kCGDirectMainDisplay);
        arr.push_back(vidmodeFromCGDisplayMode(mode));
#endif
        
#if MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_6
        CFArrayRef modes = CGDisplayCopyAllDisplayModes(CGMainDisplayID(), 0);
        CFIndex count = CFArrayGetCount(modes);
        
        for (CFIndex index = 0; index < count; index++) {
            CGDisplayModeRef cgmode = (CGDisplayModeRef)CFArrayGetValueAtIndex(modes, index);
            
            SystemInformation::DesktopMode mode;
            mode.width  = (uint32)CGDisplayModeGetWidth(cgmode);
            mode.height = (uint32)CGDisplayModeGetHeight(cgmode);
            
            CFStringRef pixEnc = CGDisplayModeCopyPixelEncoding(cgmode);
            if(CFStringCompare(pixEnc, CFSTR(IO32BitDirectPixels), kCFCompareCaseInsensitive) == kCFCompareEqualTo)
                mode.bpp = 32;
            else if(CFStringCompare(pixEnc, CFSTR(IO16BitDirectPixels), kCFCompareCaseInsensitive) == kCFCompareEqualTo)
                mode.bpp = 16;
            else if(CFStringCompare(pixEnc, CFSTR(IO8BitIndexedPixels), kCFCompareCaseInsensitive) == kCFCompareEqualTo)
                mode.bpp = 8;
            CFRelease(pixEnc);

            arr.push_back(mode);
        }
        CFRelease(modes);
        
#else
        NSArray* modes = (NSArray*) CGDisplayAvailableModes(CGMainDisplayID());
        unsigned long n = [modes count];
        
        for(unsigned long i = 0;  i < n;  i++) {
            NSDictionary *mode = [modes objectAtIndex:i];
            
            arr.push_back(vidmodeFromCGDisplayMode(mode));
        }
#endif
    }
      
#elif defined(MIST_OS_IOS)
    
    iOSDeviceType mist_get_ios_device_type() {
        if(mist_ios_is_ipad()) {
            if(mist_ios_is_retina_display())
                return IDT_iPadRetina;
            return IDT_iPad;
        }
        else if(mist_ios_is_retina_display())
            return IDT_Retina;
        else
            return IDT_Old;
    }
    
    bool mist_ios_is_ipad() {
        return UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad;
    }
    
    bool mist_ios_is_retina_display() {
        if ([[UIScreen mainScreen] respondsToSelector:@selector(displayLinkWithTarget:selector:)] && ([UIScreen mainScreen].scale == 2.0)) { 
            return true; 
        }	
        return false; 
    }
    
    uint32 mist_ios_get_screen_width() {
        CGRect rect = [[UIScreen mainScreen] bounds];
        return rect.size.width;
    }
    
    uint32 mist_ios_get_screen_height() {
        CGRect rect = [[UIScreen mainScreen] bounds];
        return rect.size.height;
    }
    
    float mist_ios_get_scale_factor() {
        return mist_ios_is_retina_display() ? 2.0 : 1.0;
    }
    
    void mist_ios_vabrite_device() {
        AudioServicesPlaySystemSound(kSystemSoundID_Vibrate);
    }
    
    static bool g_use_retina = false;
    
    void mist_ios_set_use_retina(bool flag) {
        g_use_retina = flag;
    }
    
    bool mist_ios_is_use_retina() {
        return g_use_retina;
    }
    
    uint64 mist_apple_get_processor_speed() {
        return 0;
    }
    
    void mist_apple_enum_desktop_modes(Array<SystemInformation::DesktopMode>& arr) {
        SystemInformation::DesktopMode mode;
        mode.width = mist_ios_get_screen_width();
        mode.height = mist_ios_get_screen_height();
        mode.bpp = 32;
        
        arr.push_back(mode);
    }
    
    uint64 mist_apple_get_memory_size() {
#ifndef MIST_OS_IOS
        int mib[2] = { CTL_HW, HW_MEMSIZE };
        u_int namelen = sizeof(mib) / sizeof(mib[0]);
        uint64_t size = 0;
        size_t len = sizeof(size);
        
        sysctl(mib, namelen, &size, &len, NULL, 0);
#endif
        return 0;
    }
    
    MistString mist_apple_get_os_version() {
        NSString*  systemVersion=[[UIDevice currentDevice] systemVersion];
        NSString*  model=[[UIDevice currentDevice] model];
        
        return format_string("iOS %s %s", 
                             [systemVersion UTF8String],
                             [model UTF8String]);
    }
    
    MessageBoxButton mist_apple_message_box(const MistString& mssg, const MistString& title, int option) {
        
        NSString* nsMessage = [[NSString alloc] initWithUTF8String:mssg.c_str()];
		NSString* nsTitle = [[NSString alloc] initWithUTF8String:title.c_str()];
		
		UIAlertView* alert = [[UIAlertView alloc]
							  initWithTitle:nsTitle
							  message:nsMessage
							  delegate:nil 
							  cancelButtonTitle:(option & MBO_OKCancel)?@"Cancel":nil
							  otherButtonTitles:nil];
		[alert show];
        
		return MBB_OK;
    }
    
    MessageBoxButton mist_apple_message_box(const MistString& mssg, const MistString& title, int option) {
        return mist_apple_message_box(string::WStringToString(mssg),
                                     string::WStringToString(title),
                                     option);
    }
    
#endif
    
    
} // namespace mist

#endif // MIST_OS_FAMILY_APPLE
