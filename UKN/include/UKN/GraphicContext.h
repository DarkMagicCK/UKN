//
//  RenderHelper.h
//  Project Unknown
//
//  Created by Ruiwei Bu on 1/20/12.
//  Copyright (c) 2012 heizi. All rights reserved.
//

#ifndef Project_Unknown_RenderHelper_h
#define Project_Unknown_RenderHelper_h

#include "mist/Platform.h"
#include "mist/Color.h"

#include "UKN/PreDeclare.h"
#include "UKN/Renderable.h"
#include "UKN/GraphicBuffer.h"
#include "UKN/GraphicSettings.h"
#include "UKN/Vertex.h"

namespace ukn {
    
    /* rendering utilities */
    class UKN_API GraphicContext: public virtual Object {
    public:
        GraphicContext();
        virtual ~GraphicContext();

    public:
        void setDrawColor(const ukn::Color& clr);
        void setFillColor(const ukn::Color& clr);
        const ukn::Color& getDrawColor() const;
        const ukn::Color& getFillColor() const;
        
        void begin();
        
        void moveTo(float x, float y, float z);
        void lineTo(float x, float y, float z);
        
        void drawCircle(float x, float y, float z, float r);
        void fillCircle(float x, float y, float z, float r);
        
        void drawRect(const ukn::Rectangle& rect, float z);
        void fillRect(const ukn::Rectangle& rect, float z);

        // render a quad, usually for texture
        void drawQuad(const float2& upper_left, const float2& lower_right);

        void end();
        
    protected:
        virtual void onRender();
        virtual void onRenderBegin();
        virtual void onRenderEnd();
        
    protected:
        void addSeg(RenderMode mode);
        
        SharedPtr<MemoryGraphicBuffer<Vertex2D> > mVertexBuffer;
        RenderBufferPtr mRenderBuffer;
        
        ukn::Color mDrawColor;
        ukn::Color mFillColor;
    
        struct SegInfo {
            uint32 start_index;
            uint32 length;
            RenderMode render_mode;
        };
        typedef Array<SegInfo> SegVector;
        SegVector mSegments;
        
        float mCurrX, mCurrY, mCurrZ;
        uint32 mPrevSegIndex;
        RenderMode mCurrRenderMode;
    };
    
    typedef SharedPtr<GraphicContext> GraphicContextPtr;

} // namespace ukn

#endif
