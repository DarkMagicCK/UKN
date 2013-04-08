//
//  Font.h
//  Project Unknown
//
//  Created by Robert Bu on 11/23/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_Font_h
#define Project_Unknown_Font_h

#include "mist/Platform.h"
#include "mist/Uncopyable.h"
#include "mist/Serializer.h"
#include "mist/Color.h"
#include "mist/RectPlacement.h"

#include "UKN/PreDeclare.h"
#include "UKN/Renderable.h"
#include "UKN/Texture.h"
#include "UKN/SpriteBatch.h"
#include <vector>

#include <ft2build.h>
#include <freetype/freetype.h>

namespace ukn {
    
    /**
     * Font based on freetype for font loading and SpriteBatch for rendering
     **/
    
    enum FontAlignment {
        FA_Left,
        FA_Right,
        FA_Center,
        FA_XCenter,
    };
    
    enum FontStyle {
        FS_Shadow,
        FS_Stroke,
    };
    
    enum FontStyleProperty {
        FSP_ShadowXOffset,
        FSP_ShadowYOffset,
        FSP_StrokeWidth,
        FSP_KerningWidth,
        FSP_KerningHeight,
        FSP_LineWidth,
    };

    class UKN_API Font: Uncopyable, public virtual IConfigSerializable {
    public:
        static FontPtr Create(const UknString& name_or_path, uint32 size, bool bold = false, bool italic = false);

    public:
        Font();
        ~Font();
        
        /**
         * load from font config resource
         * should be .uknfnt
         **/
        bool loadFromFontFile(const ResourcePtr& resource, uint32 size, bool bold = false, bool italic = false);
        bool loadFromConfigFile(const ResourcePtr& resource);

        /**
         * set font style 
         **/
        void setStyle(FontStyle style, bool flag);
        void setStyleProperty(FontStyleProperty sp, int32 prop);
        
        void draw(const std::wstring& str, float x, float y, FontAlignment alignment, const Color& clr = color::White);
        
        float2 getStringDimensions(const std::wstring& str, float kw=0, float kh=0);
        bool isValid() const;
        
        /* pre cache a string */        
        void cacheString(const std::wstring& str);

    public:
        // IRenderable
        virtual void begin();
        virtual void end();

    public:
        // IConfigSerializable
        virtual bool deserialize(const ConfigParserPtr& cfg);
        virtual bool serialize(const ConfigParserPtr& cfg);
        
        struct TexturePlacement {
            mist::RectPlacement rect;
            TexturePtr texture;
        };
        friend class FTGlyph;
        TexturePlacement* getTexturePlacement(uint32 tid);
        
    private:
        friend class AssetManager;
        
        uint32 getGlyphByChar(uint16 chr);
        void flushCache();
        void setFTStyle(uint32 style);
        
        uint32 mFontSize;
        UknString mFontName;
        
        bool mEnableStroke;
        bool mEnableShadow;
        
        int32 mStrokeWidth;
        int32 mShadowXOffset;
        int32 mShadowYOffset;

        int32 mKerningWidth;
        int32 mKerningHeight;
        int32 mLineWidth;
        bool mBegan;

        float mCharRotation;
        
        SpriteBatchPtr mSpriteBatch;
      
        struct FTLibrary;
        struct FTFace;
        struct FTGlyph;
        
        friend struct FTGlyph;
        
        ScopedPtr<FTFace> mFace;

		struct FTGlyph {
			FTGlyph();
			~FTGlyph();
			bool cached;
        
			void resetFontSize(uint32 newsize);
			bool cache(uint32 index, Font& font, uint32* texdata, TexturePlacement& placement);
        
			FT_Face* face;
        
			uint32 size;
			uint32 top;
			uint32 left;
			uint32 texw;
			uint32 texh;
            
            mist::Rectangle rect;
            uint32 texture_id;
		};
        std::vector<FTGlyph> mGlyphs;
        
        TexturePlacement* appendTexturePlacement();
        uint32 getLastTexturePlacement() const;

        std::vector<TexturePlacement> mTextures;
    };
    
} // namespace ukn  

#endif
