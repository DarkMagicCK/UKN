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
#include "UKN/2DHelper.h"
#include "UKN/BlendStateObject.h"
#include "UKN/DepthStencilStateObject.h"
#include "UKN/Shader.h"

#include "mist/Singleton.h"
#include "mist/Profiler.h"

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
                u1 = descriptor.source_rect.x() / texture->width();
                u2 = descriptor.source_rect.right() / texture->width();
                v1 = descriptor.source_rect.y() / texture->height();
                v2 = descriptor.source_rect.bottom() / texture->height();
                
                if(descriptor.hflip)
                    std::swap(v1, v2);
                
                if(descriptor.vflip)
                    std::swap(u1, u2);
            }
        }
        
        tmpVert[0].uv.set(u1, v1);
        tmpVert[1].uv.set(u2, v1);
        tmpVert[2].uv.set(u2, v2);
        tmpVert[3].uv.set(u1, v2);
        
        if(descriptor.dest_rect.isEmpty()) {
            float tx1 = -descriptor.center.x() * descriptor.scale.x();
            float ty1 = -descriptor.center.y() * descriptor.scale.y();
            float tx2, ty2;
            
            if(descriptor.source_rect.isEmpty() && texture) {
                tx2 = (texture->width() - descriptor.center.x()) * descriptor.scale.x();
                ty2 = (texture->height() - descriptor.center.y()) * descriptor.scale.y();
            } else {
                tx2 = (descriptor.source_rect.width() - descriptor.center.x()) * descriptor.scale.x();
                ty2 = (descriptor.source_rect.height() - descriptor.center.y()) * descriptor.scale.y();
            }
                        
            if(descriptor.rotation != 0.0f) {
                float cost = cosf(descriptor.rotation);
                float sint = sinf(descriptor.rotation);
                
                tmpVert[0].xyz.set(tx1*cost - ty1*sint + descriptor.position.x(),
                                   tx1*sint + ty1*cost + descriptor.position.y(),
                                   descriptor.layer_depth);
                tmpVert[1].xyz.set(tx2*cost - ty1*sint + descriptor.position.x(),
                                   tx2*sint + ty1*cost + descriptor.position.y(),
                                   descriptor.layer_depth);
                tmpVert[2].xyz.set(tx2*cost - ty2*sint + descriptor.position.x(),
                                   tx2*sint + ty2*cost + descriptor.position.y(),
                                   descriptor.layer_depth);
                tmpVert[3].xyz.set(tx1*cost - ty2*sint + descriptor.position.x(),
                                   tx1*sint + ty2*cost + descriptor.position.y(),
                                   descriptor.layer_depth);
   
            }
            else {
                tmpVert[0].xyz.set(tx1 + descriptor.position.x(),
                                   ty1 + descriptor.position.y(),
                                   descriptor.layer_depth);
                tmpVert[1].xyz.set(tx2 + descriptor.position.x(),
                                   tmpVert[0].xyz.y(),
                                   descriptor.layer_depth);
                tmpVert[2].xyz.set(tmpVert[1].xyz.x(),
                                   ty2 + descriptor.position.y(),
                                   descriptor.layer_depth);
                tmpVert[3].xyz.set(tmpVert[0].xyz.x(),
                                   tmpVert[2].xyz.y(),
                                   descriptor.layer_depth);
            }
            
        } else {
            tmpVert[0].xyz.set(descriptor.dest_rect.x1,
                               descriptor.dest_rect.y1,
                               descriptor.layer_depth);
            tmpVert[1].xyz.set(descriptor.dest_rect.x2,
                               descriptor.dest_rect.y1,
                               descriptor.layer_depth);
            tmpVert[2].xyz.set(descriptor.dest_rect.x2,
                               descriptor.dest_rect.y2,
                               descriptor.layer_depth);
            tmpVert[3].xyz.set(descriptor.dest_rect.x1,
                               descriptor.dest_rect.y2,
                               descriptor.layer_depth);
        }
        
        for(int i=0; i<4; ++i) {
            tmpVert[i].color = descriptor.color.toABGR();
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
        GraphicFactory& gf = Context::Instance().getGraphicFactory();

        mRenderBuffer = gf.createRenderBuffer();
        mist_assert(mRenderBuffer);
        
        mVertexBuffer = gf.createVertexBuffer(GraphicBuffer::WriteOnly, 
                                              GraphicBuffer::Dynamic, 
                                              1024, 
                                              0, 
                                              Vertex2D::Format());
        mist_assert(mVertexBuffer);
        
        mRenderBuffer->bindVertexStream(mVertexBuffer, Vertex2D::Format());
        
        mBegan = false;
        mBatchRendering = false;
        mCurrMode = SBS_None;

        mTransformMatrix = Matrix4();
    }
    
    SpriteBatch::~SpriteBatch() {
        
    }
    
    const UknString& SpriteBatch::getName() const {
        static UknString name(L"SpriteBatch");
        return name;
    }
    
    Box SpriteBatch::getBound() const {
        return mBoundingBox;
    }
    
    RenderBufferPtr SpriteBatch::getRenderBuffer() const {
        return mRenderBuffer;
    }
    
    void SpriteBatch::onRenderBegin() {
        Ukn2DHelper::Instance().setupMat(mTransformMatrix);
    }
    
    void SpriteBatch::onRenderEnd() {
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
        TexturePtr currTexture;
        uint32 renderCount = 0;
        
        Vertex2D* vertices = (Vertex2D*)mVertexBuffer->map();
        Ukn2DHelper& helper = Ukn2DHelper::Instance();

        helper.setupMat();
        while(it != mRenderQueue.end()) { 
            if(currTexture &&
                it->texture != currTexture) {
                mVertexBuffer->unmap();
                
                helper.bindTexture(currTexture);
                mRenderBuffer->setVertexCount(renderCount);
                gd.renderBuffer(helper.getEffect()->getTechnique(0),
                                mRenderBuffer);

                renderCount = 0;
                vertices = (Vertex2D*)mVertexBuffer->map();
            }
            memcpy(vertices + renderCount, &it->vertices[0], sizeof(Vertex2D)*6);
            renderCount += 6;
            
            currTexture = it->texture;

            ++it;
            if(it == mRenderQueue.end()) {
                mVertexBuffer->unmap();
                
                helper.bindTexture(currTexture);
                mRenderBuffer->setVertexCount(renderCount);
                gd.renderBuffer(helper.getEffect()->getTechnique(0),
                                mRenderBuffer);

                renderCount = 0;
            }
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
            
            Ukn2DHelper& helper = Ukn2DHelper::Instance();

            helper.bindTexture(mRenderQueue[mCurrentBatchIndex].texture);

            gd.renderBuffer(helper.getEffect()->getTechnique(0),
                            mRenderBuffer);

            onRenderEnd();
        }
        mBatchRendering = false;
    }
    
    void SpriteBatch::updateBoundingBox(float x, float y, float z) {
        if(mBoundingBox.lower_left.x() > x) {
            mBoundingBox.lower_left.x() = x;
        }
        if(mBoundingBox.lower_left.y() > y) {
            mBoundingBox.lower_left.y() = y;
        }
        if(mBoundingBox.upper_right.x() < x) {
            mBoundingBox.upper_right.x() = x;
        }
        if(mBoundingBox.upper_right.y() < y) {
            mBoundingBox.upper_right.y() = y;
        }
        if(mBoundingBox.lower_left.z() > z) {
            mBoundingBox.lower_left.z() = z;
        }
        if(mBoundingBox.upper_right.z() < z) {
            mBoundingBox.upper_right.z() = z;
        }
    }
    
    void SpriteBatch::draw(const TexturePtr& texture, const Vector2& pos, const Color& color) {
        draw(texture, pos, 0.f, color);
    }
    
    void SpriteBatch::draw(const TexturePtr& texture, const mist::Rectangle& dstRect, const Color& color) {
        draw(texture, dstRect, 0.f, color);
    }
    
    void SpriteBatch::draw(const TexturePtr& texture, const mist::Rectangle& srcRect, const mist::Rectangle& dstRect, const Color& color) {
        if(!mBegan) {
            MIST_THROW_EXCEPTION(L"ukn::SpriteBatch::draw: begin must be called before any draw function");
        }
        
        TextureObject obj(texture);
               
        SpriteDescriptor descriptor;
        descriptor.source_rect = srcRect;
        descriptor.dest_rect = dstRect;
        descriptor.layer_depth = 0.0;
        descriptor.color = color;
        obj.buildVertices(descriptor);
                          
        mRenderQueue.push_back(obj);
    }
    
    void SpriteBatch::draw(const TexturePtr& texture, const Vector2& pos, const Vector2& center, float rot, const Vector2& scale, const Color& color) {
        draw(texture, pos, center, rot, scale, 0.f, color);
    }
    
    void SpriteBatch::draw(const TexturePtr& texture, const Vector2& pos, float layerDepth, const Color& color) {
        if(!mBegan) {
            MIST_THROW_EXCEPTION(L"ukn::SpriteBatch::draw: begin must be called before any draw function");
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
    
    void SpriteBatch::draw(const TexturePtr& texture, const mist::Rectangle& dstRect, float layerDepth, const Color& color) {
        if(!mBegan) {
            MIST_THROW_EXCEPTION(L"ukn::SpriteBatch::draw: begin must be called before any draw function");
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
            MIST_THROW_EXCEPTION(L"ukn::SpriteBatch::draw: begin must be called before any draw function");
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
        
    void SpriteBatch::draw(const TexturePtr& texture, const Vector2& pos, const mist::Rectangle& src, const Vector2& center, float rot, const Vector2& scale, const Color& color) {
        draw(texture, pos, src, center, rot, scale, 0.f, color);
    }
    
    void SpriteBatch::draw(const TexturePtr& texture, const SpriteDescriptor& descriptor) {
        if(!mBegan) {
            MIST_THROW_EXCEPTION(L"ukn::SpriteBatch::draw: begin must be called before any draw function");
        }
        
        TextureObject obj(texture);
        obj.buildVertices(descriptor);
        
        mRenderQueue.push_back(obj);
    }
    
    void SpriteBatch::draw(const TexturePtr& texture, const Vector2& pos, const mist::Rectangle& srcRect, const Vector2& center, float rot, const Vector2& scale, float layerDepth, const Color& color) {        
        if(!mBegan) {
            MIST_THROW_EXCEPTION(L"ukn::SpriteBatch::draw: begin must be called before any draw function");
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
                gd.setBlendState(BlendStateObject::AlphaBlend());
                break;
                
            case SBB_Additive:
                gd.setBlendState(BlendStateObject::Addtive());
                break;
                
            case SBB_None:
                gd.setBlendState(BlendStateObject::BlendOff());
                break;
        }
        gd.setDepthStencilState(DepthStencilStateObject::None());
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

    void SpriteBatch::drawQuad(const EffectTechniquePtr& technique, const Vector2& upper, const Vector2& lower) {
        Vertex2D* vertices = (Vertex2D*)mVertexBuffer->map();
        
        if(vertices) {
            Vertex2D vertex;
            vertex.xyz.set(lower.x(), lower.y(), 0);
            vertex.uv.set(1, 1);
            *vertices++ = vertex; 
        
            vertex.xyz.x() = upper.x();
            vertex.uv.set(0, 1);
            *vertices++ = vertex; 
        
            vertex.xyz.y() = upper.y();
            vertex.uv.set(0, 0);
            *vertices++ = vertex; 
            *vertices++ = vertex; 

            vertex.xyz.x() = lower.x();
            vertex.uv.set(1, 0);
            *vertices++ = vertex; 

            vertex.xyz.y() = lower.y();
            vertex.uv.set(1, 1);
            *vertices++ = vertex; 
        
            mVertexBuffer->unmap();

            mRenderBuffer->setVertexCount(6);
            Context::Instance().getGraphicFactory().getGraphicDevice().renderBuffer(technique, mRenderBuffer);
        } else {
            log_error(L"SpriteBatch::drawQuad: error mapping data");
        }
    }
    
    SpriteBatch& SpriteBatch::DefaultObject() {
        static SpriteBatchPtr instance = Context::Instance().getGraphicFactory().createSpriteBatch();
        return *instance;
    }
    
} // namespace ukn
