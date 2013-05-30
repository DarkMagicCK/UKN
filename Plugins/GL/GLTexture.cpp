//
//  GLTexture.cpp
//  Project Unknown
//
//  Created by Robert Bu on 12/2/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#include "GLTexture.h"

#include "soil/SOIL.h"

#include "mist/Resource.h"
#include "mist/Stream.h"
#include "mist/Logger.h"
#include "mist/FileUtil.h"

#include "GLPreq.h"
#include "GLConvert.h"
#include "GLError.h"

#if defined(MIST_OS_OSX)
#include <OpenGL/glu.h>
#endif


namespace ukn {

    GLTexture2D::GLTexture2D():
    Texture(TT_Texture2D, 1, 0, 0),
    mMappedLevel(0),
    mTextureData(0) {

    }

    GLTexture2D::~GLTexture2D() {
        if(mTextureId != 0) {
            glDeleteTextures(1, (GLuint*)&mTextureId);
        }
    
    }

    bool GLTexture2D::load(const ResourcePtr& resource, bool mipmap) {
        if(resource && resource->getResourceStream()) {
            StreamPtr memStream = resource->getResourceStream()->readIntoMemory();

            if(memStream) {
                MemoryStream* memData = static_cast<MemoryStream*>(memStream.get());

                int w, h, channels;

                mTextureId = (uintPtr)SOIL_load_OGL_texture_and_info_from_memory(
                                                                   (const unsigned char* const)memData->data(),
                                                                   (int)memData->size(),
                                                                   SOIL_LOAD_AUTO,
                                                                   SOIL_CREATE_NEW_ID,
                                                                   (mipmap ? SOIL_FLAG_MIPMAPS : 0)
                                                                   | /*SOIL_FLAG_POWER_OF_TWO | */SOIL_FLAG_TEXTURE_REPEATS/*| SOIL_FLAG_INVERT_Y*/,
                                                                   &w, &h, &channels);

                if(mTextureId != 0) {
                    mOrigWidth = mWidth = w;
                    mOrigHeight = mHeight = h;

                    glBindTexture(GL_TEXTURE_2D, (GLuint)mTextureId);
                    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &w);
                    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &h);
                    
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                    
                    glBindTexture(GL_TEXTURE_2D, 0);

                    mWidth = w;
                    mHeight = h;
                    mNumMipmaps = 1;
                    // temp
                    if(channels == 4 || channels == 3) {
                        mFormat = EF_RGBA8;
                    }

                    return true;
                }
            }
        } else {
			log_error(L"ukn::GLTexture2D::load(name, mipmap): error loading texture " + resource->getName());
		}
        return false;
    }

    bool GLTexture2D::create(uint32 w, uint32 h, uint32 mipmaps, ElementFormat format, const uint8* initialData) {
        if(mTextureId != 0)
            glDeleteTextures(1, (GLuint*)&mTextureId);
        
        GLint prevTexture;
        glGetIntegerv(GL_TEXTURE_BINDING_2D, &prevTexture);

        // to do with element formats
        GLuint texId;
		glGenTextures(1, &texId);

        if(texId != 0) {
            glBindTexture(GL_TEXTURE_2D, texId);
            glEnable(GL_TEXTURE_2D);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            uint8* texData;
            if(initialData == 0) {
                texData = 0;
            } else {
                texData = const_cast<uint8*>(initialData);
            }

            if(mipmaps > 1) {
                CHECK_GL_CALL(gluBuild2DMipmaps(GL_TEXTURE_2D,
                                  element_format_to_gl_format(format),
                                  w,
                                  h,
                                  element_format_to_texdata_format(format),
                                  element_format_to_gl_element_type(format),
                                  texData));
            }
            else {
                CHECK_GL_CALL(glTexImage2D(GL_TEXTURE_2D,
                              0,
                              element_format_to_gl_format(format),
                              w,
                              h,
                              0,
                              element_format_to_texdata_format(format),
                              element_format_to_gl_element_type(format),
                              texData));
            }
            if(initialData == 0)
                mist_free(texData);
                
            mTextureId = (uintPtr)texId;
            mWidth = mOrigWidth = w;
            mHeight = mOrigHeight = h;
            mFormat = format;
            mNumMipmaps = mipmaps;
            
            glBindTexture(GL_TEXTURE_2D, prevTexture);

            return true;
        }
        return false;
    }

    uint32 GLTexture2D::width(uint32 level) const {
        return mOrigWidth;
    }

    uint32 GLTexture2D::height(uint32 level) const {
        return mOrigHeight;
    }

    uint32 GLTexture2D::depth(uint32 level) const {
        return 0;
    }

    void GLTexture2D::generateMipmaps() {

    }
    
    void* GLTexture2D::map(uint32 level) {
        if(mTextureData) {
            log_error(L"GLTexture2D::map, texture already mapped");
            return mTextureData;
        }

        GLint prevTexture;
        glGetIntegerv(GL_TEXTURE_BINDING_2D, &prevTexture);
        
        uint8* texData = mist_malloc_t(uint8, mOrigWidth * mOrigHeight * GetElementSize(this->format()));
        
        CHECK_GL_CALL(glBindTexture(GL_TEXTURE_2D, (GLint)mTextureId));
        CHECK_GL_CALL(glGetTexImage(GL_TEXTURE_2D,
                      level,
                      element_format_to_texdata_format(this->format()),
                      element_format_to_gl_element_type(this->format()),
                      texData));
        
        glBindTexture(GL_TEXTURE_2D, prevTexture);
        mTextureData = texData;
        mMappedLevel = level;
        
        return texData;
    }
    
    void GLTexture2D::unmap() {
        if(!mTextureData) {
            log_error(L"GLTexture2D::unmap: texture not mapped");
            return;
        }

        GLint prevTexture;
        glGetIntegerv(GL_TEXTURE_BINDING_2D, &prevTexture);
        
        CHECK_GL_CALL(glBindTexture(GL_TEXTURE_2D, (GLint)mTextureId));
        CHECK_GL_CALL(glTexSubImage2D(GL_TEXTURE_2D,
                        mMappedLevel,
                        0,
                        0,
                        mWidth,
                        mHeight,
                        element_format_to_texdata_format(this->format()),
                        element_format_to_gl_element_type(this->format()),
                        mTextureData));
        
        glBindTexture(GL_TEXTURE_2D, prevTexture);

        mist_free(mTextureData);
        mTextureData = 0;
    }
    
    void GLTexture2D::updateData(const mist::Rectangle& rect, const uint8* data, uint32 level) {
        GLint prevTexture;
        glGetIntegerv(GL_TEXTURE_BINDING_2D, &prevTexture);
        
        glBindTexture(GL_TEXTURE_2D, (GLint)mTextureId);
        CHECK_GL_CALL(glTexSubImage2D(GL_TEXTURE_2D,
                        level,
                        (GLsizei)rect.x(),
                        (GLsizei)rect.y(),
                        (GLsizei)rect.width(),
                        (GLsizei)rect.height(),
                        element_format_to_texdata_format(this->format()),
                        element_format_to_gl_element_type(this->format()),
                        data));
        
        glBindTexture(GL_TEXTURE_2D, prevTexture);
    }
    
    uintPtr GLTexture2D::getTextureId() const {
        return mTextureId;
    }

} // namespace ukn