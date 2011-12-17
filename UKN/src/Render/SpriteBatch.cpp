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
       
    void SpriteBatch::TextureObject::buildVertices(const Rectangle& dstRect, float rot, const Color& color) {
        float u1 = 0, u2 = 0, v1 = 0, v2 = 0;
        if(texture) {
            u1 = srcRect.x1 / texture->getWidth();
            u2 = srcRect.x2 / texture->getWidth();
            v1 = srcRect.y1 / texture->getHeight();
            v2 = srcRect.y2 / texture->getHeight();
        }
        
        Vertex2D tmpVert[4];
        tmpVert[0].u = u1; tmpVert[0].v = v1;
        tmpVert[1].u = u2; tmpVert[1].v = v1;
        tmpVert[2].u = u2; tmpVert[2].v = v2;
        tmpVert[3].u = u1; tmpVert[3].v = v2;
        
        tmpVert[0].x = dstRect.x1; tmpVert[0].y = dstRect.y1; tmpVert[0].z = layerDepth;
        tmpVert[1].x = dstRect.x2; tmpVert[1].y = dstRect.y1; tmpVert[0].z = layerDepth;
        tmpVert[2].x = dstRect.x2; tmpVert[2].y = dstRect.y2; tmpVert[0].z = layerDepth;
        tmpVert[3].x = dstRect.x1; tmpVert[3].y = dstRect.y2; tmpVert[0].z = layerDepth;
        
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
        
    void SpriteBatch::TextureObject::buildVertices(float x, float y, float cx, float cy, float rot, float scalex, float scaley, const Color& color) {  
        Vertex2D tmpVert[4];
        
        float u1 = 0, u2 = 0, v1 = 0, v2 = 0;
        if(texture) {
            u1 = srcRect.x1 / texture->getWidth();
            u2 = srcRect.x2 / texture->getWidth();
            v1 = srcRect.y1 / texture->getHeight();
            v2 = srcRect.y2 / texture->getHeight();
        }
        
        tmpVert[0].u = u1; tmpVert[0].v = v1; tmpVert[0].z = layerDepth;
        tmpVert[1].u = u2; tmpVert[1].v = v1; tmpVert[0].z = layerDepth;
        tmpVert[2].u = u2; tmpVert[2].v = v2; tmpVert[0].z = layerDepth;
        tmpVert[3].u = u1; tmpVert[3].v = v2; tmpVert[0].z = layerDepth;
        
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
        
        GraphicDevice& gd = Context::Instance().getGraphicFactory().getGraphicDevice();
        ukn::Matrix4::OrthoMat(0, gd.getCurrFrameBuffer()->width(), gd.getCurrFrameBuffer()->height(), 0, 0, 1);
    }
    
    SpriteBatch::~SpriteBatch() {
        
    }
    
    const ukn_wstring& SpriteBatch::getName() const {
        static ukn_wstring name(L"SpriteBatch");
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
    
    Matrix4& SpriteBatch::getTransformMatrix() {
        return mTransformMatrix;
    }
    
    const Matrix4& SpriteBatch::getTransformMatrix() const {
        return mTransformMatrix;
    }
    
    void SpriteBatch::render() {
        GraphicDevice& gd = Context::Instance().getGraphicFactory().getGraphicDevice();
        
        gd.pushProjectionMatrix();
        gd.pushViewMatrix();
        
        Camera* cam = gd.getCurrFrameBuffer()->getViewport().camera.get();
        
        gd.setProjectionMatrix(cam->getProjMatrix());
        gd.setViewMatrix(mTransformMatrix * cam->getViewMatrix());
        
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
    
    void SpriteBatch::draw(const TexturePtr& texture, const Rectangle& dstRect, const Color& color) {
        draw(texture, dstRect, 0.f);
    }
    
    void SpriteBatch::draw(const TexturePtr& texture, const Rectangle& srcRect, const Rectangle& dstRect, const Color& color) {
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
    
    void SpriteBatch::draw(const TexturePtr& texture, const Rectangle& dstRect, float layerDepth, const Color& color) {
        TextureObject obj(texture, layerDepth);
        
        Rectangle rdst = dstRect;
        if(rdst.x2 == dstRect.x1)
            rdst.x2 = texture->getWidth() + dstRect.x1;
        if(dstRect.y2 == dstRect.y1)
            rdst.y2 = texture->getHeight() + dstRect.y1;
        
        obj.buildVertices(rdst,
                          0.f,
                          color);
                          
        mRenderQueue.insert(obj);
    }
    
    void SpriteBatch::draw(const TexturePtr& texture, const Rectangle& srcRect, const Rectangle& dstRect, float rot, const Color& color) {
        TextureObject obj(texture, 0.f);
        
        obj.srcRect = srcRect;
        if(obj.srcRect.x2 == 0 && srcRect.y2 == 0) {
            obj.srcRect.x2 = texture->getWidth();
            obj.srcRect.y2 = texture->getHeight();
        }
        
        Rectangle rdst = dstRect;
        if(dstRect.x2 == dstRect.x1)
            rdst.x2 = texture->getWidth() + dstRect.x1;
        if(dstRect.y2 == dstRect.y1)
            rdst.y2 = texture->getHeight() + dstRect.y1;
        
        obj.buildVertices(rdst,
                          rot,
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
    
    void SpriteBatch::draw(const TexturePtr& texture, const Rectangle& srcRect, const Rectangle& dstRect, float rot, float layerDepth, const Color& color) {
        TextureObject obj(texture, layerDepth);
        
        obj.srcRect = srcRect;
        if(obj.srcRect.x2 == 0 && srcRect.y2 == 0) {
            obj.srcRect.x2 = texture->getWidth();
            obj.srcRect.y2 = texture->getHeight();
        }
        
        Rectangle rdst = dstRect;
        if(dstRect.x2 == dstRect.x1)
            rdst.x2 = texture->getWidth() + dstRect.x1;
        if(dstRect.y2 == dstRect.y1)
            rdst.y2 = texture->getHeight() + dstRect.y1;
        
        obj.buildVertices(rdst,
                          rot,
                          color);
        
        mRenderQueue.insert(obj);
    }
    
    void SpriteBatch::draw(const TexturePtr& texture, float x, float y, const Rectangle& src, const Color& color) {
        draw(texture, x, y, src, 0.f, color);
    }
    
    void SpriteBatch::draw(const TexturePtr& texture, float x, float y, const Rectangle& srcRect, float layerDepth, const Color& color) {        
        TextureObject obj(texture, layerDepth);
        
        obj.srcRect = srcRect;
        if(obj.srcRect.x2 == 0 && srcRect.y2 == 0) {
            obj.srcRect.x2 = texture->getWidth();
            obj.srcRect.y2 = texture->getHeight();
        }

        obj.buildVertices(x,
                          y,
                          0.f,
                          0.f,
                          0.f, 
                          1.f, 
                          1.f,
                          color);
    }
    
} // namespace ukn
