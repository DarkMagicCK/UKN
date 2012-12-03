//
//  Texture.h
//  Mist
//
//  Created by Robert Bu on 11/23/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef MISTTexture_h
#define MISTTexture_h

#include "mist/Platform.h"
#include "ukn/PreDeclare.h"
#include "ukn/GraphicSettings.h"

namespace ukn {
    
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
    
    /**
     * Abstract texture class
     **/
    class MIST_API Texture {
    public:
        static TexturePtr NullObject();
        
        explicit Texture(TextureType type);
        virtual ~Texture();
        
        ElementFormat getFormat() const;
        TextureType   getType() const;
        
        virtual uint32 getWidth(uint32 level = 0) const = 0;
        virtual uint32 getHeight(uint32 level = 0) const = 0;
        
        virtual uintPtr getTextureId() const = 0;
                
    protected:
        TextureType mType;
        uint32 mNumMipmaps;
        ElementFormat mFormat;
    };
    
} // namespace ukn

#endif
