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

#include "GLPreq.h"
#include "GLConvert.h"

namespace ukn {

    GLTexture2D::GLTexture2D():
    Texture(TT_Texture2D) {

    }

    GLTexture2D::~GLTexture2D() {
        if(mTextureId != 0)
            glDeleteTextures(1, (GLuint*)&mTextureId);
    }

    bool GLTexture2D::load(const ResourcePtr& resource, bool mipmap) {
        if(resource->getResourceStream()) {
            StreamPtr memStream = resource->getResourceStream()->readIntoMemory();

            if(memStream) {
                MemoryStream* memData = static_cast<MemoryStream*>(memStream.get());

                int w, h, channels;

                mTextureId = (uintPtr)SOIL_load_OGL_texture_and_info_from_memory((const unsigned char* const)memData->data(),
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
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            uint8* texData;
            if(initialData == 0) {
                texData = mist_malloc_t(uint8, w*h*sizeof(uint32));
                memset(texData, 0xFF, w*h*sizeof(uint32));
            } else {
                texData = const_cast<uint8*>(initialData);
            }

            if(mipmaps != 0)
                gluBuild2DMipmaps(GL_TEXTURE_2D,
                                  element_format_to_gl_format(format),
                                  w,
                                  h,
                                  GL_RGBA,
                                  GL_UNSIGNED_BYTE,
                                  texData);
            else
                glTexImage2D(GL_TEXTURE_2D,
                             0,
                             element_format_to_gl_format(format),
                             w,
                             h,
                             0,
                             GL_RGBA,
                             GL_UNSIGNED_BYTE,
                             texData);
            if(initialData == 0)
                mist_free(texData);
                
            mTextureId = (uintPtr)texId;
            mWidth = mOrigWidth = w;
            mHeight = mOrigHeight = h;
            
            glBindTexture(GL_TEXTURE_2D, prevTexture);

            return true;
        }
        return false;
    }

    uint32 GLTexture2D::getWidth(uint32 level) const {
        return mOrigWidth;
    }

    uint32 GLTexture2D::getHeight(uint32 level) const {
        return mOrigHeight;
    }
    
    SharedPtr<uint8> GLTexture2D::readTextureData(uint8 level) {
        GLint prevTexture;
        glGetIntegerv(GL_TEXTURE_BINDING_2D, &prevTexture);
        
        uint8* texData = new uint8[mOrigWidth * mOrigHeight * GetElementSize(this->getFormat())];
        
        glBindTexture(GL_TEXTURE_2D, (GLint)mTextureId);
        glGetTexImage(GL_TEXTURE_2D,
                      level,
                      element_format_to_gl_format(this->getFormat()),
                      element_format_to_gl_element_type(this->getFormat()),
                      texData);
        if(glGetError() != GL_NO_ERROR) {
            log_error("GLGraphicDevice: error when locking texture");
        }
        
        glBindTexture(GL_TEXTURE_2D, prevTexture);
        return SharedPtr<uint8>(texData);
    }
    
    void GLTexture2D::updateTextureData(void* data, int32 x, int32 y, uint32 width, uint32 height, uint8 level) {
        if(width == 0)
            width = mOrigWidth;
        if(height == 0)
            height = mOrigHeight;
        
        GLint prevTexture;
        glGetIntegerv(GL_TEXTURE_BINDING_2D, &prevTexture);
        
        glBindTexture(GL_TEXTURE_2D, (GLint)mTextureId);
        glTexSubImage2D(GL_TEXTURE_2D,
                        level,
                        x,
                        y,
                        width,
                        height,
                        element_format_to_gl_format(this->getFormat()),
                        element_format_to_gl_element_type(this->getFormat()),
                        data);
        
        if(glGetError() != GL_NO_ERROR) {
            log_error("GLGraphicDevice: error when updating texture");
        }
        
        glBindTexture(GL_TEXTURE_2D, prevTexture);
    }

    uintPtr GLTexture2D::getTextureId() const {
        return mTextureId;
    }

} // namespace ukn