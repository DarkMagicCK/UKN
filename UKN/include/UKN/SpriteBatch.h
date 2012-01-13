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
    
    enum SpriteBatchSortMode {
        SBS_None,
        SBS_Deffered,
        SBS_BackToFront,
        SBS_FrontToBack
    };
    
    struct SpriteDescriptor {
        Vector2     position;
        Vector2     scale;
        Vector2     center;
        float       rotation;
        float       layer_depth;
        Color       color;
        Rectangle   source_rect;
        Rectangle   dest_rect;
        
        bool    vflip;
        bool    hflip;
        
        SpriteDescriptor();
        SpriteDescriptor(const Vector2& pos,
                         const Vector2& center,
                         float rot,
                         const Vector2& scale, 
                         Color clr,
                         float depth,
                         bool vflip=false,
                         bool hflip=false);
    };
        
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

        // must be called before any draw call
        void begin(SpriteBatchSortMode mode = SBS_Deffered);
        void begin(SpriteBatchSortMode mode, const Matrix4& transformMat);
        
        // must be called after draw to render the SpriteBatch
        void end();
            
        // draw with position
        void draw(const TexturePtr& texture, const Vector2& pos, const Color& color=color::White);
        void draw(const TexturePtr& texture, const Vector2& pos, float layerDepth, const Color& color=color::White);

        // draw with a custom destination rect
        void draw(const TexturePtr& texture, const Rectangle& dstRect, const Color& color=color::White);
        void draw(const TexturePtr& texture, const Rectangle& dstRect, float layerDepth, const Color& color=color::White);

        // draw with custom source rect and position
        void draw(const TexturePtr& texture, const Vector2& pos, const Rectangle& src, const Color& color=color::White);        
        void draw(const TexturePtr& texture, const Vector2& pos, const Rectangle& src, float layerDetph, const Color& color=color::White);

        // draw with custom source and destination rect
        void draw(const TexturePtr& texture, const Rectangle& srcRect, const Rectangle& dstRect, const Color& color=color::White);
        
        // draw with position, center, rotation and scale
        void draw(const TexturePtr& texture, const Vector2& pos, const Vector2& center, float rot, const Vector2& scale, const Color& color=color::White);
        void draw(const TexturePtr& texture, const Vector2& pos, const Vector2& center, float rot, const Vector2& scale, float layerDepth, const Color& color=color::White);
        
        // draw with custom descriptor
        void draw(const TexturePtr& texture, const SpriteDescriptor& descriptor);
        
        Matrix4& getTransformMatrix();
        const Matrix4& getTransformMatrix() const;
                
    protected:
        void updateBoundingBox(float x, float y, float z);
        
        struct TextureObject {
            TextureObject();
            TextureObject(const TexturePtr& tex);
            
            void buildVertices(const SpriteDescriptor&);
            
            bool operator<(const SpriteBatch::TextureObject& rhs) const;
            bool operator>(const SpriteBatch::TextureObject& rhs) const;
            bool operator!=(const SpriteBatch::TextureObject& rhs) const;
            
            TexturePtr  texture;
            float       layer_depth;
            Vertex2D    vertices[6];
        };
        typedef Array<TextureObject> RenderQueue;
        RenderQueue mRenderQueue;
        
        RenderBufferPtr mRenderBuffer;
        GraphicBufferPtr mVertexBuffer;
        
        Box mBoundingBox;
        Matrix4 mTransformMatrix;
        
        bool mBegan;
        SpriteBatchSortMode mCurrMode;
    };
    
} // namespace ukn

#endif
