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
        
        void fillGraphicCaps(GraphicDeviceCaps& caps);
        
        void setRenderState(RenderStateType type, uint32 func);
        
        void bindGLFrameBuffer(GLuint fbo);
        GLuint getBindedGLFrameBuffer() const;

        void adjustPerspectiveMat(Matrix4& mat) override;

        void begin2DRendering(const OrthogonalParams& params) override;
        void end2DRendering() override;
        
        void setBlendState(const BlendStatePtr& blendState) override;
        void setSamplerState(const SamplerStatePtr& samplerState) override;
        
    private:
        WindowPtr mWindow;
        TexturePtr mCurrTexture;
        EffectPtr mEffect;

        Matrix4 mWorldMat;
        Matrix4 mViewMat;
        Matrix4 mProjectionMat;

        bool mIs2D;
        EffectPtr m2DEffect;
        
        GLuint mCurrGLFrameBuffer;
    };
    
} // namespace ukn



#endif
