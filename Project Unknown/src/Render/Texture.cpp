//
//  Texture.cpp
//  Project Unknown
//
//  Created by Ruiwei Bu on 11/24/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#include "UKN/Texture.h"

namespace ukn {
    
    class NullTexture: public Texture {
    public:
        NullTexture():
        Texture(TT_Null) {
            
        }
        
        uint32 getWidth(uint32 level) {
            return 0;
        }
        
        uint32 getHeight(uint32 level) {
            return 0;
        }
    };
    
    TexturePtr Texture::NullObject() {
        static TexturePtr static_ptr = MakeSharedPtr<NullTexture>();
        return static_ptr;
    }
    
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
