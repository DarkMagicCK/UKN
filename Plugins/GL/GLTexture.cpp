//
//  GLTexture.cpp
//  Project Unknown
//
//  Created by Ruiwei Bu on 12/2/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#include "GLTexture.h"

#include "soil/SOIL.h"

#include "UKN/Resource.h"
#include "UKN/Stream.h"

#include "glfw/glfw3.h"

namespace ukn {
    
    GLTexture2D::GLTexture2D():
    Texture(TT_Texture2D) {
        
    }
    
    GLTexture2D::~GLTexture2D() {
        if(mTextureId != 0)
            glDeleteTextures(1, (GLuint*)&mTextureId);
    }
    
    bool GLTexture2D::load(const ukn_wstring& name, bool mipmap) {
        ResourcePtr resource = ResourceLoader::Instance().loadResource(name);
        if(resource && resource->getResourceStream()) {
            StreamPtr memStream = stream_to_memory_stream(resource->getResourceStream());
            
            if(memStream) {
                MemoryStream* memData = static_cast<MemoryStream*>(memStream.get());
                
                int w, h, channels;
                
                mTextureId = (uintPtr)SOIL_load_OGL_texture_and_info_from_memory((const unsigned char* const)memData->get(),
                                                                   (int)memData->getSize(),
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
                    glBindTexture(GL_TEXTURE_2D, 0);
                    
                    glBindTexture(GL_TEXTURE_2D, (GLuint)mTextureId);
                    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &h);
                    glBindTexture(GL_TEXTURE_2D, 0);
                    
                    mWidth = w;
                    mHeight = h;
                    
                    return true;
                }
            }            
        }
        return false;
    }
    
    bool GLTexture2D::create(uint32 w, uint32 h, uint32 mipmaps, ElementFormat format, const uint8* initialData) {
        if(mTextureId != 0)
            glDeleteTextures(1, (GLuint*)&mTextureId);
        
        // to do with element formats
        GLuint texId;
		glGenTextures(1, &texId);
		
        if(texId != 0) {
            glBindTexture(GL_TEXTURE_2D, texId);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		
            uint8* texData;
            if(initialData == 0) {
                texData = ukn_malloc_t(uint8, w*h*sizeof(uint32));
                memset(texData, 0, w*h*sizeof(uint32));
            } else {
                texData = const_cast<uint8*>(initialData);
            }
            
            if(mipmaps != 0)
                gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, w, h, GL_RGBA, GL_UNSIGNED_BYTE, texData);
            else
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData);
            delete texData;
        
            mTextureId = (uintPtr)texId;
            mWidth = mOrigWidth = w;
            mHeight = mOrigHeight = h;
            
            return true;
        }
        return false;
    }
    
    uint32 GLTexture2D::getWidth(uint32 level) const {
        return mWidth;
    }
    
    uint32 GLTexture2D::getHeight(uint32 level) const {
        return mHeight;
    }
    
    uintPtr GLTexture2D::getTextureId() const {
        return mTextureId;
    }
    
} // namespace ukn
