//
//  Texture.cpp
//  Project Unknown
//
//  Created by Robert Bu on 11/24/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#include "UKN/Texture.h"

namespace ukn {
        
    class NullTexture: public Texture {
    public:
        NullTexture():
        Texture(TT_Null) {
            
        }
        
        uint32 getWidth(uint32 level) const {
            return 0;
        }
        
        uint32 getHeight(uint32 level) const {
            return 0;
        }
        
        uintPtr getTextureId() const {
            return 0;
        }

        void* map(uint32 pass) { return 0; }
        void unmap() { }

    };
    
    TexturePtr Texture::NullObject() {
        static TexturePtr static_ptr = MakeSharedPtr<NullTexture>();
        return static_ptr;
    }
    
    Texture::Texture(TextureType type):
    mType(type) {
        
    }
    
    TextureType Texture::getType() const {
        return mType;
    }
    
    Texture::~Texture() {
        
    }
    
    ElementFormat Texture::getFormat() const {
        // return mFormat;
        // currently RGBA8(32bit) textures only
        // todo with element formats
        return EF_RGBA8;
    }
    
} // namespace ukn
