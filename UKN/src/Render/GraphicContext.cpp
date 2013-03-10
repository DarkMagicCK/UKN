//
//  RenderHelper.cpp
//  Project Unknown
//
//  Created by Ruiwei Bu on 1/21/12.
//  Copyright (c) 2012 heizi. All rights reserved.
//

#include "UKN/GraphicContext.h"
#include "UKN/GraphicBuffer.h"
#include "UKN/Vertex.h"
#include "UKN/GraphicDevice.h"
#include "UKN/GraphicFactory.h"
#include "UKN/Context.h"
#include "UKN/RenderBuffer.h"
#include "UKN/2DHelper.h"
#include "UKN/Shader.h"

namespace ukn {
    
    GraphicContext::GraphicContext() {
        GraphicFactory& gf = Context::Instance().getGraphicFactory();
        
        mRenderBuffer = gf.createRenderBuffer();
        mVertexBuffer = new MemoryGraphicBuffer<Vertex2D>(0, 0);
        mRenderBuffer->bindVertexStream(mVertexBuffer, Vertex2D::Format());
    }
    
    GraphicContext::~GraphicContext() {
        
    }
    
    void GraphicContext::setDrawColor(const ukn::Color& clr) {
        mDrawColor = clr;
    }
    
    void GraphicContext::setFillColor(const ukn::Color& clr) {
        mFillColor = clr;
    }
    
    const ukn::Color& GraphicContext::getDrawColor() const {
        return mDrawColor;
    }
    
    const ukn::Color& GraphicContext::getFillColor() const {
        return mFillColor;
    }
    
    void GraphicContext::addSeg(RenderMode mode) {
        if(mVertexBuffer->count() == mPrevSegIndex) {
            return;
        }
        GraphicContext::SegInfo seg;
        seg.start_index = mPrevSegIndex;
        seg.length = mVertexBuffer->count() - mPrevSegIndex;
        seg.render_mode = mode;
        
        mSegments.push_back(seg);
        
        mPrevSegIndex = mVertexBuffer->count();
    }
    
    void GraphicContext::begin() {
        mSegments.clear();
        mVertexBuffer->clear();
        
        mCurrX = mCurrY = mCurrZ = 0.0;
        mPrevSegIndex = 0;
        mCurrRenderMode = RM_Point;
    }
    
    void GraphicContext::end() {
        addSeg(mCurrRenderMode);
        
        onRenderBegin();
        
        onRender();
        
        onRenderEnd();
    }
    
    void GraphicContext::moveTo(float x, float y, float z) {
        mCurrX = x;
        mCurrY = y;
        mCurrZ = z;
    }
    
    void GraphicContext::lineTo(float x, float y, float z) {
        Vertex2D vertex;
        vertex.xyz.set(mCurrX, mCurrY, mCurrZ);
        vertex.color = mDrawColor.toRGBA();
        
        mVertexBuffer->push(vertex);
        
        vertex.xyz.set(x, y, z);
        mVertexBuffer->push(vertex);
        
        mCurrX = x;
        mCurrY = y;
        mCurrZ = z;
        
        mCurrRenderMode = RM_Line;
    }
    
    void GraphicContext::drawCircle(float x, float y, float z, float r) {
        addSeg(mCurrRenderMode);
        
        uint32 seg_size = uint32(ceil(r / 100.f) * 100);
        double step = mist::math::degree_to_radius(360.f / seg_size);
        for(uint32 i = 0; i < seg_size; ++i) {
            Vertex2D vertex;
            vertex.xyz.set((float)(x + r * cos(i * step)),
                           (float)(y + r * sin(i * step)),
                           z);
            vertex.color = mDrawColor.toRGBA();
            
            mVertexBuffer->push(vertex);
        }
        
        addSeg(RM_LineLoop);
    }
    
    void GraphicContext::fillCircle(float x, float y, float z, float r) {
        addSeg(mCurrRenderMode);

        Vertex2D vertex;
        vertex.xyz.set(x, y, z);
        vertex.color = mFillColor.toRGBA();
        
        uint32 seg_size = uint32(ceil(r / 100.f) * 100);
        double step = mist::math::degree_to_radius(360.f / seg_size);
        for(uint32 i = 0; i <= seg_size; ++i) {
            Vertex2D vertex;
            vertex.xyz.set((float)(x + r * cos(i * step)),
                           (float)(y + r * sin(i * step)),
                           z);
            vertex.color = mFillColor.toRGBA();
            
            mVertexBuffer->push(vertex);
        }
        
        addSeg(RM_TriangleFan);
    }
    
    void GraphicContext::drawRect(const ukn::Rectangle& rect, float z) {
        addSeg(mCurrRenderMode);

        Vertex2D vertex;
        vertex.xyz.set(rect.left(),
                       rect.top(),
                       z);
        vertex.color = mDrawColor.toRGBA();
        
        mVertexBuffer->push(vertex);
        
        vertex.xyz.x() = rect.right();
        mVertexBuffer->push(vertex);
        
        vertex.xyz.y() = rect.bottom();
        mVertexBuffer->push(vertex);
        
        vertex.xyz.x() = rect.left();
        mVertexBuffer->push(vertex);
        
        addSeg(RM_LineLoop);
    }
    
    void GraphicContext::fillRect(const ukn::Rectangle& rect, float z) {
        addSeg(mCurrRenderMode);

        Vertex2D vertex;
        vertex.xyz.set(rect.left(),
                       rect.top(),
                       z);
        vertex.color = mFillColor.toRGBA();
        
        mVertexBuffer->push(vertex);
        
        vertex.xyz.x() = rect.right();
        mVertexBuffer->push(vertex);
        
        vertex.xyz.y() = rect.bottom();
        mVertexBuffer->push(vertex);
        
        mVertexBuffer->push(vertex);

        vertex.xyz.x() = rect.left();
        mVertexBuffer->push(vertex);

        vertex.xyz.y() = rect.top();
        mVertexBuffer->push(vertex);
        
        addSeg(RM_Triangle);
    }

    void GraphicContext::drawQuad(const float2& upper_left, const float2& lower_right) {
        this->fillRect(ukn::Rectangle(upper_left, lower_right), 0);
    }
    
    void GraphicContext::onRenderBegin() {
        
    }
    
    void GraphicContext::onRenderEnd() {
        
    }
    
    void GraphicContext::onRender() {
        if(!mSegments.empty()) {
            GraphicDevice& gd = Context::Instance().getGraphicFactory().getGraphicDevice();

            MIST_FOR_EACH(SegInfo& seg, mSegments) {
                mRenderBuffer->setRenderMode(seg.render_mode);
                mRenderBuffer->setVertexCount(seg.length);
                mRenderBuffer->setVertexStartIndex(seg.start_index);
                
                gd.renderBuffer(Ukn2DHelper::Instance().getEffect()->getTechnique(0),
                                mRenderBuffer);
            }
            
            mSegments.clear();
        }
    }
    
} // namespace ukn
