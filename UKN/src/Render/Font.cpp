//
//  Font.cpp
//  Project Unknown
//
//  Created by Robert Bu on 12/9/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#include "UKN/Font.h"
#include "UKN/RenderBuffer.h"
#include "UKN/GraphicDevice.h"
#include "UKN/GraphicFactory.h"
#include "UKN/MathUtil.h"
#include "UKN/Stream.h"
#include "UKN/Resource.h"
#include "UKN/StringUtil.h"
#include "UKN/Context.h"
#include "UKN/Logger.h"
#include "UKN/SpriteBatch.h"
#include "UKN/Texture.h"
#include "UKN/ConfigParser.h"
#include "UKN/FileUtil.h"

#include <ft2build.h>
#include <freetype/freetype.h>

#ifdef UKN_OS_WINDOWS
#pragma comment(lib, "freetype244ST.lib")
#endif

namespace ukn {
    
    struct Font::FTLibrary {
        static FTLibrary& Instance() {
            static Font::FTLibrary* lib = 0;
            if(!lib)
                lib = new Font::FTLibrary;
            return *lib;
        }
        
        FTLibrary() {
            if(FT_Init_FreeType(&library)) {
                log_error("ukn::Font::FTLibrary: error initializing freetype, font rendering will not available");
            }
        }
        ~FTLibrary() {
            if(library)
                FT_Done_FreeType(library);
        }
        
        FT_Library library;
    };
    
    struct Font::FTFace {
        FTFace(): face(0) { }
        
        ~FTFace() {
            if(face) {
                FT_Done_Face(face);
            }
        }
                
        FT_Face     face;
        StreamPtr   font_data;

        bool load(const ResourcePtr& resource) {
            if(!Font::FTLibrary::Instance().library)
                return false;
            
            if(face) {
                FT_Done_Face(face);
            }
            
            if(resource && resource->getResourceStream()) {
                font_data = resource->getResourceStream()->readIntoMemory();
                if(FT_New_Memory_Face(Font::FTLibrary::Instance().library, 
                                      static_cast<MemoryStream*>(font_data.get())->data(), 
                                      font_data->size(),
                                      0,
                                      &face)) {
                    return false;
                }
                FT_Select_Charmap(face, FT_ENCODING_UNICODE);
                return true;
            } else {
                if(FT_New_Face(Font::FTLibrary::Instance().library,
                               String::WStringToString(resource->getName()).c_str(),
                               0,
                               &face)) {
                    return false;
                }
                return true;
            }
            return false;
        }
    };
    
    struct Font::FTGlyph {
        FTGlyph():
        cached(false),
        face(0),
        size(0),
        top(0),
        left(0),
        texw(0),
        texh(0),
        imgw(0),
        imgh(0),
        texture(TexturePtr()) { 
        }
        
        ~FTGlyph() {
        }
        
        bool cached;
        
        void resetFontSize(uint32 newsize) {
            if(size != newsize) {
                size = newsize; 
                cached = false;
            }
        }
        
        void cache(uint32 index, Font& font) {
            if(cached)
                return;
            
            FT_Set_Pixel_Sizes(*face, 0, size);
            if(!FT_Load_Glyph(*face, index, FT_LOAD_DEFAULT)) {
                FT_GlyphSlot glyph = (*face)->glyph;
                FT_Bitmap bits;
                if(glyph->format == ft_glyph_format_outline) {
                    if(!FT_Render_Glyph(glyph, FT_RENDER_MODE_NORMAL)) {
                        bits = glyph->bitmap;
                        uint8* pt = bits.buffer;
                        
                        top = glyph->bitmap_top;
                        left = glyph->bitmap_left;
                        imgw = 1;
                        imgh = 1;
                        texw = bits.width;
                        texh = bits.rows;
                        
                        imgw = next_pow_of_2(texw);
                        imgh = next_pow_of_2(texh);
                        imgw > imgh ? imgh = imgw : imgw = imgh;
                        
                        if(imgw == 0 || imgh == 0)
                            return;
                        
                        uint32* texd = (uint32*)ukn_malloc(imgw * imgh * 4);
                        memset(texd, 0, imgw * imgh * 4);
                        uint32* texp = texd;
                        bool cflag = true;
                        for(int i = 0; i < bits.rows; ++i) {
                            uint32* rowp = texp;
                            for(int j=0; j < bits.width; ++j) {
                                if(*pt) {
                                    if(cflag) {
                                        *rowp = *pt;
                                        *rowp *= 0x01010101;
                                    } else {
                                        *rowp = *pt << 24;
                                        *rowp |= 0xffffffff;
                                    }
                                } else {
                                    *rowp = 0;
                                }
                                pt++;
                                rowp++;
                            }
                            texp += imgw;
                        }
                        
                        texture = Context::Instance().getGraphicFactory().create2DTexture(imgw, imgh, 0, EF_ARGB8, (uint8*)texd);
                        ukn_free(texd);
                        cached = true;
                    }
                }
            }
        }
        
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
    
    struct Font::StringData {
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
    
    Font::Font():
    mFontSize(0),
    mEnableStroke(false),
    mEnableShadow(false),
    mStrokeWidth(0),
    mShadowXOffset(0),
    mShadowYOffset(0),
    mFace(new Font::FTFace) {
        mSpriteBatch = Context::Instance().getGraphicFactory().createSpriteBatch();
        if(!mSpriteBatch)
            log_error("ukn::Font::Font Error creating sprite batch for font rendering");
    }
    
    Font::~Font() {
        
    }
    
    bool Font::loadFromResource(const ResourcePtr& resource) {
        if(resource->getName().find(L".xml") == String::npos) {
            bool result = mFace->load(resource);
            if(result) {
                mGlyphs.resize(mFace->face->num_glyphs);
                
                for(int i = 0; i < mFace->face->num_glyphs; ++i) {
                    mGlyphs[i].size = mFontSize;
                    mGlyphs[i].face = &mFace->face;
                }
                
                mEnableShadow = false;
                mEnableStroke = false;
                
                mShadowXOffset = 0;
                mShadowYOffset = 0;
                
                mStrokeWidth = 0;
                
                mFontSize = 14;
                mFontName = resource->getName();
            }
            return result;
        }
        
        ConfigParserPtr config = ConfigParser::MakeParser(resource);
        
        if(config)
            return deserialize(config);
        else {
            log_error(L"ukn::Font::loadFromResource: invalid resource ptr, with resource name" + resource->getName());
            
                    }
        return false;
    }
    
    bool Font::deserialize(const ConfigParserPtr& config) {
        mGlyphs.clear();

        if(config && config->toNode(L"font")) {            
            ukn_string font_name = config->getString(L"name");
            
            if(!font_name.empty()) {
                ukn_string fullFontPath = Path::CheckAndGetFontPath(font_name);
                
                if(fullFontPath.empty()) {
                    log_error(L"ukn::Font::deserialize: error finding font name " + font_name);
                    return false;
                }
                ResourcePtr fontResource = ResourceLoader::Instance().loadResource(fullFontPath);
                if(fontResource && fontResource->getResourceStream()) {
                    bool result = mFace->load(fontResource);
                    
                    if(result) {
                        mGlyphs.resize(mFace->face->num_glyphs);
                        
                        for(int i = 0; i < mFace->face->num_glyphs; ++i) {
                            mGlyphs[i].size = mFontSize;
                            mGlyphs[i].face = &mFace->face;
                        }
                    }
                }
            }
            
            mEnableShadow = config->getBool(L"shadow", false);
            mEnableStroke = config->getBool(L"stroke", false);
            
            mShadowXOffset = config->getInt(L"shadow_offset_x", 0);
            mShadowYOffset = config->getInt(L"shadow_offset_y", 0);
            
            mStrokeWidth = config->getInt(L"stroke_width", 0);
            
            mFontSize = config->getInt(L"size", 14);
            mFontName = font_name;
            
            config->toParent();
            
            return true;
        }
        return false;
    }
    
    bool Font::serialize(const ConfigParserPtr& cfg) {
        if(cfg && !mGlyphs.empty()) {
            cfg->beginNode(L"font");
            cfg->setString(L"name", mFontName);
            cfg->setBool(L"shadow", mEnableShadow);
            cfg->setBool(L"stroke", mEnableStroke);
            cfg->setInt(L"shadow_offset_x", mShadowXOffset);
            cfg->setInt(L"shadow_offset_y", mShadowYOffset);
            cfg->setInt(L"stroke_width", mStrokeWidth);
            cfg->setInt(L"size", mFontSize);
            cfg->endNode();
            
            return true;
        } else {
            log_error(L"ukn::Font::serialize: unable to serialize font "+mFontName);
        }
        return false;
    }
    
    void Font::flushCache() {
        mGlyphs.clear();
    }
    
    void Font::setFTStyle(uint32 style) {
        mFace->face->style_flags = mFace->face->style_flags | style;
     //   flushCache();
    }
    
    void Font::setStyle(ukn::FontStyle style, bool flag) {
        switch(style) {
            case FS_Shadow: mEnableShadow = flag; break;
            case FS_Stroke: mEnableStroke = flag; break;
            case FS_Bold:
                setFTStyle(FT_STYLE_FLAG_BOLD);
                break;
            case FS_Italic:
                setFTStyle(FT_STYLE_FLAG_ITALIC);
                break;
        }
    }
    
    void Font::setStyleProperty(FontStyleProperty sp, int32 prop) {
        switch(sp) {
            case FSP_Stroke_Width: mStrokeWidth = prop; break;
            case FSP_Shadow_XOffset: mShadowXOffset = prop; break;
            case FSP_Shadow_YOffset: mShadowYOffset = prop; break;
            case FSP_Size: mFontSize = prop; break;
        }
    }
    
    bool Font::isValid() const {
        return !mGlyphs.empty();
    }
    
    void Font::doRender(const StringData& data) {
        float x = data.x;
        float y = data.y;
        
        std::wstring::const_iterator it = data.string_to_render.begin();
        while(it != data.string_to_render.end() && *it != 0) {
            if(*it != L'\n') {
                if(*it == L' ') {
                    x += mFontSize;
                    
                } else {
                    uint32 gidx = getGlyphByChar(*it);
                    
                    if(gidx > 0 && gidx < mGlyphs.size()) {
                        FTGlyph& glyph = mGlyphs[gidx-1];
                        
                        mSpriteBatch->draw(glyph.texture, 
                                           Vector2(x+glyph.left, 
                                                   y+glyph.size-glyph.top), 
                                           data.char_rot, 
                                           data.clr);
                        
                        x += glyph.texw + glyph.left + data.kerning_width;
                    }
                    
                    if(data.line_width != 0.f && (x - data.x) > data.line_width) {
                        y += mFontSize + data.kerning_height;
                        x = data.x;
                    }
               }
            } else {
                y += mFontSize + data.kerning_height;
                x = data.x;
            }
            
            ++it;
        }
    }
    
    void Font::render() {
        if(!mSpriteBatch)
            return;
        if(mGlyphs.size() == 0)
            return;
    
        onRenderBegin();
                
        std::vector<StringData>::const_iterator it = mRenderQueue.begin();
        while(it != mRenderQueue.end()) {
            doRender(*it);
            
            ++it;
        }
                
        onRenderEnd();
    }
    
    uint32 Font::getGlyphByChar(uint16 chr) {
        uint32 idx = FT_Get_Char_Index(mFace->face, chr);
        if(idx == 0)
            return idx;
        
        FTGlyph& glyph = mGlyphs[idx-1];
		if((idx && !glyph.cached) || glyph.size != mFontSize) {
			glyph.resetFontSize(mFontSize);
			glyph.cache(idx, *this);
		}
        return idx;
    }
    
    void Font::draw(const wchar_t* str, float x, float y, FontAlignment alignment, const Color& clr) {
        // in windows, gbk -> utf8
        Font::StringData data(str, x, y, alignment);
        data.clr = clr;
        
        mRenderQueue.push_back(data);
    }
    
    const String& Font::getName() const {
        return mFontName;
    }
    
    float2 Font::getStringDimensions(const wchar_t* str, float kw, float kh) {
        float2 dim(0.f, 0.f);
        
        float tmpw = 0.f;
        uint32 size = (uint32)wcslen(str);
        const wchar_t* ustr = str;
        while(ustr && *ustr) {
            if(*ustr != L'\n') {
                tmpw += mGlyphs[getGlyphByChar(*ustr)].imgw + kw;
                if(dim[0] < tmpw) 
                    dim[0] = tmpw;
            }
            else {
                dim[1] += mFontSize + kh;
                tmpw = 0.f;
            }
            
            ustr++;
        }
        return dim;
    }
    
    void Font::onRenderBegin() {
        if(mSpriteBatch) {
            mSpriteBatch->begin();
        }
    }
    
    void Font::onRenderEnd() {
        mRenderQueue.clear();
        
        if(mSpriteBatch) {
            mSpriteBatch->end();
        }
    }
    
    Box Font::getBound() const {
        return mSpriteBatch->getBound();
    }
    
    RenderBufferPtr Font::getRenderBuffer() const {
        return mSpriteBatch->getRenderBuffer();
    }
    
    
} // namespace ukn
