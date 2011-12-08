//
//  GLSpriteBatch.cpp
//  Project Unknown
//
//  Created by Ruiwei Bu on 12/8/11.
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
    
    void GLSpriteBatch::render() {
        if(mRenderQueue.size() == 0)
            return ;
        
        GraphicDevice& gd = Context::Instance().getGraphicFactory().getGraphicDevice();
        
        gd.pushProjectionMatrix();
        gd.pushViewMatrix();
        
        gd.setProjectionMatrix(ukn::Matrix4::OrthoMat(0, gd.getCurrFrameBuffer()->width(), gd.getCurrFrameBuffer()->height(), 0, 0, 1));
        gd.setViewMatrix(ukn::Matrix4());
        
        RenderQueue::iterator it = mRenderQueue.begin();
        
        Array<Vertex2D> buffer;
        TexturePtr prevTexture = it->texture;
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
                
            } else {
                buffer.append(&it->vertices[0], 6);
            }
            
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
        
        gd.bindTexture(TexturePtr());
        
        gd.popProjectionMatrix();
        gd.popViewMatrix();
    }
    
} // namespace ukn
