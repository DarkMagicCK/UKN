//
//  GLWindow.cpp
//  Project Unknown
//
//  Created by Robert Bu on 11/24/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#include "GLWindow.h"
#include "GLRenderView.h"

#include "mist/SysUtil.h"

#include "UKN/Context.h"
#include "UKN/GraphicFactory.h"
#include "UKN/GraphicDevice.h"
#include "UKN/Input.h"

#include "GLPreq.h"
#include "GLConvert.h"

#include "GLFWInput.h"

namespace ukn {
    
    static void ErrorFunc(int e, const char* w) {
	}
    
    static int g_key_flag;
    
    static void update_key_flag(GLFWwindow window) {
        g_key_flag = 0;
        if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT))
            g_key_flag |= input::Key::FlagShift;
        if(glfwGetKey(window, GLFW_KEY_LEFT_ALT) || glfwGetKey(window, GLFW_KEY_RIGHT_ALT))
            g_key_flag |= input::Key::FlagAlt;
        if(glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) || glfwGetKey(window, GLFW_KEY_RIGHT_ALT))
            g_key_flag |= input::Key::FlagCtrl;
        if(glfwGetKey(window, GLFW_KEY_CAPS_LOCK))
            g_key_flag |= input::Key::FlagCapslock;
        if(glfwGetKey(window, GLFW_KEY_SCROLL_LOCK))
            g_key_flag |= input::Key::FlagScrolllock;
        if(glfwGetKey(window, GLFW_KEY_NUM_LOCK))
            g_key_flag |= input::Key::FlagNumlock;
        if(glfwGetKey(window, GLFW_KEY_REPEAT))
            g_key_flag |= input::Key::FlagRepeat;
    }
    
    static void WindowSizeFunc(GLFWwindow window, int w, int h) {
        GLWindow* glwnd = (GLWindow*)glfwGetWindowUserPointer(window);

        WindowResizeEventArgs args(w, h);
        glwnd->onResize().raise(glwnd, args);
    }
    
    static int WindowCloseFunc(GLFWwindow window) {
        GLWindow* glwnd = (GLWindow*)glfwGetWindowUserPointer(window);

        glwnd->onClose().raise(glwnd, _NullEventArgs);
        return 1;
    }
    
    static void WindowRefreshFunc(GLFWwindow window) {
    //    GLWindow* glwnd = (GLWindow*)glfwGetWindowUserPointer(window);

    }
    
    static void WindowFocusFunc(GLFWwindow window, int f) {
        GLWindow* glwnd = (GLWindow*)glfwGetWindowUserPointer(window);

        WindowBoolEventArgs args(f > 0 ? true: false);
        glwnd->onActive().raise(glwnd, args);
    }
    
    static void WindowIconifyFunc(GLFWwindow window, int f) {
        GLWindow* glwnd = (GLWindow*)glfwGetWindowUserPointer(window);

        WindowBoolEventArgs args(f > 0 ? true: false);
        glwnd->onIconify().raise(glwnd, args);
    }
    
    static void MouseButtonFunc(GLFWwindow window, int btn, int state) {
        GLWindow* glwnd = (GLWindow*)glfwGetWindowUserPointer(window);

        input::MouseEventArgs args;
        args.button = glfw_to_ukn_mouse(btn);
        args.state  = glfw_to_ukn_mouse_state(state);
        args.flag   = g_key_flag;
        
        // retrieve mouse pos
        glfwGetMousePos(window, &args.x, &args.y);
        
        glwnd->onMouseEvent().raise(glwnd, args);
    }
    
    static void MousePosFunc(GLFWwindow window, int x, int y) {
        GLWindow* glwnd = (GLWindow*)glfwGetWindowUserPointer(window);
                
        input::MouseEventArgs args;
        args.button = input::Mouse::Null;
        args.state  = input::Mouse::Move;
        args.flag   = g_key_flag;
        args.x      = x;
        args.y      = y;
        
        glwnd->onMouseEvent().raise(glwnd, args);
    }
    
    static void ScrollFunc(GLFWwindow window, int a, int b) {
    //    GLWindow* glwnd = (GLWindow*)glfwGetWindowUserPointer(window);

  //      glwnd->onScroll().getSignal()(*glwnd, a, b);
    }
    
    static void KeyFunc(GLFWwindow window, int key, int state) {
        GLWindow* glwnd = (GLWindow*)glfwGetWindowUserPointer(window);

        input::KeyEventArgs args;
        args.key    = glfw_to_ukn_key(key);
        args.state  = glfw_to_ukn_key_state(state);
        args.flag   = g_key_flag;
        
        glwnd->onKeyEvent().raise(glwnd, args);
    }
    
    static void CharFunc(GLFWwindow window, int c) {
   //     GLWindow* glwnd = (GLWindow*)glfwGetWindowUserPointer(window);

    }
    
    GLWindow::GLWindow(const UknString& name, const RenderSettings& settings):
    mFrameBuffer(new GLFrameBuffer(false)),
    Window(name) {
        glfwInit();
       
        switch(settings.color_fmt) {
            case EF_RGBA8:
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
                
                mFrameBuffer->mIsDepthBuffered = true;
                mFrameBuffer->mDepthBits = 16;
                mFrameBuffer->mStencilBits = 0;
                break;
                
            case EF_D24S8:
                glfwOpenWindowHint(GLFW_DEPTH_BITS, 24);
                glfwOpenWindowHint(GLFW_STENCIL_BITS, 8);
                
                mFrameBuffer->mIsDepthBuffered = true;
                mFrameBuffer->mDepthBits = 24;
                mFrameBuffer->mStencilBits = 8;
                break;
                
            default:
                glfwOpenWindowHint(GLFW_DEPTH_BITS, 0);
                glfwOpenWindowHint(GLFW_STENCIL_BITS, 0);
                
                mFrameBuffer->mIsDepthBuffered = false;
                mFrameBuffer->mDepthBits = 0;
                mFrameBuffer->mStencilBits = 0;
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
                                         String::WStringToString(name).c_str(),
                                         0)) == 0) {
            // no window = app exit
            // so exception is acceptable here
            MIST_THROW_EXCEPTION("GLWindow::GLWindow: Error opening window");
        } 

#if defined(MIST_OS_WINDOWS)
		GLenum err = glewInit();
		if (GLEW_OK != err) {
			MIST_THROW_EXCEPTION(format_string("GLWindow::GLWindow: error initializing OpenGL profilem, error; %s", glewGetErrorString(err)));
		}
#endif
    
        // if wnd pos is (0, 0), then put it in the center of current screen
        int32 wndPosX = settings.left, wndPosY = settings.top;
        if(wndPosX == 0 && wndPosY == 0) {
            Array<SystemInformation::DesktopMode> desktop_modes = SystemInformation::EnumDesktopMode();
            
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
        
        mFrameBuffer->attach(ATT_Color0, RenderViewPtr(new GLScreenColorRenderView(settings.width, 
                                                                                   settings.height,
                                                                                   settings.color_fmt)));
        mFrameBuffer->attach(ATT_DepthStencil, RenderViewPtr(new GLScreenDepthStencilRenderView(settings.width, 
                                                                                                settings.height,
                                                                                                settings.depth_stencil_fmt)));
        
        onResize() += Bind(this, &GLWindow::onWindowResize);
        
        updateWindowProperties(wndPosX, wndPosY, settings.width, settings.height);
    }
    
    GLFrameBufferPtr GLWindow::getFrameBuffer() const {
        return mFrameBuffer;
    }
    
    void GLWindow::setName(const UknString& name) {
        glfwSetWindowTitle(mGlfwWindow,
                           String::WStringToString(name).c_str());
    }
    
    void GLWindow::updateWindowProperties(int32 x, int32 y, uint32 w, uint32 h) {
        mFrameBuffer->mLeft = 0;
        mFrameBuffer->mTop = 0;
        mFrameBuffer->mWidth = w;
        mFrameBuffer->mHeight = h;
        
        Window::mLeft = x;
        Window::mTop = y;
        Window::mWidth = w;
        Window::mHeight = h;
        
        mFrameBuffer->mViewPort.left = 0;
        mFrameBuffer->mViewPort.top = 0;
        mFrameBuffer->mViewPort.width = w;
        mFrameBuffer->mViewPort.height = h;
        
        Viewport& vp = mFrameBuffer->getViewport();
        vp.camera = MakeSharedPtr<Camera2D>();
        ((Camera2D*)vp.camera.get())->setOrthoParams(0, mFrameBuffer->width(), mFrameBuffer->height(), 0);

        vp.camera->update();
    }
    
    void GLWindow::onWindowResize(void* wnd, WindowResizeEventArgs& args) {
        updateWindowProperties(left(), top(), args.width, args.height);

    }
    
    void GLWindow::onWindowMove(Window& wnd, uint32 x, uint32 y) {
        updateWindowProperties(x, y, width(), height());
    }
    
    int2 GLWindow::getMousePos() {
        int2 pos;
        glfwGetMousePos(mGlfwWindow, &pos[0], &pos[1]);
        
        return pos;
    }
    
    int32 GLWindow::getMouseWheel() {
        return 0;
    }
    
    bool GLWindow::isKeyDown(input::Key::KeyCode key) {
        return glfwGetKey(mGlfwWindow, ukn_key_to_glfw(key)) == GLFW_PRESS;
    }
    
    bool GLWindow::isMouseButtonDown(input::Mouse::MouseButton btn) {
        return glfwGetKey(mGlfwWindow, ukn_mouse_to_glfw(btn)) == GLFW_PRESS;
    }
    
    void GLWindow::setMousePos(int32 x, int32 y) {
        glfwSetMousePos(mGlfwWindow, x, y);
    }
    
    bool GLWindow::pullEvents() { 
#ifdef MIST_OS_WINDOWS
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
        
        update_key_flag(mGlfwWindow);
        glfwPollEvents();
        
        return false;
    }

#ifdef MIST_OS_WINDOWS
	HWND GLWindow::getHWnd() const {
		return 0;
	}
#endif
    
    GLWindow::~GLWindow() {
        glfwCloseWindow(mGlfwWindow);
        glfwTerminate();
    }
    
    UknString GLWindow::description() const {
        static UknString des(L"OpenGL Window based on GLFW");
        return des;
    }
     
} // namespce ukn
