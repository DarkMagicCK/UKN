//
//  GLTexture.h
//  Project Unknown
//
//  Created by Robert Bu on 12/2/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_GLTexture_h
#define Project_Unknown_GLTexture_h

#include "UKN/Texture.h"

namespace ukn {
    
    class GLTexture2D: public Texture {
    public:  
        GLTexture2D();
        ~GLTexture2D();
        
        bool load(const ResourcePtr& rsrc, bool createMipmap);
        bool create(uint32 w, uint32 h, uint32 mipmas, ElementFormat format, const uint8* initialData);
        
        uint32 getWidth(uint32 level = 0) const override;
        uint32 getHeight(uint32 level = 0) const override;
        
        uintPtr getTextureId() const override;
        
        void* map(uint32 level = 0) override;
        void unmap() override;

    private:
        uintPtr mTextureId;

        void* mTextureData;
        uint32 mMappedLevel;

        uint32 mWidth;
        uint32 mHeight;
        uint32 mOrigWidth;
        uint32 mOrigHeight;
    };
    
} // namespace ukn


#endif