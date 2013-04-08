//
//  Font.cpp
//  Project Unknown
//
//  Created by Robert Bu on 12/9/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#include "mist/ConfigParser.h"
#include "mist/FileUtil.h"
#include "mist/Logger.h"
#include "mist/MathUtil.h"
#include "mist/Stream.h"
#include "mist/Resource.h"
#include "mist/StringUtil.h"

#include "UKN/Font.h"
#include "UKN/RenderBuffer.h"
#include "UKN/GraphicDevice.h"
#include "UKN/GraphicFactory.h"
#include "UKN/Context.h"
#include "UKN/SpriteBatch.h"
#include "UKN/Texture.h"

#ifdef MIST_OS_WINDOWS
#pragma comment(lib, "freetype244ST.lib")
#endif

namespace ukn {

    FontPtr Font::Create(const UknString& name_or_path, uint32 size, bool bold, bool italic) {
        ResourcePtr resource;
        
        if(mist::File::FileExists(name_or_path)) {
            resource = ResourceLoader::Instance().loadResource(name_or_path);
        } else {
            resource = ResourceLoader::Instance().loadResource(mist::Path::GetFont() + name_or_path);
        }
        
        if(resource) {            
            SharedPtr<Font> font = MakeSharedPtr<Font>();
            if(font && font->loadFromFontFile(resource, size, bold, italic))
                return font;
        }
        
        return SharedPtr<Font>();
    }
    
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
                               string::WStringToString(resource->getName()).c_str(),
                               0,
                               &face)) {
                    return false;
                }
                return true;
            }
            return false;
        }
    };
    
    Font::FTGlyph::FTGlyph():
    cached(false),
    face(0),
    size(0),
    top(0),
    left(0),
    texw(0),
    texh(0),
    texture_id(0) {

    }
        
    Font::FTGlyph:: ~FTGlyph() {
    }
       
    void Font::FTGlyph::resetFontSize(uint32 newsize) {
        if(size != newsize) {
            size = newsize; 
            cached = false;
        }
    }
        
    bool Font::FTGlyph::cache(uint32 index, Font& font, uint32* texdata, TexturePlacement& placement) {
        if(cached)
            return true;
            
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
                    uint32 imgw = 1;
                    uint32 imgh = 1;
                    texw = bits.width;
                    texh = bits.rows;
                        
                    imgw = math::next_pow_of_2(texw);
                    imgh = math::next_pow_of_2(texh);
                    imgw > imgh ? imgh = imgw : imgw = imgh;
                        
                    if(imgw == 0 || imgh == 0)
                        return true;
                    
                    this->rect = mist::Rectangle(0.f, 0.f, (float)imgw, (float)imgh, true);
                    if(!placement.rect.addAtEmptySpotAutoGrow(this->rect, imgw, imgh)) {
                        return false;
                    }
                    
                    if(texdata) {
                        uint32* texp = texdata + (uint32)this->rect.x() 
                                               + (uint32)this->rect.y() * placement.texture->width();
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
                            texp += placement.texture->width();
                        }
                        cached = true;
                        return true;
                    }
                }
            }
        }
        return true;
    }
    
    
    Font::Font():
    mFontSize(0),
    mEnableStroke(false),
    mEnableShadow(false),
    mStrokeWidth(0),
    mShadowXOffset(0),
    mShadowYOffset(0),
    mKerningWidth(0),
    mKerningHeight(0),
    mCharRotation(0),
    mLineWidth(0),
    mBegan(false),
    mFace(new Font::FTFace) {
        mSpriteBatch = Context::Instance().getGraphicFactory().createSpriteBatch();
        if(!mSpriteBatch)
            log_error("ukn::Font::Font Error creating sprite batch for font rendering");
        
        this->appendTexturePlacement();
    }
    
    Font::~Font() {
        
    }
    
    bool Font::loadFromFontFile(const ResourcePtr& resource, uint32 size, bool bold, bool italic) {
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
                
            mFontSize = size;
            if(bold) {
                this->setFTStyle(FT_STYLE_FLAG_BOLD);
            }
            if(italic) {
                this->setFTStyle(FT_STYLE_FLAG_ITALIC);
            }
            mFontName = resource->getName();
        }

        return result;
    }

    bool Font::loadFromConfigFile(const ResourcePtr& resource) {
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
            UknString font_name = config->getString(L"name");
            
            if(!font_name.empty()) {
                UknString fullFontPath = Path::CheckAndGetFontPath(font_name);
                
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

            if(config->getBool(L"bold", false)) {
                this->setFTStyle(FT_STYLE_FLAG_BOLD);
            }
            if(config->getBool(L"italic", false)) {
                this->setFTStyle(FT_STYLE_FLAG_ITALIC);
            }
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
            cfg->setBool(L"bold", (bool)(mFace->face->style_flags & FT_STYLE_FLAG_BOLD));
            cfg->setBool(L"italic", (bool)(mFace->face->style_flags & FT_STYLE_FLAG_ITALIC));

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
        }
    }
    
    void Font::setStyleProperty(FontStyleProperty sp, int32 prop) {
        switch(sp) {
            case FSP_StrokeWidth: mStrokeWidth = prop; break;
            case FSP_ShadowXOffset: mShadowXOffset = prop; break;
            case FSP_ShadowYOffset: mShadowYOffset = prop; break;
            case FSP_KerningHeight: mKerningHeight = prop; break;
            case FSP_KerningWidth: mKerningWidth = prop; break;
            case FSP_LineWidth: mLineWidth = prop; break;
        }
    }
    
    bool Font::isValid() const {
        return !mGlyphs.empty();
    }
    
    void Font::begin() {
        if(!mSpriteBatch)
            return;
           
        if(mSpriteBatch) {
            mSpriteBatch->begin();
            mBegan = true;
        }
    }

    void Font::end() {
        if(mGlyphs.size() == 0)
            return;
                
        mBegan = false;
        if(mSpriteBatch) {
            mSpriteBatch->end();
        }
    }
    
    uint32 Font::getGlyphByChar(uint16 chr) {
        uint32 idx = FT_Get_Char_Index(mFace->face, chr);
        if(idx == 0)
            return idx;
        
        FTGlyph& glyph = mGlyphs[idx-1];
		/* each font should be cached here */
        if((idx && !glyph.cached)) {
			return 0;
		}
        return idx;
    }

    void Font::cacheString(const std::wstring& str) {
       
       std::vector<uint32> uncached_idx;
       for(wchar_t chr: str) {
           uint32 idx = FT_Get_Char_Index(mFace->face, chr);
           if(idx == 0) {
               continue;
           }
           if(!mGlyphs[idx-1].cached)
               uncached_idx.push_back(idx);
       }

       if(!uncached_idx.empty()) {
           TexturePlacement* placement = this->getTexturePlacement(this->getLastTexturePlacement());
           uint32* texdata = (uint32*)placement->texture->map();

           if(texdata) {
               for(uint32& idx: uncached_idx) {
                   FTGlyph& glyph = mGlyphs[idx-1];
                   glyph.size = mFontSize;
                   if(!glyph.cache(idx, *this, texdata, *placement)) {
                       // texture full, add a new texture
                       placement->texture->unmap();
                       placement = this->appendTexturePlacement();
                       texdata = (uint32*)placement->texture->map();
                       if(!texdata) {
                           log_error(L"Font::cacheString: unable to map texture");
                           break;
                       }
                   }
               }
    
               placement->texture->unmap();
           } else {
               log_error(L"Font::cacheString: unable to map texture");
           }
       }
    }
    
    void Font::draw(const std::wstring& str, float _x, float _y, FontAlignment alignment, const Color& clr) {
        if(!mBegan) {
            log_warning("Font::draw: Font::begin must be called before drawing the string");
            return;
        }

        this->cacheString(str);

        float start_x = _x;
        float start_y = _y;
        float x = _x;
        float y = _y;
        
        for(wchar_t chr: str) {
            if(chr != L'\n') {
                if(chr == L' ') {
                    x += mFontSize / 2 + this->mKerningWidth;
                    
                } else {
                    uint32 gidx = getGlyphByChar(chr);
                    
                    if(gidx > 0 && gidx < mGlyphs.size()) {
                        FTGlyph& glyph = mGlyphs[gidx-1];
                        
                        mSpriteBatch->draw(mTextures[glyph.texture_id].texture,
                                           Vector2(x+glyph.left, 
                                                   y+glyph.size-glyph.top),
                                           glyph.rect,
                                           Vector2(0, 0),
                                           0,
                                           Vector2(1, 1),
                                           mCharRotation, 
                                           clr);
                        
                        x += glyph.texw + glyph.left + mKerningWidth;
                    }
                    
                    if(mLineWidth > 0 && (x - start_y) > mLineWidth) {
                        y += mFontSize + mKerningHeight;
                        x = start_x;
                    }
               }
            } else {
                y += mFontSize + mKerningHeight;
                x = start_x;
            }
        }
    }
    
    float2 Font::getStringDimensions(const std::wstring& str, float kw, float kh) {
        float2 dim(0.f, 0.f);
        
        float tmpw = 0.f;
        for(wchar_t chr: str) {
            if(chr != L'\n') {
                tmpw += mGlyphs[getGlyphByChar(chr)].texw + kw;
                if(dim[0] < tmpw) 
                    dim[0] = tmpw;
            }
            else {
                dim[1] += mFontSize + kh;
                tmpw = 0.f;
            }
        }
        return dim;
    }
    
    Font::TexturePlacement* Font::getTexturePlacement(uint32 tid) {
        if(tid < mTextures.size())
            return &mTextures[tid];
        return 0;
    }
    
    Font::TexturePlacement* Font::appendTexturePlacement() {
        mTextures.push_back(TexturePlacement());
        mTextures.back().texture = Context::Instance().getGraphicFactory().create2DTexture(1024, 
                                                                                           1024,
                                                                                           0, 
                                                                                           ukn::EF_RGBA8);
        mTextures.back().rect.init(1024, 1024);
        return &mTextures.back();
    }
    
    uint32 Font::getLastTexturePlacement() const {
        return (uint32)mTextures.size() - 1;
    }
    
} // namespace ukn
