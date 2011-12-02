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

#ifdef UKN_OS_WINDOWS
#pragma comment(lib, "OpenGL32.lib")
#pragma comment(lib, "glu32.lib")
#endif

namespace ukn {
    
    class GLGraphicDevice: public GraphicDevice {
    public:
        GLGraphicDevice();
        virtual ~GLGraphicDevice();
        
        ukn_string description() const;
        
        WindowPtr doCreateRenderWindow(const ukn_string& name, const RenderSettings& settings);
        
        void beginFrame();
        void endFrame();
        
        void onRenderBuffer(const RenderBufferPtr& buffer);
        void onBindFrameBuffer(const FrameBufferPtr& frameBuffer);
        
        void setViewMatrix(const Matrix4& mat);
        void setProjectionMatrix(const Matrix4& mat);
        
        void getViewMatrix(Matrix4& mat);
        void getProjectionMatrix(Matrix4& mat);
        
        void bindTexture(TexturePtr texture);
        
        void fillGraphicCaps(GraphicDeviceCaps& caps);
        
    private:
        TexturePtr mCurrTexture;
    };
    
} // namespace ukn



#endif
