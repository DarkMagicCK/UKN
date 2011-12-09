//
//  SpriteBatch.h
//  Project Unknown
//
//  Created by Ruiwei Bu on 12/6/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_SpriteBatch_h
#define Project_Unknown_SpriteBatch_h

#include "UKN/Platform.h"
#include "UKN/Uncopyable.h"
#include "UKN/PreDeclare.h"
#include "UKN/Color.h"
#include "UKN/Vertex.h"
#include "UKN/Renderable.h"

namespace ukn {
        
    class UKN_API SpriteBatch: public Renderable {
    protected:
        SpriteBatch();
        
        friend class GraphicFactory;
    
    public:
        virtual ~SpriteBatch();

        const ukn_string& getName() const;
        
        Rectangle getBound() const;
        RenderBufferPtr getRenderBuffer() const;
        
        virtual void onRenderBegin();
        virtual void onRenderEnd();
        
        virtual void render();
        
        void draw(const TexturePtr& texture, float x, float y, const Color& color=ColorWhite);
        void draw(const TexturePtr& texture, Rectangle dstRect, const Color& color=ColorWhite);
        void draw(const TexturePtr& texture, Rectangle srcRect, Rectangle dstRect, const Color& color=ColorWhite);
        void draw(const TexturePtr& texture, Rectangle srcRect, Rectangle dstRect, float rot, const Color& color=ColorWhite);
        void draw(const TexturePtr& texture, float x, float y, float cx, float cy, float rot, float scalex, float scaley, const Color& color=ColorWhite);
        
        void draw(const TexturePtr& texture, float x, float y, float layerDepth, const Color& color=ColorWhite);
        void draw(const TexturePtr& texture, Rectangle dstRect, float layerDepth, const Color& color=ColorWhite);
        void draw(const TexturePtr& texture, Rectangle srcRect, Rectangle dstRect, float rot, float layerDepth, const Color& color=ColorWhite);
        void draw(const TexturePtr& texture, float x, float y, float cx, float cy, float rot, float scalex, float scaley, float layerDepth, const Color& color=ColorWhite);
        
        Matrix4& getTransformMatrix();
        const Matrix4& getTransformMatrix() const;
                
    protected:
        void updateBoundingBox(float x, float y, float x2, float y2);
        
        struct TextureObject {
            TextureObject();
            TextureObject(const TexturePtr& tex);
            TextureObject(const TexturePtr& tex, float depth);
            
            void buildVertices(float x, float y, float cx, float cy, float rot, float scalex, float scaley, const Color& color);
            void buildVertices(const Rectangle& dstRect, float rot, const Color& color);
            bool operator<(const SpriteBatch::TextureObject& rhs) const;
            
            TexturePtr  texture;
            float       layerDepth;
            
            Vertex2D    vertices[6];
            Rectangle   srcRect;
        };
        typedef Array<TextureObject> RenderQueue;
        RenderQueue mRenderQueue;
        
        RenderBufferPtr mRenderBuffer;
        GraphicBufferPtr mVertexBuffer;
        
        Rectangle mBoundingBox;
        Matrix4 mTransformMatrix;
    };
    
} // namespace ukn

#endif
