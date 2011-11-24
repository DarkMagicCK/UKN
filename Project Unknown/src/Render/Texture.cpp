//
//  Texture.cpp
//  Project Unknown
//
//  Created by Ruiwei Bu on 11/24/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#include "Texture.h"

namespace ukn {
    
    Texture::Texture(TextureType type):
    mType(type) {
        
    }
    
    Texture::~Texture() {
        
    }
    
    ElementFormat Texture::getFormat() const {
        // return mFormat;
        // currently ARGB8(32bit) textures only
        // todo with element formats
        return EF_ARGB8;
    }
    
} // namespace ukn
