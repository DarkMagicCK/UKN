//
//  Texture.h
//  Project Unknown
//
//  Created by Robert Bu on 11/23/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_Texture_h
#define Project_Unknown_Texture_h

#include "Platform.h"
#include "PreDeclare.h"
#include "GraphicSettings.h"

namespace ukn {
    
    /**
     * Abstract texture class
     * Implementations differs on plugins
     **/
    class UKN_API Texture {
    public:
        enum TextureType {
            // Null Texture
            TT_Null = 0,
            
            // 2d texture
            TT_Texture2D,
            
            // 1d texture
            TT_Texture1D,
            // 3d texture
            TT_Texture3D,
            // cubemap
            TT_TextureCubeMap,
        };
        
        enum MapperAccess {
            MA_ReadOnly,
            MA_WriteOnly,
            MA_ReadWrite,
        };
        
        static TexturePtr NullObject();
        
        explicit Texture(TextureType type);
        virtual ~Texture();
        
        ElementFormat getFormat() const;
        
        virtual uint32 getWidth(uint32 level = 0) = 0;
        virtual uint32 getHeight(uint32 level = 0) = 0;
        
    protected:
        TextureType mType;
        uint32 mNumMipmaps;
        ElementFormat mFormat;
    };
    
} // namespace ukn

#endif
