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
    
    class GLWindow: public Window  {
    public:
        GLWindow(const ukn_string& name, const RenderSettings& settings);
        ~GLWindow();
        
        ukn_string description() const;
        
#ifdef UKN_OS_WINDOWS
		HWND getHWnd() const;
#endif
        
        bool pullEvents();
        
        int2  getMousePos();
        int32 getMouseWheel();
        
        bool isKeyDown(input::Key::KeyCode key);
        bool isMouseButtonDown(input::Mouse::MouseButton btn);
        
        void setMousePos(int32 x, int32 y);

        void onWindowResize(void* wnd, WindowResizeEventArgs&);
        void onWindowMove(Window& wnd, uint32 x, uint32 y);
        
        GLFrameBufferPtr getFrameBuffer() const;
        
    private:
        void updateWindowProperties(int32 x, int32 y, uint32 w, uint32 h);
        
        void* mGlfwWindow;
        
        GLFrameBufferPtr mFrameBuffer;
    };
    
} // namespace ukn

#endif
