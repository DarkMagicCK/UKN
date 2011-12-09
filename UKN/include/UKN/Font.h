//
//  Font.h
//  Project Unknown
//
//  Created by Robert Bu on 11/23/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_Font_h
#define Project_Unknown_Font_h

#include "UKN/Platform.h"
#include "UKN/Uncopyable.h"
#include "UKN/PreDeclare.h"
#include "UKN/Renderable.h"

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
        FS_Bold,
        FS_Italic,
        FS_Shadow,
        FS_Stroke,
    };
    
    enum FontStyleProperty {
        FSP_Shadow_XOffset,
        FSP_Shadow_YOffset,
        FSP_Stroke_Width,
    };
    
    class Font: Uncopyable, public Renderable {
    public:
        Font();
        explicit Font(const ukn_wstring& font_name, uint32 font_size);
        ~Font();
        
        bool load(const ukn_wstring& name, uint32 size);
        
        /**
         * set font style 
         **/
        void setStyle(FontStyle style, bool flag);
        void setStyleProperty(FontStyle style, FontStyleProperty prop);
        
        void draw(const ukn_wstring& str, float x, float y, FontAlignment alignment);
        
        // inherited from Renderable
        void render();
        
        const ukn_string& getName() const = 0;
        
        Rectangle getBound() const = 0;
        RenderBufferPtr getRenderBuffer() const = 0;
        
    private:
        uint32 mFontSize;
        ResourcePtr mFontData;
        
        bool mIsBold;
        bool mIsItalic;
        bool mEnableStroke;
        bool mEnableShadow;
        
        int32 mStrokeWidth;
        int32 mShadowXOffset;
        int32 mShadowYOffset;
        
        Rectangle mBoundingBox;
        RenderBufferPtr mRenderBuffer;
        
        struct StringData;
        Array<StringData> mRenderQueue;
    };
    
} // namespace ukn  

#endif
