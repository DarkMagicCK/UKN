//
//  GLWindow.cpp
//  Project Unknown
//
//  Created by Robert Bu on 11/24/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#include "GLWindow.h"
#include "GLRenderView.h"

#include "UKN/SysUtil.h"
#include "glfw/glfw3.h"

namespace ukn {
    
    static void ErrorFunc(int e, const char* w) {
	}
    
    static void WindowSizeFunc(GLFWwindow window, int w, int h) {
        GLWindow* glwnd = (GLWindow*)glfwGetWindowUserPointer(window);

        glwnd->onResize().getEvent()(*glwnd, w, h);
    }
    
    static int WindowCloseFunc(GLFWwindow window) {
        GLWindow* glwnd = (GLWindow*)glfwGetWindowUserPointer(window);

        glwnd->onClose().getEvent()(*glwnd);
        return 1;
    }
    
    static void WindowRefreshFunc(GLFWwindow window) {
        GLWindow* glwnd = (GLWindow*)glfwGetWindowUserPointer(window);

        glwnd->onRender().getEvent()(*glwnd);
    }
    
    static void WindowFocusFunc(GLFWwindow window, int f) {
        GLWindow* glwnd = (GLWindow*)glfwGetWindowUserPointer(window);

        glwnd->onActive().getEvent()(*glwnd, f > 0 ? true: false);
    }
    
    static void WindowIconifyFunc(GLFWwindow window, int f) {
        GLWindow* glwnd = (GLWindow*)glfwGetWindowUserPointer(window);

        glwnd->onIconify().getEvent()(*glwnd, f > 0 ? true : false);
    }
    
    static void MouseButtonFunc(GLFWwindow window, int a, int b) {
        GLWindow* glwnd = (GLWindow*)glfwGetWindowUserPointer(window);

    }
    
    static void MousePosFunc(GLFWwindow window, int x, int y) {
        GLWindow* glwnd = (GLWindow*)glfwGetWindowUserPointer(window);

    }
    
    static void ScrollFunc(GLFWwindow window, int a, int b) {
        GLWindow* glwnd = (GLWindow*)glfwGetWindowUserPointer(window);

        glwnd->onScroll().getEvent()(*glwnd, a, b);
    }
    
    static void KeyFunc(GLFWwindow window, int a, int b) {
        GLWindow* glwnd = (GLWindow*)glfwGetWindowUserPointer(window);

    }
    
    static void CharFunc(GLFWwindow window, int c) {
        GLWindow* glwnd = (GLWindow*)glfwGetWindowUserPointer(window);

        glwnd->onChar().getEvent()(*glwnd, c);
    }
    
    GLWindow::GLWindow(const ukn_string& name, const RenderSettings& settings):
    GLFrameBuffer(false),
    Window(name) {
        glfwInit();
       
        switch(settings.color_fmt) {
            case EF_ARGB8:
                glfwOpenWindowHint(GLFW_RED_BITS, 8);
                glfwOpenWindowHint(GLFW_BLUE_BITS, 8);
                glfwOpenWindowHint(GLFW_GREEN_BITS, 8);
                glfwOpenWindowHint(GLFW_ALPHA_BITS, 8);
                break;
                
            default:
                // error
                break;
        }
        
        switch(settings.depth_stencil_fmt) {
            case EF_D16:
                glfwOpenWindowHint(GLFW_DEPTH_BITS, 16);
                glfwOpenWindowHint(GLFW_STENCIL_BITS, 0);
                
                mIsDepthBuffered = true;
                mDepthBits = 16;
                mStencilBits = 0;
                break;
                
            case EF_D24S8:
                glfwOpenWindowHint(GLFW_DEPTH_BITS, 24);
                glfwOpenWindowHint(GLFW_STENCIL_BITS, 8);
                
                mIsDepthBuffered = true;
                mDepthBits = 24;
                mStencilBits = 8;
                break;
                
            default:
                glfwOpenWindowHint(GLFW_DEPTH_BITS, 0);
                glfwOpenWindowHint(GLFW_STENCIL_BITS, 0);
                
                mIsDepthBuffered = false;
                mDepthBits = 0;
                mStencilBits = 0;
                break;
        }

        if(settings.fsaa_samples > 0) {
            glfwOpenWindowHint(GLFW_FSAA_SAMPLES, settings.fsaa_samples);
        }
        
        glfwOpenWindowHint(GLFW_WINDOW_RESIZABLE, settings.resizable);
        
#if defined(UKN_OPENGL_3_2) && defined(UKN_REQUEST_OPENGL_3_2_PROFILE)
        
         glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
         glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 2);
        
#endif // UKN_HAS_OPENGL_3_2
        
        if((mGlfwWindow = glfwOpenWindow(settings.width,
                                         settings.height,
                                         settings.full_screen ? GLFW_FULLSCREEN : GLFW_WINDOWED, 
                                         name.c_str(), 
                                         0)) == 0) {
            // no window = app exit
            // so exception is acceptable here
            UKN_THROW_EXCEPTION("GLWindow::GLWindow: Error opening window");
        } 

#if defined(UKN_OS_WINDOWS)
		GLenum err = glewInit();
		if (GLEW_OK != err) {
			UKN_THROW_EXCEPTION(format_string("GLWindow::GLWindow: error initializing OpenGL profilem, error; %s", glewGetErrorString(err)));
		}
#endif
        
        
        // if wnd pos is (0, 0), then put it in the center of current screen
        int32 wndPosX = settings.left, wndPosY = settings.top;
        if(wndPosX == 0 && wndPosY == 0) {
            Array<DesktopMode> desktop_modes = enum_desktop_mode();
            
            wndPosX = (desktop_modes[0].width - settings.width) / 2;
            wndPosY = (desktop_modes[0].height - settings.height) / 2;
        }
        

		glfwSetWindowPos(mGlfwWindow, wndPosX, wndPosY);
        
        glfwSetWindowUserPointer(mGlfwWindow, this);
        
        glfwSetErrorCallback(ErrorFunc);
        glfwSetWindowSizeCallback(WindowSizeFunc);
        glfwSetWindowCloseCallback(WindowCloseFunc);
        glfwSetWindowRefreshCallback(WindowRefreshFunc);
        glfwSetWindowFocusCallback(WindowFocusFunc);
        glfwSetWindowIconifyCallback(WindowIconifyFunc);
        
        glfwSetKeyCallback(KeyFunc);
        glfwSetMousePosCallback(MousePosFunc);
        glfwSetMouseButtonCallback(MouseButtonFunc);
        glfwSetScrollCallback(ScrollFunc);
        glfwSetCharCallback(CharFunc);
        
        glfwSwapInterval(0);
        
        this->attach(ATT_Color0, new GLScreenColorRenderView(settings.width, 
                                                             settings.height,
                                                             settings.color_fmt));
        this->attach(ATT_DepthStencil, new GLScreenDepthStencilRenderView(settings.width, 
                                                                          settings.height,
                                                                          settings.depth_stencil_fmt));
        
        FrameBuffer::mLeft = 0;
        FrameBuffer::mTop = 0;
        FrameBuffer::mWidth = settings.width;
        FrameBuffer::mHeight = settings.height;
        
        Window::mLeft = wndPosX;
        Window::mTop = wndPosY;
        Window::mWidth = settings.width;
        Window::mHeight = settings.height;
        
        mViewPort.left = 0;
        mViewPort.top = 0;
        mViewPort.width = settings.width;
        mViewPort.height = settings.height;
    }
    
    bool GLWindow::pullEvents() { 
#ifdef UKN_OS_WINDOWS
		bool gotMsg;
		MSG  msg;
        
		while(::PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) {
        
			if (WM_QUIT != msg.message && WM_CLOSE != msg.message) {
				gotMsg = ::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) ? true : false;
				
				if (gotMsg)
				{
					::TranslateMessage(&msg);
					::DispatchMessage(&msg);
				}
			} else 
				return true;
		}
#endif
        
        glfwPollEvents();
        return false;
    }

    void GLWindow::swapBuffers() {
        glfwSwapBuffers();
    }

#ifdef UKN_OS_WINDOWS
	HWND GLWindow::getHWnd() const {
		return 0;
	}
#endif
    
    GLWindow::~GLWindow() {
        glfwCloseWindow(mGlfwWindow);
        glfwTerminate();
    }
    
    ukn_string GLWindow::description() const {
        static ukn_string des("OpenGL Window based on GLFW");
        return des;
    }
     
} // namespce ukn
