//
//  MistObjc.m
//  Mist
//
//  Created by Robert Bu on 1/27/13.
//  Copyright (c) 2013 Robert Bu. All rights reserved.
//

/*
 Obj-C OSX and iOS Wrappers
 
 */

#import "MistObjc.h"

#ifdef MIST_OS_FAMILY_APPLE

#import <CoreFoundation/CoreFoundation.h>
#import <Foundation/Foundation.h>

#ifdef MIST_OS_OSX
#import <sys/sysctl.h>

#else

#import <UIKit/UIkit.h>
#import <UIKit/UIDevice.h>
#import <AudioToolbox/AudioToolbox.h>


#endif

#endif

@implementation MistObjc

+ (NSString*)applicationPath {
    return [[[NSBundle mainBundle] bundlePath] stringByAppendingString:@"/"];
}

+ (NSString*)resourcePath {
    return [[[NSBundle mainBundle] resourcePath] stringByAppendingString:@"/"];
}

+ (NSString*)resourcePathForResource:(NSString *)resource {
    return [[MistObjc resourcePath] stringByAppendingString:resource];
}

+ (NSString*)documentsPath {
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *basePath = ([paths count] > 0) ? [paths objectAtIndex:0] : nil;
    return [basePath stringByAppendingString:@"/"];
}

+ (bool)fileExists:(NSString*)file {
    return [[NSFileManager defaultManager] fileExistsAtPath:file];
}

+ (bool)copyFile:(NSString*)src dst:(NSString*)dst {
    NSFileManager* fm = [NSFileManager defaultManager];
    
    NSError* error = nil;
    [fm copyItemAtPath:src toPath:dst error:&error];
    
    if(error != nil) {
        NSLog(@"%@", [error localizedDescription]);
    }
    return error != nil;
}

+ (bool)moveFile:(NSString*)src dst:(NSString*)dst {
    NSFileManager* fm = [NSFileManager defaultManager];
    
    NSError* error = nil;
    [fm moveItemAtPath:src toPath:dst error:&error];
    
    if(error != nil) {
        NSLog(@"%@", [error localizedDescription]);
    }
    return error != nil;
}

+ (bool)deleteFile:(NSString*)src dst:(NSString*)dst {
    NSFileManager* fm = [NSFileManager defaultManager];
    
    NSError* error = nil;
    [fm removeItemAtPath:src error:&error];
    
    if(error != nil) {
        NSLog(@"%@", [error localizedDescription]);
    }
    return error != nil;
}

+ (bool)createDirectory:(NSString*)dir {
    NSFileManager* fm = [NSFileManager defaultManager];
    
    NSError* error = nil;
    [fm createDirectoryAtPath:dir
  withIntermediateDirectories:YES
                   attributes:nil
                        error:&error];
    if(error != nil) {
        NSLog(@"%@", [error localizedDescription]);
    }
    return error != nil;
}

#ifdef MIST_OS_OSX
+ (int) getOSXVersion {
    return MAC_OS_X_VERSION_MAX_ALLOWED;
}

+ (NSString*)osVersion {
    SInt32 versionMajor = 0;
    SInt32 versionMinor = 0;
    Gestalt( gestaltSystemVersionMajor, &versionMajor );
    Gestalt( gestaltSystemVersionMinor, &versionMinor );
    return [NSString stringWithFormat:@"Mac OS X Version %d.%d", versionMajor, versionMinor];
}

+ (void)enumDesktopModes:(void (^)(unsigned int w, unsigned int h, int bpp))callback {
#if MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_6
    CGDisplayModeRef cgmode = CGDisplayCopyDisplayMode(kCGDirectMainDisplay);
    
    int bpp = 0;
    
    CFStringRef pixEnc = CGDisplayModeCopyPixelEncoding(cgmode);
    if(CFStringCompare(pixEnc, CFSTR(IO32BitDirectPixels), kCFCompareCaseInsensitive) == kCFCompareEqualTo)
        bpp = 32;
    else if(CFStringCompare(pixEnc, CFSTR(IO16BitDirectPixels), kCFCompareCaseInsensitive) == kCFCompareEqualTo)
        bpp = 16;
    else if(CFStringCompare(pixEnc, CFSTR(IO8BitIndexedPixels), kCFCompareCaseInsensitive) == kCFCompareEqualTo)
        bpp = 8;
    
    callback((unsigned int)CGDisplayModeGetWidth(cgmode),
             (unsigned int)CGDisplayModeGetHeight(cgmode),
             bpp);
    
    CFRelease(pixEnc);
    CGDisplayModeRelease(cgmode);
    
#else
    NSDictionary *mode = (NSDictionary*)CGDisplayCurrentMode(kCGDirectMainDisplay);
    callback([[mode objectForKey:(id)kCGDisplayWidth] unsignedIntValue],
             [[mode objectForKey:(id)kCGDisplayHeight] unsignedIntValue],
             [[mode objectForKey:(id)kCGDisplayBitsPerPixel] unsignedIntValue]);
#endif
    
#if MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_6
    CFArrayRef modes = CGDisplayCopyAllDisplayModes(CGMainDisplayID(), 0);
    CFIndex count = CFArrayGetCount(modes);
    
    for (CFIndex index = 0; index < count; index++) {
        CGDisplayModeRef cgmode = (CGDisplayModeRef)CFArrayGetValueAtIndex(modes, index);
        bpp = 0;
        
        CFStringRef pixEnc = CGDisplayModeCopyPixelEncoding(cgmode);
        if(CFStringCompare(pixEnc, CFSTR(IO32BitDirectPixels), kCFCompareCaseInsensitive) == kCFCompareEqualTo)
            bpp = 32;
        else if(CFStringCompare(pixEnc, CFSTR(IO16BitDirectPixels), kCFCompareCaseInsensitive) == kCFCompareEqualTo)
            bpp = 16;
        else if(CFStringCompare(pixEnc, CFSTR(IO8BitIndexedPixels), kCFCompareCaseInsensitive) == kCFCompareEqualTo)
            bpp = 8;
        CFRelease(pixEnc);
        
        callback((unsigned int)CGDisplayModeGetWidth(cgmode),
                 (unsigned int)CGDisplayModeGetHeight(cgmode),
                 bpp);
    }
    CFRelease(modes);
    
#else
    NSArray* modes = (NSArray*) CGDisplayAvailableModes(CGMainDisplayID());
    unsigned long n = [modes count];
    
    for(unsigned long i = 0;  i < n;  i++) {
        NSDictionary *mode = [modes objectAtIndex:i];
        callback([[mode objectForKey:(id)kCGDisplayWidth] unsignedIntValue],
                 [[mode objectForKey:(id)kCGDisplayHeight] unsignedIntValue],
                 [[mode objectForKey:(id)kCGDisplayBitsPerPixel] unsignedIntValue]);
    }
#endif
}

+ (unsigned int)screenWidth {
    CGDisplayModeRef cgmode = CGDisplayCopyDisplayMode(kCGDirectMainDisplay);
    unsigned int result = (unsigned int)CGDisplayModeGetWidth(cgmode);
    CGDisplayModeRelease(cgmode);
    return result;
}

+ (unsigned int)screenHeight {
    CGDisplayModeRef cgmode = CGDisplayCopyDisplayMode(kCGDirectMainDisplay);
    unsigned int result = (unsigned int)CGDisplayModeGetHeight(cgmode);
    CGDisplayModeRelease(cgmode);
    return result;
}

#elif defined(MIST_OS_IOS)

+ (iOSDeviceType)getiOSDeviceType {
    if([MistObjc isiPad]) {
        if([MistObjc isRetina])
            return IDT_iPadRetina;
        return IDT_iPad;
    }
    else if([MistObjc isRetina])
        return IDT_Retina;
    else
        return IDT_Old;
}

+ (bool)isiPad {
    return UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad;
}

+ (bool)isRetina {
    return [[UIScreen mainScreen] respondsToSelector:@selector(displayLinkWithTarget:selector:)] &&
           ([UIScreen mainScreen].scale == 2.0);
}

+ (float)scaleFactor {
    return [MistObjc isRetina] ? 2.0 : 1.0;
}

+ (void)vabriteDevice {
    AudioServicesPlaySystemSound(kSystemSoundID_Vibrate);
}

+ (unsigned int)screenWidth {
    CGRect rect = [[UIScreen mainScreen] bounds];
    return rect.size.width;
}

+ (unsigned int)screenHeight {
    CGRect rect = [[UIScreen mainScreen] bounds];
    return rect.size.height;
}

+ (void)enumDesktopModes:(void (^)(int w, int h, int bpp))callback {
    callback([MistObjc screenWidth],
             [MistObjc screenHeight],
             32);
}

+ (NSString*)osVersion {
    NSString* systemVersion=[[UIDevice currentDevice] systemVersion];
    NSString* model=[[UIDevice currentDevice] model];
    
    return [NSString stringWithFormat:@"iOS %@ %@", systemVersion, model];
}
#endif



@end
