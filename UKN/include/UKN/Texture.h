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
        // Cubemap
        TT_TextureCube,
    };
    
    /* 
        notice the restrictions 
    */
    enum TextureMapAccess {
        TMA_ReadOnly,
        TMA_WriteOnly,
        TMA_ReadWrite,
    };

    enum TextureCubeFace {
        TCF_PositionX, // <1, 0, 0>
        TCF_NegativeX, // <-1, 0, 0>
        TCF_PositionY, // <0, 1, 0>
        TCF_NegativeY, // <0, -1, 0>
        TCP_PositionZ, // <0, 0, 1>
        TCP_NegativeZ, // <0, 0, -1>
    };
    
    /*
    new texture 
    */
    class UKN_API Texture {
    public:
        static TexturePtr NullObject();

        /* 
        default:
         sample_count = 1,
         sample_quality = 0,
         format = EF_Unknown,
         array_size = 1,
         num_mipmaps = 1
         */
        Texture(TextureType type, uint32 sample_count, uint32 sample_quality, uint32 _reserved);
        virtual ~Texture();

        virtual uint32 width(uint32 level = 0) const = 0;
        virtual uint32 height(uint32 level = 0) const = 0;
        /* 3d texture only */
        virtual uint32 depth(uint32 level = 0) const = 0;
        
        virtual void generateMipmaps() = 0;

        /* native handle */
        virtual uintPtr getTextureId() const = 0;

        ElementFormat   format() const;
        TextureType     type() const;
        uint32          numMipmaps() const;
        uint32          sampleQuality() const;
        uint32          sampleCount() const;

        /* texture mapping todo */

    protected:
        TextureType mType;
        ElementFormat mFormat;
        
        uint32 mNumMipmaps;
        /* glGenBuffer instead texture to support multisampling? */
        uint32 mSampleCount;
        uint32 mSampleQuality;
        /* to do, not used now, GL_TEXTURE2D_ARRAY in 3.x */
        uint32 mArraySize; 
        uint32 mReserverd;
    };
    
} // namespace ukn

#endif
