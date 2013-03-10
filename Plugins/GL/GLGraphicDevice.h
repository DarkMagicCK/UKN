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
#include "UKN/Vertex.h"
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
        
        void renderBuffer(const EffectTechniquePtr& technique, const RenderBufferPtr& buffer);
        void onBindFrameBuffer(const FrameBufferPtr& frameBuffer);
        
        void setViewMatrix(const Matrix4& mat);
        void setProjectionMatrix(const Matrix4& mat);
        
        void getViewMatrix(Matrix4& mat) const;
        void getProjectionMatrix(Matrix4& mat) const;
        
        void bindTexture(const TexturePtr& texture);
       
        void fillGraphicCaps(GraphicDeviceCaps& caps);
        
        void setRenderState(RenderStateType type, uint32 func);
        
        void bindGLFrameBuffer(GLuint fbo);
        GLuint getBindedGLFrameBuffer() const;

        void adjustPerspectiveMat(Matrix4& mat) override;
        void adjustOrthoMat(Matrix4& mat) override;

        void onSetBlendState(const BlendStatePtr& blendState) override;
        void onSetSamplerState(const SamplerStatePtr& samplerState, uint32 index) override;
        void onSetRasterizerState(const RasterizerStatePtr& rasterizerState) override;
        void onSetDepthStencilState(const DepthStencilStatePtr& depthStencilState) override;

    private:
        std::vector<int> applyBuffer(const vertex_elements_type& format, uint32 size, uint32 instance_freq);

        WindowPtr mWindow;
        
        Matrix4 mViewMat;
        Matrix4 mProjectionMat;

        GLuint mCurrGLFrameBuffer;
    };
    
} // namespace ukn



#endif
