//
//  GraphicDevice.cpp
//  Project Unknown
//
//  Created by Robert Bu on 11/24/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#include "UKN/GraphicDevice.h"
#include "UKN/FrameBuffer.h"
#include "UKN/Window.h"
#include "UKN/Texture.h"
#include "UKN/RenderTarget.h"
#include "UKN/RenderView.h"

namespace ukn {
    
    class NullWindow: public Window {
    public:
        NullWindow():
        Window(L"Null") { }
        
        UknString description() const { return UknString(L"Null Window"); }
        
#if defined(MIST_OS_WINDOWS)
        HWND getHWnd() const { return 0; }
#endif
        bool pullEvents() { return true; }
        
        int2  getMousePos() { return int2(0, 0); }
        int32 getMouseWheel() { return 0; }
        
        void setName(const UknString& name) { }
        
        bool isKeyDown(input::KeyCode key) { return false; }
        bool isMouseButtonDown(input::MouseButton btn) { return false; }
        
        void setMousePos(int32 x, int32 y) { }
    };
    
    class NullGraphicDevice: public GraphicDevice {
    public:
        WindowPtr doCreateRenderWindow(const UknString& name, const RenderSettings& settings) {
            static WindowPtr static_ptr = MakeSharedPtr<NullWindow>();
            return static_ptr;
        }
        
        void renderBuffer(const RenderBufferPtr& buffer) { }
        
        UknString description() const {
            return UknString(L"Null GraphicDevice");
        }
        
        void beginRendering() { }
        
        void setViewMatrix(const Matrix4& mat) { }
        void setProjectionMatrix(const Matrix4& mat) { }
        void setWorldMatrix(const Matrix4& mat) { }
        
        void getViewMatrix(Matrix4& mat) const { }
        void getProjectionMatrix(Matrix4& mat) const { }
        void getWorldMatrix(Matrix4& mat) const { }
        
        void setRenderState(RenderStateType, uint32) { }
        
        void fillGraphicCaps(GraphicDeviceCaps& caps) {
            caps.max_indices = 0;
            caps.max_vertices = 0;
            caps.max_texture_width = 0;
            caps.max_texture_cube_map_size = 0;
            caps.max_texture_height = 0;
            caps.max_pixel_texture_units = 0;
            caps.max_vertex_texture_units = 0;
        }
        
        void bindTexture(const TexturePtr& tex) { }
        void bindEffect(const EffectPtr& effect) { }
        
        void beginFrame() { }
        void endFrame() { }
        void onBindFrameBuffer(const FrameBufferPtr& frameBuffer) { }
        
        void begin2DRendering(const OrthogonalParams& params) { }
        void end2DRendering() { }
        
        void setBlendState(const BlendStatePtr& blendState) { }
        void setSamplerState(const SamplerStatePtr& samplerState) { }
    };
    
    GraphicDevicePtr GraphicDevice::NullObject() {
        static GraphicDevicePtr static_ptr = MakeSharedPtr<NullGraphicDevice>();
        return static_ptr;
    }
    
    GraphicDevice::GraphicDevice() {
        
    }
    
    GraphicDevice::~GraphicDevice() {
        
    }
    
    Matrix4 GraphicDevice::pushViewMatrix() {
        Matrix4 mat;
        getViewMatrix(mat);
        mViewMatrixStack.push(mat);
        
        return mat;
    }
    
    Matrix4 GraphicDevice::popViewMatrix() {
        if(!mViewMatrixStack.empty()) {
            Matrix4 mat = mViewMatrixStack.top();
            setViewMatrix(mat);
            mViewMatrixStack.pop();
            
            return mat;
        }
        return Matrix4();
    }
    
    Matrix4 GraphicDevice::pushProjectionMatrix() {
        Matrix4 mat;
        getProjectionMatrix(mat);
        mProjMatrixStack.push(mat);
        
        return mat;
    }
   
    Matrix4 GraphicDevice::popProjectionMatrix() {
        if(!mProjMatrixStack.empty()) {
            Matrix4 mat = mProjMatrixStack.top();
            setProjectionMatrix(mat);
            mProjMatrixStack.pop();
            
            return mat;
        }
        return Matrix4();
    }
    
    void GraphicDevice::bindFrameBuffer(const FrameBufferPtr& frameBuffer) {
        if(mCurrFrameBuffer) {
            mCurrFrameBuffer->onUnbind();
        }
        
        if(frameBuffer.isValid()) {
            mCurrFrameBuffer = frameBuffer;
            mCurrFrameBuffer->onBind();
        
            onBindFrameBuffer(frameBuffer);
        } else
            bindFrameBuffer(mScreenFrameBuffer);
    }
    
    const FrameBufferPtr& GraphicDevice::getCurrFrameBuffer() const {
        return mCurrFrameBuffer;
    }
    
    const FrameBufferPtr& GraphicDevice::getScreenFrameBuffer() const {
        return mScreenFrameBuffer;
    }
    
    WindowPtr GraphicDevice::createRenderWindow(const UknString& name, const RenderSettings& settings) {
        return doCreateRenderWindow(name, settings);
    }
    
    void GraphicDevice::clearColor(const Color& clr) {
        if(mCurrFrameBuffer) {
            mCurrFrameBuffer->clear(CM_Color, clr, 0, 0);
        }
    }
    
    void GraphicDevice::clearDepth(float depth) {
        if(mCurrFrameBuffer) {
            mCurrFrameBuffer->clear(CM_Depth, Color(), depth, 0);
        }
    }
    
    void GraphicDevice::clearStencil(int32 stencil) {
        if(mCurrFrameBuffer) {
            mCurrFrameBuffer->clear(CM_Stencil, Color(), 0, stencil);
        }
    }
    
    void GraphicDevice::setRenderTarget(const RenderTargetPtr& target) {
        if(target) {
            if(mCurrFrameBuffer) {
                mCurrFrameBuffer->attach(ATT_Color0, target->getTargetView());
                if(target->getDepthStencilView()) {
                    mCurrFrameBuffer->attach(ATT_DepthStencil, target->getDepthStencilView());
                }
                
                mCurrTarget = target;
            }
        } else {
            if(mCurrTarget) {
                mCurrFrameBuffer->detach(ATT_Color0);
                if(mCurrTarget->getDepthStencilView())
                    mCurrFrameBuffer->detach(ATT_DepthStencil);
            }
            mCurrTarget = RenderTargetPtr();
        }
    }
    
    void GraphicDevice::clear(uint32 flags, const class Color& clr, float depth, int32 stencil) {
        if(mCurrFrameBuffer) {
            mCurrFrameBuffer->clear(flags, clr, depth, stencil);
        }
    }
    
} // namespace ukn
