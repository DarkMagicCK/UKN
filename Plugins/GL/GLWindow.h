//
//  GLWindow.h
//  Project Unknown
//
//  Created by Robert Bu on 11/24/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_GLWindow_h
#define Project_Unknown_GLWindow_h

#include "UKN/Platform.h"
#include "UKN/Window.h"
#include "UKN/Signal.h"
#include "UKN/FrameBuffer.h"

#include "GLFrameBuffer.h"

namespace ukn {
    
    class GLWindow: public Window, public GLFrameBuffer {
    public:
        GLWindow(const ukn_string& name, const RenderSettings& settings);
        ~GLWindow();
        
        ukn_string description() const;
        
#ifdef UKN_OS_WINDOWS
		HWND getHWnd() const;
#endif
        
        bool pullEvents();
        void swapBuffers();
        
    private:
        void* mGlfwWindow;
    };
    
} // namespace ukn

#endif
