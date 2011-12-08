//
//  SpriteBatch.cpp
//  Project Unknown
//
//  Created by Ruiwei Bu on 12/7/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#include "UKN/SpriteBatch.h"
#include "UKN/GraphicFactory.h"
#include "UKN/GraphicDevice.h"
#include "UKN/Context.h"
#include "UKN/RenderBuffer.h"
#include "UKN/Texture.h"
#include "UKN/GraphicBuffer.h"
#include "UKN/FrameBuffer.h"

namespace ukn {
    
    SpriteBatch::TextureObject::TextureObject():
    layerDepth(0.f),
    srcRect(Rectangle(0, 0, 0, 0)) { 
     
    }
        
    SpriteBatch::TextureObject::TextureObject(const TexturePtr& tex):
    texture(tex),
    layerDepth(0.f) {
        if(texture) {
            srcRect.set(0, 0, texture->getWidth(), texture->getHeight());
        }
    }
        
    SpriteBatch::TextureObject::TextureObject(const TexturePtr& tex, float depth):
    texture(tex),
    layerDepth(depth) {
        if(texture) {
            srcRect.set(0, 0, texture->getWidth(), texture->getHeight());
        }
    }
       
        
    void SpriteBatch::TextureObject::buildVertices(float x, float y, float cx, float cy, float rot, float scalex, float scaley, const Color& color) {  
        if(!texture)
            return;
        
        Vertex2D tmpVert[4];
        
        float u1 = srcRect.x1 / texture->getWidth();
        float u2 = srcRect.x2 / texture->getWidth();
        float v1 = srcRect.y1 / texture->getHeight();
        float v2 = srcRect.y2 / texture->getHeight();
        
        tmpVert[0].u = u1; tmpVert[0].v = v1;
        tmpVert[1].u = u2; tmpVert[1].v = v1;
        tmpVert[2].u = u2; tmpVert[2].v = v2;
        tmpVert[3].u = u1; tmpVert[3].v = v2;
        
        float tx1 = -cx*scalex;
        float ty1 = -cx*scaley;
        float tx2 = (srcRect.x2-cx)*scalex;
        float ty2 = (srcRect.y2-cy)*scaley;
        
        if(rot != 0.0f) {
            float cost = cosf(rot);
            float sint = sinf(rot);
            
            tmpVert[0].x  = tx1*cost - ty1*sint + x;
            tmpVert[1].x  = tx2*cost - ty1*sint + x;
            tmpVert[2].x  = tx2*cost - ty2*sint + x;
            tmpVert[3].x  = tx1*cost - ty2*sint + x;
            
            tmpVert[0].y  = tx1*sint + ty1*cost + y;	
            tmpVert[1].y  = tx2*sint + ty1*cost + y;	
            tmpVert[2].y  = tx2*sint + ty2*cost + y;	
            tmpVert[3].y  = tx1*sint + ty2*cost + y;
        }
        else {
            tmpVert[0].x = tx1 + x; 
            tmpVert[1].x = tx2 + x; 
            tmpVert[2].x = tmpVert[1].x; 
            tmpVert[3].x = tmpVert[0].x; 
            
            tmpVert[0].y = ty1 + y;
            tmpVert[1].y = tmpVert[0].y;
            tmpVert[2].y = ty2 + y;
            tmpVert[3].y = tmpVert[2].y;
        }
        
        for(int i=0; i<4; ++i) {
            tmpVert[i].color = color.toHWColor();
        }
        
        vertices[0] = tmpVert[0];
        vertices[1] = tmpVert[1];
        vertices[2] = tmpVert[2];
        
        vertices[3] = tmpVert[2];
        vertices[4] = tmpVert[3];
        vertices[5] = tmpVert[0];
    }
        
    bool SpriteBatch::TextureObject::operator<(const SpriteBatch::TextureObject& rhs) const {
        return this->layerDepth < rhs.layerDepth;
    }
    
    SpriteBatch::SpriteBatch() {
        mRenderBuffer = Context::Instance().getGraphicFactory().createRenderBuffer();
        ukn_assert(mRenderBuffer);
        
        mVertexBuffer = Context::Instance().getGraphicFactory().createVertexBuffer(GraphicBuffer::ReadWrite, 
                                                                                   GraphicBuffer::Static, 
                                                                                   6, 
                                                                                   0, 
                                                                                   Vertex2D::Format());
        ukn_assert(mVertexBuffer);
        
        mRenderBuffer->bindVertexStream(mVertexBuffer, Vertex2D::Format());
    }
    
    SpriteBatch::~SpriteBatch() {
        
    }
    
    const ukn_string& SpriteBatch::getName() const {
        static ukn_string name("SpriteBatch");
        return name;
    }
    
    Rectangle SpriteBatch::getBound() const {
        return mBoundingBox;
    }
    
    RenderBufferPtr SpriteBatch::getRenderBuffer() const {
        return mRenderBuffer;
    }
    
    void SpriteBatch::onRenderBegin() {
        mRenderQueue.sort();
    }
    
    void SpriteBatch::onRenderEnd() {
        mRenderQueue.clear();
    }
    
    void SpriteBatch::render() {
        GraphicDevice& gd = Context::Instance().getGraphicFactory().getGraphicDevice();
        
        gd.pushProjectionMatrix();
        gd.pushViewMatrix();
        
        gd.setProjectionMatrix(ukn::Matrix4::OrthoMat(0, gd.getCurrFrameBuffer()->width(), gd.getCurrFrameBuffer()->height(), 0, 0, 1));
        gd.setViewMatrix(ukn::Matrix4());
        
        RenderQueue::iterator it = mRenderQueue.begin();
        while(it != mRenderQueue.end()) { 
            Vertex2D* vertices = (Vertex2D*)mVertexBuffer->map();
            memcpy(vertices, &it->vertices[0], sizeof(Vertex2D)*6);
            mVertexBuffer->unmap();
            
            gd.bindTexture(it->texture);
            gd.onRenderBuffer(mRenderBuffer);
            
            ++it;
        }
        
        gd.bindTexture(TexturePtr());
        
        gd.popProjectionMatrix();
        gd.popViewMatrix();
    }
    
    void SpriteBatch::updateBoundingBox(float x, float y, float x2, float y2) {
        if(mBoundingBox.x1 > x) {
            mBoundingBox.x1 = x;
        }
        if(mBoundingBox.y1 > y) {
            mBoundingBox.y1 = y;
        }
        if(mBoundingBox.x2 < x2) {
            mBoundingBox.x2 = x2;
        }
        if(mBoundingBox.y2 < y2) {
            mBoundingBox.y2 = y2;
        }
    }
    
    void SpriteBatch::draw(const TexturePtr& texture, float x, float y, const Color& color) {
        draw(texture, x, y, 0.f);
    }
    
    void SpriteBatch::draw(const TexturePtr& texture, Rectangle dstRect, const Color& color) {
        draw(texture, dstRect, 0.f);
    }
    
    void SpriteBatch::draw(const TexturePtr& texture, Rectangle srcRect, Rectangle dstRect, const Color& color) {
        draw(texture, srcRect, dstRect, 0.f);
    }
    
    void SpriteBatch::draw(const TexturePtr& texture, float x, float y, float cx, float cy, float rot, float scalex, float scaley, const Color& color) {
        draw(texture, x, y, cx, cy, rot, scalex, scaley, 0.f);
    }
    
    void SpriteBatch::draw(const TexturePtr& texture, float x, float y, float layerDepth, const Color& color) {
        TextureObject obj(texture, layerDepth);
        obj.buildVertices(x, y, 0.f, 0.f, 0.f, 1.f, 1.f, color);
        
        mRenderQueue.insert(obj);
    }
    
    void SpriteBatch::draw(const TexturePtr& texture, Rectangle dstRect, float layerDepth, const Color& color) {
        TextureObject obj(texture, layerDepth);
        
        obj.buildVertices(dstRect.x1, 
                          dstRect.y1, 
                          0.f,
                          0.f,
                          0.f, 
                          (dstRect.x2 - dstRect.x1) / texture->getWidth(), 
                          (dstRect.y2 - dstRect.y1) / texture->getHeight(),
                          color);
                          
        
        mRenderQueue.insert(obj);
    }
    
    void SpriteBatch::draw(const TexturePtr& texture, Rectangle srcRect, Rectangle dstRect, float rot, const Color& color) {
        TextureObject obj(texture, 0.f);
        obj.srcRect = srcRect;
        
        float w = srcRect.x2 - srcRect.x1;
        float h = srcRect.y2 - srcRect.y1;
        float sx = (dstRect.x2 - dstRect.x1) / w;
        float sy = (dstRect.y2 - dstRect.y1) / h;
        obj.buildVertices(dstRect.x1 + w / 2 * sx, 
                          dstRect.y1 + h / 2 * sy, 
                          w / 2,
                          h / 2,
                          rot, 
                          sx, 
                          sy,
                          color);
        
        mRenderQueue.insert(obj);
    }
    
    void SpriteBatch::draw(const TexturePtr& texture, float x, float y, float cx, float cy, float rot, float scalex, float scaley, float layerDepth, const Color& color) {
        TextureObject obj(texture, layerDepth);
        
        obj.buildVertices(x,
                          y,
                          cx,
                          cy,
                          rot, 
                          scalex, 
                          scaley,
                          color);
        
        mRenderQueue.insert(obj);
    }
    
    void SpriteBatch::draw(const TexturePtr& texture, Rectangle srcRect, Rectangle dstRect, float rot, float layerDepth, const Color& color) {
        TextureObject obj(texture, layerDepth);
        obj.srcRect = srcRect;
        
        float w = srcRect.x2 - srcRect.x1;
        float h = srcRect.y2 - srcRect.y1;
        float sx = (dstRect.x2 - dstRect.x1) / w;
        float sy = (dstRect.y2 - dstRect.y1) / h;
        obj.buildVertices(dstRect.x1 + w / 2 * sx, 
                          dstRect.y1 + h / 2 * sy, 
                          w / 2,
                          h / 2,
                          rot, 
                          sx, 
                          sy,
                          color);
        
        mRenderQueue.insert(obj);
    }
    
} // namespace ukn
