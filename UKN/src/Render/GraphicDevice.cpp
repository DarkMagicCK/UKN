//
//  GraphicDevice.cpp
//  Project Unknown
//
//  Created by Robert Bu on 11/24/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#include "UKN/GraphicDevice.h"

#include "UKN/Window.h"

namespace ukn {
    
    class NullWindow: public Window {
    public:
        NullWindow():
        Window("Null") {
            
        }
        
        ukn_string description() const {
            return ukn_string("Null Window");
        }
        
#if defined(UKN_OS_WINDOWS)
        HWND getHWnd() const {
            return 0;
        }
#endif
        
        void swapBuffers() {
            
        }
        
        void pullEvents() {
            
        }
        
    };
    
    class NullGraphicDevice: public GraphicDevice {
    public:
        WindowPtr createRenderWindow(const ukn_string& name, const RenderSettings& settings) {
            static WindowPtr static_ptr = MakeSharedPtr<NullWindow>();
            return static_ptr;
        }
        
        void onRenderBuffer(const RenderBufferPtr& buffer) {
            
        }
        
        ukn_string description() const {
            return ukn_string("Null GraphicDevice");
        }
    };
    
    GraphicDevicePtr GraphicDevice::NullObject() {
        static GraphicDevicePtr static_ptr = MakeSharedPtr<NullGraphicDevice>();
        return static_ptr;
    }
    
    GraphicDevice::GraphicDevice() {
        
    }
    
    GraphicDevice::~GraphicDevice() {
        
    }
    
    
} // namespace ukn
