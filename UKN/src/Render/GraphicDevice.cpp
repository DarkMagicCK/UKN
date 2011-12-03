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

namespace ukn {
    
    class NullWindow: public Window {
    public:
        NullWindow():
        Window("Null") { }
        
        ukn_string description() const { return ukn_string("Null Window"); }
        
#if defined(UKN_OS_WINDOWS)
        HWND getHWnd() const { return 0; }
#endif
        bool pullEvents() { return true; }
    };
    
    class NullGraphicDevice: public GraphicDevice {
    public:
        WindowPtr doCreateRenderWindow(const ukn_string& name, const RenderSettings& settings) {
            static WindowPtr static_ptr = MakeSharedPtr<NullWindow>();
            return static_ptr;
        }
        
        void onRenderBuffer(const RenderBufferPtr& buffer) { }
        
        ukn_string description() const {
            return ukn_string("Null GraphicDevice");
        }
        
        void beginRendering() { }
        
        void setViewMatrix(const Matrix4& mat) { }
        void setProjectionMatrix(const Matrix4& mat) { }
        
        void getViewMatrix(Matrix4& mat) { }
        void getProjectionMatrix(Matrix4& mat) { }
        
        void fillGraphicCaps(GraphicDeviceCaps& caps) {
            caps.max_indices = 0;
            caps.max_vertices = 0;
            caps.max_texture_width = 0;
            caps.max_texture_cube_map_size = 0;
            caps.max_texture_height = 0;
            caps.max_pixel_texture_units = 0;
            caps.max_vertex_texture_units = 0;
        }
        
        void bindTexture(TexturePtr tex) { }
        
        void beginFrame() { }
        void endFrame() { }
        void onBindFrameBuffer(const FrameBufferPtr& frameBuffer) {  }
    };
    
    GraphicDevicePtr GraphicDevice::NullObject() {
        static GraphicDevicePtr static_ptr = MakeSharedPtr<NullGraphicDevice>();
        return static_ptr;
    }
    
    GraphicDevice::GraphicDevice() {
        
    }
    
    GraphicDevice::~GraphicDevice() {
        
    }
    
    void GraphicDevice::pushViewMatrix() {
        Matrix4 mat;
        getViewMatrix(mat);
        mViewMatrixStack.push(mat);
    }
    
    void GraphicDevice::popViewMatrix() {
        if(!mViewMatrixStack.empty()) {
            setViewMatrix(mViewMatrixStack.top());
            mViewMatrixStack.pop();
        }
    }
    
    void GraphicDevice::pushProjectionMatrix() {
        Matrix4 mat;
        getProjectionMatrix(mat);
        mProjMatrixStack.push(mat);
    }
   
    void GraphicDevice::popProjectionMatrix() {
        if(!mProjMatrixStack.empty()) {
            setProjectionMatrix(mProjMatrixStack.top());
            mProjMatrixStack.pop();
        }
    }
    
    void GraphicDevice::bindFrameBuffer(const FrameBufferPtr& ptr) {
        if(mCurrFrameBuffer) {
            mCurrFrameBuffer->onUnbind();
        }
        
        mCurrFrameBuffer = ptr;
        mCurrFrameBuffer->onBind();
        
        onBindFrameBuffer(ptr);
    }
    
    const FrameBufferPtr& GraphicDevice::getCurrFrameBuffer() const {
        return mCurrFrameBuffer;
    }
    
    const FrameBufferPtr& GraphicDevice::getScreenFrameBuffer() const {
        return mScreenFrameBuffer;
    }
    
    WindowPtr GraphicDevice::createRenderWindow(const ukn_string& name, const RenderSettings& settings) {
        return doCreateRenderWindow(name, settings);
    }
    
    void GraphicDevice::clearColor(const Color& clr) {
        
    }
    
    void GraphicDevice::clearDepth(float depth) {
        
    }
    
    void GraphicDevice::clearStencil(int32 stencil) {
        
    }
    
} // namespace ukn
