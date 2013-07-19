//
//  MistObjc.h
//  Mist
//
//  Created by Robert Bu on 1/27/13.
//  Copyright (c) 2013 Robert Bu. All rights reserved.
//

/* 
 iOS And OSX Objc Wrappers
 used in mist_apple.mm
 */

#import <Foundation/Foundation.h>

#if defined(MIST_OS_IOS)

enum iOSDeviceType {
    IDT_Old,    // "old" devices such as ipod touch 3rd gen or iphone 3gs, resolution 480*320
    IDT_Retina, // newer devices with a retina display, resolution 960 * 640
    IDT_iPad,   // iPad with 1024 * 768 resolution
    IDT_iPadRetina, // Retina ipad with 2048 * 1536 resolution
};


#endif

@interface MistObjc : NSObject

+ (NSString*)applicationPath;
+ (NSString*)documentsPath;
+ (NSString*)resourcePath;

+ (NSString*)resourcePathForResource:(NSString*)resource;

+ (NSString*)osVersion;

+ (void)enumDesktopModes:(void (^)(unsigned int w, unsigned int h, int bpp))callback;

+ (unsigned int)screenWidth;
+ (unsigned int)screenHeight;

+ (bool)fileExists:(NSString*)file;
+ (bool)copyFile:(NSString*)src dst:(NSString*)dst;
+ (bool)moveFile:(NSString*)src dst:(NSString*)dst;
+ (bool)deleteFile:(NSString*)src dst:(NSString*)dst;
+ (bool)createDirectory:(NSString*)dir;

#ifdef MIST_OS_OSX
+ (int) getOSXVersion;

#elif defined(MIST_OS_IOS)

+ (iOSDeviceType)getiOSDeviceType();
+ (bool)isiPad;
+ (bool)isRetina;

+ (float)scaleFactor;

+ (void)vabriteDevice;

#endif

@end
