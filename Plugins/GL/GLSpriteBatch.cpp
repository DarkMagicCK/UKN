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
#include "UKN/Texture.h"
#include "UKN/FrameBuffer.h"

#include "mist/MathUtil.h"


#include "GLPreq.h"

namespace ukn {
    
    GLSpriteBatch::GLSpriteBatch() {
        
    }
    
    GLSpriteBatch::~GLSpriteBatch() {
        
    }
    
    void GLSpriteBatch::onRender() {
        if(mRenderQueue.size() == 0)
            return ;
                
        GraphicDevice& gd = Context::Instance().getGraphicFactory().getGraphicDevice();
            
        RenderQueue::iterator it = mRenderQueue.begin();
        
        Array<Vertex2D> buffer;
        TexturePtr prevTexture = it->texture;
        
        while(it != mRenderQueue.end()) { 
            if(it->texture != prevTexture) {
                gd.bindTexture(prevTexture);
                
                glInterleavedArrays(GL_T2F_C4UB_V3F, 
                                    0, 
                                    &buffer.front());
                
                glDrawArrays(GL_TRIANGLES, 
                             0, 
                             (GLsizei)buffer.size());
                
                prevTexture = it->texture;
                buffer.clear();
            }
            buffer.insert(buffer.end(), &it->vertices[0], &it->vertices[0] + 6);
            
            ++it;
        }
        
        if(!buffer.empty()) {
            gd.bindTexture(prevTexture);
            
            glInterleavedArrays(GL_T2F_C4UB_V3F, 
                                0, 
                                &buffer.front());
            
            glDrawArrays(GL_TRIANGLES, 
                         0, 
                         (GLsizei)buffer.size());
        }
    }
    
} // namespace ukn
