//
//  GLRenderDevice.h
//  Project Unknown
//
//  Created by Ruiwei Bu on 11/24/11.
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
        
        const ukn_wstring& description() const;
        
        WindowPtr createRenderWindow(const ukn_string& name, const RenderSettings& settings);
        void onRenderBuffer(const RenderBufferPtr& buffer);
    };
    
} // namespace ukn



#endif
