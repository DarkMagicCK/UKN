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
#include "UKN/StringUtil.h"
#include "UKN/Context.h"
#include "UKN/Logger.h"
#include "UKN/SpriteBatch.h"
#include "UKN/Texture.h"
#include "UKN/ConfigParser.h"
#include "UKN/FileUtil.h"

#include <ft2build.h>
#include <freetype/freetype.h>

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
                font_data = stream_to_memory_stream(resource->getResourceStream());
                if(FT_New_Memory_Face(Font::FTLibrary::Instance().library, 
                                      static_cast<MemoryStream*>(font_data.get())->get(), 
                                      font_data->getSize(),
                                      0,
                                      &face)) {
                    return false;
                }
                return true;
            } else {
                if(FT_New_Face(Font::FTLibrary::Instance().library,
                               wstring_to_string(resource->getName()).c_str(),
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
            size = newsize;
            cached = false;
        }
        
        void cache(uint32 index, Font& font) {
            if(cached)
                return;
            
            FT_Set_Pixel_Sizes(*face, 0, size);
            if(!FT_Load_Glyph(*face, index, FT_LOAD_FORCE_AUTOHINT)) {
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
                        
                        imgw = to2spow(texw);
                        imgh = to2spow(texh);
                        imgw > imgh ? imgh = imgw : imgw = imgh;
                        
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
        ukn_wstring string_to_render;
        float x;
        float y;
        float char_rot;
        float kerning_width;
        float kerning_height;
        float line_width;
        FontAlignment alignment;
        
        StringData():
        string_to_render(ukn_wstring()),
        x(0),
        y(0),
        char_rot(0),
        kerning_width(0),
        kerning_height(0),
        line_width(0),
        alignment(FA_Left) {
        
        }
        
        StringData(const ukn_wstring& str, float _x, float _y, FontAlignment align):
        string_to_render(str),
        x(_x),
        y(_y),
        char_rot(0),
        kerning_width(0),
        kerning_height(0),
        line_width(0),
        alignment(align) {
            
        }
    };
    
    Font::Font():
    mFontSize(0),
    mIsBold(false),
    mIsItalic(false),
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
        mGlyphs.clear();
        
        ConfigParserPtr config = MakeConfigParser(resource);
        
        if(config && config->toNode("/font")) {            
            ukn_string font_name = config->getString("name");
            
            if(!font_name.empty()) {
                ukn_wstring fullFontPath = check_and_get_font_path(string_to_wstring(font_name));
                
                if(fullFontPath.empty()) {
                    log_error("ukn::Font::loadFromResource: error finding font name " + font_name);
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
            
            mIsBold = config->getBool("bold", false);
            mIsItalic = config->getBool("italic", false);
            mEnableShadow = config->getBool("shadow", false);
            mEnableStroke = config->getBool("stroke", false);
            
            mShadowXOffset = config->getInt("shadow_offset_x", 0);
            mShadowYOffset = config->getInt("shadow_offset_y", 0);
            
            mStrokeWidth = config->getInt("stroke_width", 0);
            
            mFontSize = config->getInt("size", 14);
            
            return true;
        }
        return false;
    }
    
    void Font::setStyle(ukn::FontStyle style, bool flag) {
        switch(style) {
            case FS_Bold: mIsBold = flag; break;
            case FS_Italic: mIsItalic = flag; break;
            case FS_Shadow: mEnableShadow = flag; break;
            case FS_Stroke: mEnableStroke = flag; break;
        }
    }
    
    void Font::setStyleProperty(FontStyleProperty sp, int32 prop) {
        switch(sp) {
            case FSP_Stroke_Width: mStrokeWidth = prop; break;
            case FSP_Shadow_XOffset: mShadowXOffset = prop; break;
            case FSP_Shadow_YOffset: mShadowYOffset = prop; break;
        }
        
    }
    
    bool Font::isValid() const {
        return !mGlyphs.empty();
    }
    
    void Font::doRender(const StringData& data) {
        float x = data.x;
        float y = data.y;
        
        ukn_wstring::const_iterator it = data.string_to_render.begin();
        while(it != data.string_to_render.end()) {
            if(*it != L'\n') {
                uint32 gidx = getGlyphByChar(*it);
                
                if(gidx > 0 && gidx < mGlyphs.size()) {
                    FTGlyph& glyph = mGlyphs[gidx-1];

                    mSpriteBatch->draw(glyph.texture, x+glyph.left, y+glyph.size-glyph.top, data.char_rot);
                    
                    x += mGlyphs[gidx-1].texw + mGlyphs[gidx-1].left + data.kerning_width;
                }
                
                if(data.line_width != 0.f && (x - data.x) > data.line_width) {
                    y += mFontSize + data.kerning_height;
                    x = data.x;
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
        
        Array<StringData>::const_iterator it = mRenderQueue.begin();
        while(it != mRenderQueue.end()) {
            doRender(*it);
            
            ++it;
        }
        
        mSpriteBatch->render();
    }
    
    uint32 Font::getGlyphByChar(wchar_t chr) {
        uint32 idx = FT_Get_Char_Index(mFace->face, chr);
        if(idx == 0)
            return idx;
        
		if((idx && !mGlyphs[idx-1].cached) || mGlyphs[idx-1].size != mFontSize) {
			mGlyphs[idx-1].resetFontSize(mFontSize);
			mGlyphs[idx-1].cache(idx, *this);
		}
        return idx;
    }
    
    void Font::draw(const ukn_wstring& str, float x, float y, FontAlignment alignment) {
        Font::StringData data;
        data.string_to_render = str;
        data.x = x;
        data.y = y;
        data.alignment = alignment;
        
        mRenderQueue.push_back(data);
        
        // cache chars to render
        const wchar_t* sptr = str.c_str();
        while(sptr && *sptr) {
            getGlyphByChar(*sptr);
            ++sptr;
        }
    }
    
    const ukn_string& Font::getName() const {
        static ukn_string name("freetype_font");
        return name;
    }
    
    float Font::getCharWidth(wchar_t chr) {
        uint32 idx = getGlyphByChar(chr);
        
        if(idx > 0 && idx < mGlyphs.size()) {
			int w = mGlyphs[idx-1].texw;
			int left = mGlyphs[idx-1].left;
			if (w + left > 0) return (float)(w+left);
		}
		if(chr >= 0x2000) {
			return (float)mGlyphs[0].size;
		} else {
			return (float)mGlyphs[0].size / 2;
		}
    }
    
    float2 Font::getStringDimensions(const ukn_wstring& str, float kw, float kh) {
        float2 dim(0.f, 0.f);
        
        float tmpw = 0.f;
        const wchar_t* pstr = str.c_str();
        while(pstr) {
            if(*pstr != L'\n') {
                tmpw += getCharWidth(*pstr) + kw;
                if(dim[0] < tmpw) 
                    dim[0] = tmpw;
            }
            else {
                dim[1] += mFontSize + kh;
                tmpw = 0.f;
            }
            
            pstr++;
        }
        return dim;
    }
    
    void Font::onRenderBegin() {
        if(mSpriteBatch)
            mSpriteBatch->onRenderBegin();
    }
    
    void Font::onRenderEnd() {
        mRenderQueue.clear();
        
        if(mSpriteBatch)
            mSpriteBatch->onRenderEnd();
    }
    
    Rectangle Font::getBound() const {
        return mSpriteBatch->getBound();
    }
    
    RenderBufferPtr Font::getRenderBuffer() const {
        return mSpriteBatch->getRenderBuffer();
    }
    
    
} // namespace ukn
