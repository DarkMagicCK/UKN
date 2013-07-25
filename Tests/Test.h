//
//  opengl.h
//  Project Unknown
//
//  Created by Robert Bu on 5/13/13.
//  Copyright (c) 2013 heizi. All rights reserved.
//

#ifndef __Project_Unknown__opengl__
#define __Project_Unknown__opengl__

#include <iostream>

#include "mist/Resource.h"

inline void SetupTestMediaResourceDirectories() {
    mist::ResourceLoader& loader = mist::ResourceLoader::Instance();
    loader.addPath(L"../../Resources/Media/");
    loader.addPath(L"../../Resources/Shader/");
}

#endif /* defined(__Project_Unknown__opengl__) */
