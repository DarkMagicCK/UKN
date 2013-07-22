//
//  GLWindow.h
//  Project Unknown
//
//  Created by Robert Bu on 11/24/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_GLWindow_h
#define Project_Unknown_GLWindow_h

#include "mist/Platform.h"
#include "mist/SSignal.h"

#include "UKN/Window.h"
#include "UKN/FrameBuffer.h"

#include "GLFrameBuffer.h"

struct GLFWwindow;

namespace ukn {
    
    class GLWindow: public Window  {
    public:
        GLWindow(const UknString& name, const RenderSettings& settings);
        ~GLWindow();
        
        UknString description() const;
        
#ifdef MIST_OS_WINDOWS
		HWND getHWnd() const;
#endif
        
        bool pullEvents();
        
        int2  getMousePos();
        int32 getMouseWheel();
        
        bool isKeyDown(input::KeyCode key);
        bool isMouseButtonDown(input::MouseButton btn);
        
        void setMousePos(int32 x, int32 y);

        void onWindowResize(void* wnd, WindowResizeEventArgs&);
        void onWindowMove(Window& wnd, uint32 x, uint32 y);
        
        void setName(const UknString& name);
        
        GLFrameBufferPtr getFrameBuffer() const;
        GLFWwindow* getGLFWWindow() const;
        
        void swapBuffers();
        
        void onKeyDown(int key, int state);
        void onMouseDown(int btn, int state);
        
    private:
        void updateWindowProperties(int32 x, int32 y, uint32 w, uint32 h);
        
        GLFWwindow* mGlfwWindow;
        GLFrameBufferPtr mFrameBuffer;
        
        FixedArray<std::pair<int, bool>, 256> mKeyStatus;;
    };
    
} // namespace ukn

#endif
