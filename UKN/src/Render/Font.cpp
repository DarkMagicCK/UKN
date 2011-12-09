//
//  Font.cpp
//  Project Unknown
//
//  Created by Ruiwei Bu on 12/9/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#include "UKN/Font.h"
#include "UKN/RenderBuffer.h"
#include "UKN/GraphicDevice.h"
#include "UKN/GraphicFactory.h"
#include "UKN/MathUtil.h"
#include "UKN/Stream.h"
#include "UKN/Resource.h"

#include <ft2build.h>

namespace ukn {
    
    struct Font::StringData {
        ukn_wstring string_to_render;
        float x;
        float y;
        FontAlignment alignment;
        
        StringData() { }
        
        StringData(const ukn_wstring& str, float _x, float _y, FontAlignment align):
        string_to_render(str),
        x(_x),
        y(_y),
        alignment(align) {
            
        }
    };
    
    Font::Font():
    mFontSize(0),
    mFontData(ResourcePtr()),
    mIsBold(false),
    mIsItalic(false),
    mEnableStroke(false),
    mEnableShadow(false),
    mStrokeWidth(0),
    mShadowXOffset(0),
    mShadowYOffset(0),
    mBoundingBox(Rectangle()),
    mRenderBuffer(RenderBufferPtr()){
        
    }
    
    Font::Font(const ukn_wstring& font_name, uint32 font_size) {
        
    }

    
    Font::~Font() {
        
    }
    
    bool Font::load(const ukn_wstring& name, uint32 size) {
        
        return false;
    }
    
    void Font::setStyle(ukn::FontStyle style, bool flag) {
        
    }
    
    void Font::setStyleProperty(FontStyle style, FontStyleProperty prop) {
        
    }
    
    void Font::render() {
        
    }
    
    const ukn_string& Font::getName() const {
        static ukn_string name("freetype_font");
        return name;
    }
    
    Rectangle Font::getBound() const {
        return mBoundingBox;
    }
    
    RenderBufferPtr Font::getRenderBuffer() const {
        return mRenderBuffer;
    }
    
    
} // namespace ukn
