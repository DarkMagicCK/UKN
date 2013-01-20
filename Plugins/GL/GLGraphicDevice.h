//
//  GLRenderDevice.h
//  Project Unknown
//
//  Created by Robert Bu on 11/24/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_GLRenderDevice_h
#define Project_Unknown_GLRenderDevice_h

#include "UKN/GraphicDevice.h"
#include "GLPreq.h"

namespace ukn {
    
    class GLGraphicDevice: public GraphicDevice {
    public:
        GLGraphicDevice();
        virtual ~GLGraphicDevice();
        
        UknString description() const;
        
        WindowPtr doCreateRenderWindow(const UknString& name, const RenderSettings& settings);
        
        void beginFrame();
        void endFrame();
        void beginRendering();
        
        void renderBuffer(const RenderBufferPtr& buffer);
        void onBindFrameBuffer(const FrameBufferPtr& frameBuffer);
        
        void setViewMatrix(const Matrix4& mat);
        void setProjectionMatrix(const Matrix4& mat);
        void setWorldMatrix(const Matrix4& mat);
        
        void getViewMatrix(Matrix4& mat) const;
        void getProjectionMatrix(Matrix4& mat) const;
        void getWorldMatrix(Matrix4& mat) const;
        
        void bindTexture(const TexturePtr& texture);
        void bindEffect(const EffectPtr& effect);
        
        SharedPtr<uint8> readFrameBufferData(const FrameBufferPtr& buffer, int32 x, int32 y, uint32 width, uint32 height, ElementFormat format);
        SharedPtr<uint8> readTextureData(const TexturePtr& texture, uint8 level);
        void  updateTextureData(const TexturePtr& texture, void* data, int32 x, int32 y, uint32 width, uint32 height, uint8 level);
        
        void fillGraphicCaps(GraphicDeviceCaps& caps);
        
        void setRenderState(RenderStateType type, uint32 func);
        
        void bindGLFrameBuffer(GLuint fbo);
        GLuint getBindedGLFrameBuffer() const;

        void adjustPerspectiveMat(Matrix4& mat) override;
        
    private:
        WindowPtr mWindow;
        TexturePtr mCurrTexture;
        EffectPtr mEffect;

        Matrix4 mWorldMat;
        Matrix4 mViewMat;
        Matrix4 mProjectionMat;
        
        GLuint mCurrGLFrameBuffer;
    };
    
} // namespace ukn



#endif
