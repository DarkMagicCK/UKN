//
//  RenderHelper.h
//  Project Unknown
//
//  Created by Ruiwei Bu on 1/20/12.
//  Copyright (c) 2012 heizi. All rights reserved.
//

#ifndef Project_Unknown_RenderHelper_h
#define Project_Unknown_RenderHelper_h

#include "UKN/Platform.h"
#include "UKN/PreDeclare.h"
#include "UKN/Renderable.h"
#include "UKN/Color.h"

namespace ukn {
    
    class GraphicContext: public virtual IRenderable, public virtual Object {
    public:
        GraphicContext();
        virtual ~GraphicContext();
        
        virtual const String& getName() const;
        
        virtual Box getBound() const;
        virtual RenderBufferPtr getRenderBuffer() const;
        
        virtual void onRenderBegin();
        virtual void onRenderEnd();
        
        virtual void render();
        
    public:
        void setDrawColor(const ukn::Color& clr);
        void setFillColor(const ukn::Color& clr);
        const ukn::Color& getDrawColor() const;
        const ukn::Color& getFillColor() const;
        
        // state calls, begin must be called before changing the state
        // end must be called to acctually render current state
        void begin();
        void moveTo(float x, float y);
        void lineTo(float x, float y);
        void end();
        
        // immediate calls
        void drawCircle(float x, float y, float r);
        void fillCircle(float x, float y, float r);
        
        void drawRect(const ukn::Rectangle& rect);
        void fillRect(const ukn::Rectangle& rect);
        
        // simple drawTexture, actually draws in SpriteBatch::DefaultObject
        void drawTexture(const ukn::TexturePtr& texture, float x, float y, const ukn::Color& clr);
        
    private:
        GraphicBufferPtr mVertexBuffer;
        RenderBufferPtr mRenderBuffer;
        
        ukn::Color mDrawColor;
        ukn::Color mFillColor;
    };
    
} // namespace ukn

#endif
