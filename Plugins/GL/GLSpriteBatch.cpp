//
//  GLSpriteBatch.cpp
//  Project Unknown
//
//  Created by Robert Bu on 12/8/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#include "GLSpriteBatch.h"

#include "UKN/GraphicDevice.h"
#include "UKN/Context.h"
#include "UKN/GraphicFactory.h"
#include "UKN/MathUtil.h"
#include "UKN/Texture.h"
#include "UKN/FrameBuffer.h"

#include "GLPreq.h"

namespace ukn {
    
    GLSpriteBatch::GLSpriteBatch() {
        
    }
    
    GLSpriteBatch::~GLSpriteBatch() {
        
    }
    
    void GLSpriteBatch::onRender() {
        if(mRenderQueue.size() == 0)
            return ;
        
        onRenderBegin();
        
        GraphicDevice& gd = Context::Instance().getGraphicFactory().getGraphicDevice();
        
        gd.pushProjectionMatrix();
        gd.pushViewMatrix();
        
        Camera* cam = gd.getCurrFrameBuffer()->getViewport().camera.get();
        
        gd.setProjectionMatrix(cam->getProjMatrix());
        gd.setViewMatrix(mTransformMatrix * cam->getViewMatrix());
        
        RenderQueue::iterator it = mRenderQueue.begin();
        
        Array<Vertex2D> buffer;
        TexturePtr prevTexture = it->texture;

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);    
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        
        while(it != mRenderQueue.end()) { 
            if(it->texture != prevTexture) {
                gd.bindTexture(prevTexture);
                
                glInterleavedArrays(GL_T2F_C4UB_V3F, 
                                    0, 
                                    buffer.begin());
                
                glDrawArrays(GL_TRIANGLES, 
                             0, 
                             (GLsizei)buffer.size());
                
                prevTexture = it->texture;
                buffer.clear();
            }
            buffer.append(&it->vertices[0], 6);
            
            ++it;
        }
        
        if(!buffer.empty()) {
            gd.bindTexture(prevTexture);
            
            glInterleavedArrays(GL_T2F_C4UB_V3F, 
                                0, 
                                buffer.begin());
            
            glDrawArrays(GL_TRIANGLES, 
                         0, 
                         (GLsizei)buffer.size());
        }
        
        glDisable(GL_BLEND);
        
        gd.bindTexture(TexturePtr());
        
        gd.popProjectionMatrix();
        gd.popViewMatrix();
        
        onRenderEnd();
    }
    
} // namespace ukn
