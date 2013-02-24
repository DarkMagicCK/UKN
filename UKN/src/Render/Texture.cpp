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
        Texture(TT_Null, 1, 0, 0) {
            
        }

        virtual ~NullTexture() { }
        
        uint32 width(uint32 level) const {
            return 0;
        }
        
        uint32 height(uint32 level) const {
            return 0;
        }

        uint32 depth(uint32 level) const {
            return 0;
        }
        
        uintPtr getTextureId() const {
            return 0;
        }

        void generateMipmaps() {

        }

        void* map(uint32 level) { return 0; }
        void unmap() { }
        void updateData(const mist::Rectangle& rect, const uint8* data, uint32 level) { }
    };
    
    TexturePtr Texture::NullObject() {
        static TexturePtr static_ptr = MakeSharedPtr<NullTexture>();
        return static_ptr;
    }
    
    Texture::Texture(TextureType type, uint32 sample_count, uint32 sample_quality, uint32 _reserved):
    mType(type),
    mFormat(EF_Unknown),
    mNumMipmaps(1),
    mSampleCount(sample_count),
    mSampleQuality(sample_quality),
    mArraySize(1),
    mReserverd(_reserved) {
        
    }
    
    Texture::~Texture() {
        
    }
    
    TextureType Texture::type() const {
        return mType;
    }

    ElementFormat Texture::format() const {
        return mFormat;
    }

    uint32 Texture::numMipmaps() const {
        return mNumMipmaps;
    }

    uint32 Texture::sampleQuality() const {
        return mSampleQuality;
    }

    uint32 Texture::sampleCount() const {
        return mSampleCount;
    }
    
    
} // namespace ukn
