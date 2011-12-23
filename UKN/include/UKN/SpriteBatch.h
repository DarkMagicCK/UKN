//
//  SpriteBatch.h
//  Project Unknown
//
//  Created by Robert Bu on 12/6/11.
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
#include "UKN/StringUtil.h"

namespace ukn {
        
    class UKN_API SpriteBatch: public Renderable {
    protected:
        SpriteBatch();
        
        friend class GraphicFactory;
    
    public:
        virtual ~SpriteBatch();
        
        static SpriteBatch& DefaultObject();

        const String& getName() const;
        
        Box getBound() const;
        RenderBufferPtr getRenderBuffer() const;
        
        virtual void onRenderBegin();
        virtual void onRenderEnd();
        virtual void onRender();
        
        void render();
        
        void draw(const TexturePtr& texture, float x, float y, const Color& color=ColorWhite);
        void draw(const TexturePtr& texture, const Rectangle& dstRect, const Color& color=ColorWhite);
        void draw(const TexturePtr& texture, float x, float y, const Rectangle& src, const Color& color=ColorWhite);
        
        void draw(const TexturePtr& texture, const Rectangle& srcRect, const Rectangle& dstRect, const Color& color=ColorWhite);
        void draw(const TexturePtr& texture, const Rectangle& srcRect, const Rectangle& dstRect, float rot, const Color& color=ColorWhite);
        void draw(const TexturePtr& texture, float x, float y, float cx, float cy, float rot, float scalex, float scaley, const Color& color=ColorWhite);
        
        void draw(const TexturePtr& texture, float x, float y, float layerDepth, const Color& color=ColorWhite);
        void draw(const TexturePtr& texture, float x, float y, const Rectangle& src, float layerDetph, const Color& color=ColorWhite);

        void draw(const TexturePtr& texture, const Rectangle& dstRect, float layerDepth, const Color& color=ColorWhite);
        void draw(const TexturePtr& texture, const Rectangle& srcRect, const Rectangle& dstRect, float rot, float layerDepth, const Color& color=ColorWhite);
        void draw(const TexturePtr& texture, float x, float y, float cx, float cy, float rot, float scalex, float scaley, float layerDepth, const Color& color=ColorWhite);
        
        Matrix4& getTransformMatrix();
        const Matrix4& getTransformMatrix() const;
                
    protected:
        void updateBoundingBox(float x, float y, float z);
        
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
        
        Box mBoundingBox;
        Matrix4 mTransformMatrix;
    };
    
} // namespace ukn

#endif
