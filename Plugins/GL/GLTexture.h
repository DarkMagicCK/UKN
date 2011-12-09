//
//  GLTexture.h
//  Project Unknown
//
//  Created by Ruiwei Bu on 12/2/11.
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
        
        bool load(const ukn_wstring& name, bool createMipmap);
        bool create(uint32 w, uint32 h, uint32 mipmas, ElementFormat format, const uint8* initialData);
        
        uint32 getWidth(uint32 level = 0) const;
        uint32 getHeight(uint32 level = 0) const;
        
        uintPtr getTextureId() const;
        
    private:
        uintPtr mTextureId;
        
        uint32 mWidth;
        uint32 mHeight;
        uint32 mOrigWidth;
        uint32 mOrigHeight;
    };
    
} // namespace ukn


#endif