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

#include "UKN/PreDeclare.h"
#include "UKN/Renderable.h"

#include <vector>

#include <ft2build.h>
#include <freetype/freetype.h>

namespace ukn {
    
    /**
     * Font based on freetype for font loading and SpriteBatch for rendering
     **/
    
    enum FontAlignment {
        FA_Left     = 1UL,
        FA_Right    = 1UL << 1,
        FA_Center   = 1UL << 2,
        
        FA_XCenter  = 1UL << 3
    };
    
    enum FontStyle {
        FS_Shadow,
        FS_Stroke,
        FS_Italic,
        FS_Bold,
    };
    
    enum FontStyleProperty {
        FSP_Shadow_XOffset,
        FSP_Shadow_YOffset,
        FSP_Stroke_Width,
        FSP_Size,
    };

    class UKN_API Font: Uncopyable, public virtual IConfigSerializable {
    public:
        Font();
        ~Font();
           
        /**
         * load from font config resource
         * should be .uknfnt
         **/
        bool loadFromResource(const ResourcePtr& resource);
        
        /**
         * set font style 
         **/
        void setStyle(FontStyle style, bool flag);
        void setStyleProperty(FontStyleProperty sp, int32 prop);
        
        void draw(const wchar_t* str, float x, float y, FontAlignment alignment, const Color& clr = color::White);
        
        float2 getStringDimensions(const wchar_t* str, float kw=0, float kh=0);
        bool isValid() const;
        
    public:
        // IRenderable
        virtual void render();
                       
    public:
        // IConfigSerializable
        virtual bool deserialize(const ConfigParserPtr& cfg);
        virtual bool serialize(const ConfigParserPtr& cfg);
        
    private:        
        void onRenderBegin();
        void onRenderEnd();

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
        
        SpriteBatchPtr mSpriteBatch;
        
        
		struct UKN_API StringData {
			std::wstring string_to_render;
			float x;
			float y;
			float char_rot;
			float kerning_width;
			float kerning_height;
			float line_width;
			FontAlignment alignment;
			Color clr;
        
			StringData():
			x(0),
			y(0),
			char_rot(0),
			kerning_width(0),
			kerning_height(0),
			line_width(0),
			alignment(FA_Left) {
        
			}
        
			StringData(const wchar_t* str, float _x, float _y, FontAlignment align):
			x(_x),
			y(_y),
			char_rot(0),
			kerning_width(0),
			kerning_height(0),
			line_width(0),
			alignment(align) {
				string_to_render = str;
			}
		};
        std::vector<StringData> mRenderQueue;
        
        void doRender(const StringData& data);
        
        struct FTLibrary;
        struct FTFace;
        struct FTGlyph;
        
        friend struct FTGlyph;
        
        ScopedPtr<FTFace> mFace;

		struct Font::FTGlyph {
			FTGlyph();
			~FTGlyph();
			bool cached;
        
			void resetFontSize(uint32 newsize);
			void cache(uint32 index, Font& font);
        
			FT_Face* face;
        
			uint32 size;
			uint32 top;
			uint32 left;
			uint32 texw;
			uint32 texh;
			uint32 imgw;
			uint32 imgh;
        
			TexturePtr texture;
		};
        std::vector<FTGlyph> mGlyphs;
    };
    
} // namespace ukn  

#endif
