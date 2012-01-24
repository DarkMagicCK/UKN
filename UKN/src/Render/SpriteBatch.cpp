//
//  SpriteBatch.cpp
//  Project Unknown
//
//  Created by Robert Bu on 12/7/11.
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
#include "UKN/Singleton.h"
#include "UKN/Profiler.h"

namespace ukn {
    
    SpriteDescriptor::SpriteDescriptor():
    scale(Vector2(1.f, 1.f)),
    rotation(0.f),
    layer_depth(0.f),
    color(color::White),
    vflip(false),
    hflip(false) {
        
    }
    
    SpriteDescriptor::SpriteDescriptor(const Vector2& pos,
                                       const Vector2& ct,
                                       float rot,
                                       const Vector2& sc, 
                                       Color clr,
                                       float depth,
                                       bool vf,
                                       bool hf):
    position(pos),
    scale(sc),
    center(ct),
    rotation(rot),
    layer_depth(depth),
    color(clr),
    vflip(vf),
    hflip(hf) {
        
    }
    
    SpriteBatch::TextureObject::TextureObject():
    layer_depth(0.f) { 
     
    }
        
    SpriteBatch::TextureObject::TextureObject(const TexturePtr& tex):
    texture(tex),
    layer_depth(0.f) {
    }
    
    void SpriteBatch::TextureObject::buildVertices(const SpriteDescriptor& descriptor) {
        Vertex2D tmpVert[4];
        
        float u1 = 0, u2 = 1, v1 = 0, v2 = 1;
        if(texture) {
            if(!descriptor.source_rect.isEmpty()) {
                u1 = descriptor.source_rect.x() / texture->getWidth();
                u2 = descriptor.source_rect.right() / texture->getWidth();
                v1 = descriptor.source_rect.y() / texture->getHeight();
                v2 = descriptor.source_rect.bottom() / texture->getHeight();
                
                if(descriptor.hflip)
                    std::swap(v1, v2);
                
                if(descriptor.vflip)
                    std::swap(u1, u2);
            }
        }
        
        tmpVert[0].u = u1; tmpVert[0].v = v1; tmpVert[0].z = descriptor.layer_depth;
        tmpVert[1].u = u2; tmpVert[1].v = v1; tmpVert[0].z = descriptor.layer_depth;
        tmpVert[2].u = u2; tmpVert[2].v = v2; tmpVert[0].z = descriptor.layer_depth;
        tmpVert[3].u = u1; tmpVert[3].v = v2; tmpVert[0].z = descriptor.layer_depth;
        
        if(descriptor.dest_rect.isEmpty()) {
            float tx1 = -descriptor.center.x * descriptor.scale.x;
            float ty1 = -descriptor.center.y * descriptor.scale.y;
            float tx2, ty2;
            
            if(descriptor.source_rect.isEmpty()) {
                tx2 = (texture->getWidth() - descriptor.center.x) * descriptor.scale.x;
                ty2 = (texture->getHeight() - descriptor.center.y) * descriptor.scale.y;
            } else {
                tx2 = (descriptor.source_rect.width() - descriptor.center.x) * descriptor.scale.x;
                ty2 = (descriptor.source_rect.height() - descriptor.center.y) * descriptor.scale.y;
            }
                        
            if(descriptor.rotation != 0.0f) {
                float cost = cosf(descriptor.rotation);
                float sint = sinf(descriptor.rotation);
                
                tmpVert[0].x  = tx1*cost - ty1*sint + descriptor.position.x;
                tmpVert[1].x  = tx2*cost - ty1*sint + descriptor.position.x;
                tmpVert[2].x  = tx2*cost - ty2*sint + descriptor.position.x;
                tmpVert[3].x  = tx1*cost - ty2*sint + descriptor.position.x;
                
                tmpVert[0].y  = tx1*sint + ty1*cost + descriptor.position.y;	
                tmpVert[1].y  = tx2*sint + ty1*cost + descriptor.position.y;	
                tmpVert[2].y  = tx2*sint + ty2*cost + descriptor.position.y;	
                tmpVert[3].y  = tx1*sint + ty2*cost + descriptor.position.y;
            }
            else {
                tmpVert[0].x = tx1 + descriptor.position.x; 
                tmpVert[1].x = tx2 + descriptor.position.x; 
                tmpVert[2].x = tmpVert[1].x; 
                tmpVert[3].x = tmpVert[0].x; 
                
                tmpVert[0].y = ty1 + descriptor.position.y;
                tmpVert[1].y = tmpVert[0].y;
                tmpVert[2].y = ty2 + descriptor.position.y;
                tmpVert[3].y = tmpVert[2].y;
            }
            
        } else {
            tmpVert[0].x = descriptor.dest_rect.x1; 
            tmpVert[0].y = descriptor.dest_rect.y1; 
            
            tmpVert[1].x = descriptor.dest_rect.x2; 
            tmpVert[1].y = descriptor.dest_rect.y1; 
            
            tmpVert[2].x = descriptor.dest_rect.x2; 
            tmpVert[2].y = descriptor.dest_rect.y2; 
            
            tmpVert[3].x = descriptor.dest_rect.x1; 
            tmpVert[3].y = descriptor.dest_rect.y2; 
        }
        
        for(int i=0; i<4; ++i) {
            tmpVert[i].color = descriptor.color.toHWColor();
        }
        
        vertices[0] = tmpVert[0];
        vertices[1] = tmpVert[1];
        vertices[2] = tmpVert[2];
        
        vertices[3] = tmpVert[2];
        vertices[4] = tmpVert[3];
        vertices[5] = tmpVert[0];
        
        layer_depth = descriptor.layer_depth;
    }
         
    bool SpriteBatch::TextureObject::operator<(const SpriteBatch::TextureObject& rhs) const {
        return this->layer_depth < rhs.layer_depth;
    }
    
    bool SpriteBatch::TextureObject::operator>(const SpriteBatch::TextureObject& rhs) const {
        return this->layer_depth > rhs.layer_depth;
    }
    
    bool SpriteBatch::TextureObject::operator!=(const SpriteBatch::TextureObject& rhs) const {
        return this->layer_depth != rhs.layer_depth;
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
        
        mBegan = false;
        mBatchRendering = false;
        mCurrMode = SBS_None;
    }
    
    SpriteBatch::~SpriteBatch() {
        
    }
    
    const String& SpriteBatch::getName() const {
        static String name(L"SpriteBatch");
        return name;
    }
    
    Box SpriteBatch::getBound() const {
        return mBoundingBox;
    }
    
    RenderBufferPtr SpriteBatch::getRenderBuffer() const {
        return mRenderBuffer;
    }
    
    void SpriteBatch::onRenderBegin() {
        GraphicDevice& gd = Context::Instance().getGraphicFactory().getGraphicDevice();
        
        gd.pushProjectionMatrix();
        gd.pushViewMatrix();
        
        Camera* cam = gd.getCurrFrameBuffer()->getViewport().camera.get();
        
        gd.setProjectionMatrix(cam->getProjMatrix());
        gd.setViewMatrix(mTransformMatrix * cam->getViewMatrix());
    }
    
    void SpriteBatch::onRenderEnd() {
        GraphicDevice& gd = Context::Instance().getGraphicFactory().getGraphicDevice();

        gd.bindTexture(TexturePtr());
        
        gd.popProjectionMatrix();
        gd.popViewMatrix();
        
        if(!mBatchRendering)
            mRenderQueue.clear();
        else  
            mRenderQueue.erase(mRenderQueue.begin()+mCurrentBatchIndex, mRenderQueue.end());
    }
    
    Matrix4& SpriteBatch::getTransformMatrix() {
        return mTransformMatrix;
    }
    
    const Matrix4& SpriteBatch::getTransformMatrix() const {
        return mTransformMatrix;
    }
    
    void SpriteBatch::onRender() {
        GraphicDevice& gd = Context::Instance().getGraphicFactory().getGraphicDevice();
        
        RenderQueue::iterator it = mRenderQueue.begin();
        while(it != mRenderQueue.end()) { 
            Vertex2D* vertices = (Vertex2D*)mVertexBuffer->map();
            memcpy(vertices, &it->vertices[0], sizeof(Vertex2D)*6);
            mVertexBuffer->unmap();
            
            gd.bindTexture(it->texture);
            gd.onRenderBuffer(mRenderBuffer);
            
            ++it;
        }
    }
    
    void SpriteBatch::render() {
        onRenderBegin();
        
        onRender();
        
        onRenderEnd();
    }
    
    void SpriteBatch::startBatch() {
        mBatchRendering = true;
        mCurrentBatchIndex = mRenderQueue.size();
    }
    
    void SpriteBatch::endBatch() {        
        if(mCurrentBatchIndex < mRenderQueue.size()) {
            // alloc vertex buffer
            mVertexBuffer->resize((uint32)(mRenderQueue.size() - mCurrentBatchIndex) * 6);
            Vertex2D* vertices = (Vertex2D*)mVertexBuffer->map();
            for(size_t i = mCurrentBatchIndex; i < mRenderQueue.size(); ++i) {
                memcpy(vertices + i*6, &mRenderQueue[i].vertices[0], sizeof(Vertex2D)*6);
            }
            mVertexBuffer->unmap();
            
            onRenderBegin();
            GraphicDevice& gd = Context::Instance().getGraphicFactory().getGraphicDevice();

            gd.bindTexture(mRenderQueue[mCurrentBatchIndex].texture);
            gd.onRenderBuffer(mRenderBuffer);
            
            onRenderEnd();
        }
        mBatchRendering = false;
    }
    
    void SpriteBatch::updateBoundingBox(float x, float y, float z) {
        if(mBoundingBox.min.x > x) {
            mBoundingBox.min.x = x;
        }
        if(mBoundingBox.min.y > y) {
            mBoundingBox.min.y = y;
        }
        if(mBoundingBox.max.x < x) {
            mBoundingBox.max.x = x;
        }
        if(mBoundingBox.max.y < y) {
            mBoundingBox.max.y = y;
        }
        if(mBoundingBox.min.z > z) {
            mBoundingBox.min.z = z;
        }
        if(mBoundingBox.max.z < z) {
            mBoundingBox.max.z = z;
        }
    }
    
    void SpriteBatch::draw(const TexturePtr& texture, const Vector2& pos, const Color& color) {
        draw(texture, pos, 0.f);
    }
    
    void SpriteBatch::draw(const TexturePtr& texture, const Rectangle& dstRect, const Color& color) {
        draw(texture, dstRect, 0.f);
    }
    
    void SpriteBatch::draw(const TexturePtr& texture, const Rectangle& srcRect, const Rectangle& dstRect, const Color& color) {
        draw(texture, srcRect, dstRect, 0.f);
    }
    
    void SpriteBatch::draw(const TexturePtr& texture, const Vector2& pos, const Vector2& center, float rot, const Vector2& scale, const Color& color) {
        draw(texture, pos, center, rot, scale, 0.f);
    }
    
    void SpriteBatch::draw(const TexturePtr& texture, const Vector2& pos, float layerDepth, const Color& color) {
        if(!mBegan) {
            UKN_THROW_EXCEPTION("ukn::SpriteBatch::draw: begin must be called before any draw function");
        }
        
        TextureObject obj(texture);
        obj.buildVertices(SpriteDescriptor(pos,
                                           Vector2(0.f, 0.f),
                                           0.f,
                                           Vector2(1.f, 1.f),
                                           color,
                                           layerDepth));
            
        mRenderQueue.push_back(obj);
    }
    
    void SpriteBatch::draw(const TexturePtr& texture, const Rectangle& dstRect, float layerDepth, const Color& color) {
        if(!mBegan) {
            UKN_THROW_EXCEPTION("ukn::SpriteBatch::draw: begin must be called before any draw function");
        }
        
        TextureObject obj(texture);
               
        SpriteDescriptor descriptor;
        descriptor.dest_rect = dstRect;
        descriptor.layer_depth = layerDepth;
        descriptor.color = color;
        obj.buildVertices(descriptor);
                          
        mRenderQueue.push_back(obj);
        
        for(int i=0; i<6; ++i);
    }
    
    void SpriteBatch::draw(const TexturePtr& texture, const Vector2& pos, const Vector2& center, float rot, const Vector2& scale, float layerDepth, const Color& color) {
        if(!mBegan) {
            UKN_THROW_EXCEPTION("ukn::SpriteBatch::draw: begin must be called before any draw function");
        }
        TextureObject obj(texture);
        
        obj.buildVertices(SpriteDescriptor(pos,
                                           center,
                                           rot,
                                           scale,
                                           color,
                                           layerDepth));
        
        mRenderQueue.push_back(obj);
    }
        
    void SpriteBatch::draw(const TexturePtr& texture, const Vector2& pos, const Rectangle& src, const Vector2& center, float rot, const Vector2& scale, const Color& color) {
        draw(texture, pos, src, center, rot, scale, 0.f, color);
    }
    
    void SpriteBatch::draw(const TexturePtr& texture, const SpriteDescriptor& descriptor) {
        if(!mBegan) {
            UKN_THROW_EXCEPTION("ukn::SpriteBatch::draw: begin must be called before any draw function");
        }
        
        TextureObject obj(texture);
        obj.buildVertices(descriptor);
        
        mRenderQueue.push_back(obj);
    }
    
    void SpriteBatch::draw(const TexturePtr& texture, const Vector2& pos, const Rectangle& srcRect, const Vector2& center, float rot, const Vector2& scale, float layerDepth, const Color& color) {        
        if(!mBegan) {
            UKN_THROW_EXCEPTION("ukn::SpriteBatch::draw: begin must be called before any draw function");
        }
        
        TextureObject obj(texture);
        
        SpriteDescriptor descriptor(pos,
                                    center,
                                    rot,
                                    scale,
                                    color,
                                    layerDepth);
        
        descriptor.source_rect = srcRect;
        obj.buildVertices(descriptor);
        
        mRenderQueue.push_back(obj);
    }
    
    void SpriteBatch::begin(SpriteBatchSortMode mode) {
        this->begin(SBB_Alpha, mode, Matrix4());
        
    }
    
    void SpriteBatch::begin(SpriteBatchSortMode mode, const Matrix4& transformMat) {
        this->begin(SBB_Alpha, mode, transformMat);
    }
    
    void SpriteBatch::begin(SpriteBatchBlendMode blend, SpriteBatchSortMode mode, const Matrix4& transformMat) {
        mBegan = true;
        mCurrMode = mode;
        mTransformMatrix = transformMat;
        
        GraphicDevice& gd = Context::Instance().getGraphicFactory().getGraphicDevice();

        switch(blend) {
            case SBB_Alpha:
                gd.setRenderState(RS_Blend, RSP_Enable);
                gd.setRenderState(RS_SrcAlpha, RSP_BlendFuncOneMinusSrcAlpha);
                gd.setRenderState(RS_ColorOp, RSP_ColorOpModulate);
                break;
                
            case SBB_Addictive:
                gd.setRenderState(RS_Blend, RSP_Enable);
                gd.setRenderState(RS_SrcAlpha, RSP_BlendFuncOneMinusSrcAlpha);
                gd.setRenderState(RS_ColorOp, RSP_ColorOpAdd);
                break;
                
            case SBB_None:
                gd.setRenderState(RS_Blend, RSP_Disable);
                break;
        }
    }
    
    void SpriteBatch::end() {
        switch(mCurrMode) {
            case SBS_FrontToBack:
                std::sort(mRenderQueue.begin(), mRenderQueue.end(), std::less<TextureObject>());
                break;
            case SBS_BackToFront:
                std::sort(mRenderQueue.begin(), mRenderQueue.end(), std::greater<TextureObject>());
                break;
            case SBS_Deffered:
            case SBS_None:
                break;
        }
        
        render();

        mBegan = false;
        mTransformMatrix = Matrix4();
    }
    
    SpriteBatch& SpriteBatch::DefaultObject() {
        static SpriteBatch* instance = Context::Instance().getGraphicFactory().createSpriteBatch();
        return *instance;
    }
    
} // namespace ukn
