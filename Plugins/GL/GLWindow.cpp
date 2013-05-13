//
//  GLWindow.cpp
//  Project Unknown
//
//  Created by Robert Bu on 11/24/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#include "GLWindow.h"
#include "GLRenderView.h"

#include "mist/Logger.h"
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

    static void update_key_flag(GLFWwindow* window) {
        g_key_flag = 0;
        if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT))
            g_key_flag |= input::FlagShift;
        if(glfwGetKey(window, GLFW_KEY_LEFT_ALT) || glfwGetKey(window, GLFW_KEY_RIGHT_ALT))
            g_key_flag |= input::FlagAlt;
        if(glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) || glfwGetKey(window, GLFW_KEY_RIGHT_ALT))
            g_key_flag |= input::FlagCtrl;
        if(glfwGetKey(window, GLFW_KEY_CAPS_LOCK))
            g_key_flag |= input::FlagCapslock;
        if(glfwGetKey(window, GLFW_KEY_SCROLL_LOCK))
            g_key_flag |= input::FlagScrolllock;
        if(glfwGetKey(window, GLFW_KEY_NUM_LOCK))
            g_key_flag |= input::FlagNumlock;
    }

    static void WindowSizeFunc(GLFWwindow* window, int w, int h) {
        GLWindow* glwnd = (GLWindow*)glfwGetWindowUserPointer(window);

        WindowResizeEventArgs args(w, h);
        glwnd->onResize().raise(glwnd, args);
    }

    static int WindowCloseFunc(GLFWwindow* window) {
        GLWindow* glwnd = (GLWindow*)glfwGetWindowUserPointer(window);

        glwnd->onClose().raise(glwnd, _NullEventArgs);
        return 1;
    }

    static void WindowRefreshFunc(GLFWwindow* window) {
        //    GLWindow* glwnd = (GLWindow*)glfwGetWindowUserPointer(window);

    }

    static void WindowFocusFunc(GLFWwindow* window, int f) {
        GLWindow* glwnd = (GLWindow*)glfwGetWindowUserPointer(window);

        WindowBoolEventArgs args(f > 0 ? true: false);
        glwnd->onActive().raise(glwnd, args);
    }

    static void WindowIconifyFunc(GLFWwindow* window, int f) {
        GLWindow* glwnd = (GLWindow*)glfwGetWindowUserPointer(window);

        WindowBoolEventArgs args(f > 0 ? true: false);
        glwnd->onIconify().raise(glwnd, args);
    }

    static void MouseButtonFunc(GLFWwindow* window, int btn, int state) {
        GLWindow* glwnd = (GLWindow*)glfwGetWindowUserPointer(window);

        input::MouseEventArgs args;
        args.button = glfw_to_ukn_mouse(btn);
        args.state  = glfw_to_ukn_mouse_state(state);
        args.flag   = g_key_flag;

        // retrieve mouse pos
        glfwGetCursorPos(window, &args.x, &args.y);

        glwnd->onMouseEvent().raise(glwnd, args);
    }

    static void MousePosFunc(GLFWwindow* window, int x, int y) {
        GLWindow* glwnd = (GLWindow*)glfwGetWindowUserPointer(window);

        input::MouseEventArgs args;
        args.button = input::Nothing;
        args.state  = input::Move;
        args.flag   = g_key_flag;
        args.x      = x;
        args.y      = y;

        glwnd->onMouseEvent().raise(glwnd, args);
    }

    static void ScrollFunc(GLFWwindow* window, double a, double b) {
        GLWindow* glwnd = (GLWindow*)glfwGetWindowUserPointer(window);
        
        input::MouseEventArgs args;
        args.button = input::Nothing;
        args.state  = input::Wheel;
        args.flag   = g_key_flag;
        args.x      = a;
        args.y      = b;
        args.wheel  = 0;

        glwnd->onMouseEvent().raise(glwnd, args);
    }

    static void KeyFunc(GLFWwindow* window, int key, int state) {
        GLWindow* glwnd = (GLWindow*)glfwGetWindowUserPointer(window);
        glwnd->onKeyDown(glfw_to_ukn_key(key), state);
        
        
        input::KeyEventArgs args;
        args.key    = glfw_to_ukn_key(key);
        args.state  = state == 1 ? input::Press : input::Release;
        args.isIME  = false;
        args.flag   = g_key_flag;
        
        glwnd->onKeyEvent().raise(glwnd, args);
    }

    static void CharFunc(GLFWwindow* window, int c) {
     /*   GLWindow* glwnd = (GLWindow*)glfwGetWindowUserPointer(window);
        
        input::KeyEventArgs args;
        args.key    = c;
        args.state  = input::Press;
        args.isIME  = true;
        args.flag   = g_key_flag;
        
        glwnd->onKeyEvent().raise(glwnd, args);*/
    }
    
    static void ErrorCallback(int,const char* error) {
        log_error(L"GLFW Error: " + string::StringToWString(error));
    }

    GLWindow::GLWindow(const UknString& name, const RenderSettings& settings):
        mFrameBuffer(new GLFrameBuffer(false)),
        Window(name) {
            glfwSetErrorCallback(ErrorCallback);
            glfwInit();

            switch(settings.color_fmt) {
            case EF_RGBA8:
                glfwWindowHint(GLFW_RED_BITS, 8);
                glfwWindowHint(GLFW_BLUE_BITS, 8);
                glfwWindowHint(GLFW_GREEN_BITS, 8);
                glfwWindowHint(GLFW_ALPHA_BITS, 8);
                break;

            default:
                // error
                break;
            }

            switch(settings.depth_stencil_fmt) {
            case EF_D16:
                glfwWindowHint(GLFW_DEPTH_BITS, 16);
                glfwWindowHint(GLFW_STENCIL_BITS, 0);

                mFrameBuffer->mIsDepthBuffered = true;
                mFrameBuffer->mDepthBits = 16;
                mFrameBuffer->mStencilBits = 0;
                break;

            case EF_D24S8:
                glfwWindowHint(GLFW_DEPTH_BITS, 24);
                glfwWindowHint(GLFW_STENCIL_BITS, 8);

                mFrameBuffer->mIsDepthBuffered = true;
                mFrameBuffer->mDepthBits = 24;
                mFrameBuffer->mStencilBits = 8;
                break;

            default:
                glfwWindowHint(GLFW_DEPTH_BITS, 0);
                glfwWindowHint(GLFW_STENCIL_BITS, 0);

                mFrameBuffer->mIsDepthBuffered = false;
                mFrameBuffer->mDepthBits = 0;
                mFrameBuffer->mStencilBits = 0;
                break;
            }

            if(settings.sample_quality > 0) {
                glfwWindowHint(GLFW_SAMPLES, settings.sample_quality);
            }

            glfwWindowHint(GLFW_RESIZABLE, settings.resizable);

            /* maybe there are bugs... let glfw decide the best version */
            /*
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, UKN_OPENGL_VERSION_MAJOR);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, UKN_OPENGL_VERSION_MINOR);
            if(UKN_OPENGL_VERSION_MAJOR >= 3) {
                glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#if defined(MIST_OS_OSX)
                glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#else
                glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
#endif
            }
            */
            if(!settings.native_window_handle) {
                if((mGlfwWindow = glfwCreateWindow(settings.width,
                                                   settings.height,
                                                   string::WStringToString(name).c_str(),
                                                   settings.full_screen ? glfwGetPrimaryMonitor() : 0,
                                                   0)) == 0) {
                        MIST_THROW_EXCEPTION(L"GLWindow::GLWindow: Error opening window");
                }
            } else {
                if((mGlfwWindow = glfwCreateWindowSlave((void*)settings.native_window_handle,
                                                        0)) == 0) {
                    MIST_THROW_EXCEPTION(L"GLWindow::GLWindow: Error creating slave window on native handle");
                }
            }
            glfwMakeContextCurrent(mGlfwWindow);

#if !defined(UKN_OSX_REQUEST_OPENGL_32_CORE_PROFILE)
            GLenum err = glewInit();
            if (GLEW_OK != err) {
                    MIST_THROW_EXCEPTION(mist::string::StringToWString(format_string("GLWindow::GLWindow: error initializing glew profilem, error; %s", glewGetErrorString(err))));
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
            glfwSetWindowSizeCallback(mGlfwWindow, WindowSizeFunc);
            glfwSetWindowCloseCallback(mGlfwWindow, WindowCloseFunc);
            glfwSetWindowRefreshCallback(mGlfwWindow, WindowRefreshFunc);
            glfwSetWindowFocusCallback(mGlfwWindow, WindowFocusFunc);
            glfwSetWindowIconifyCallback(mGlfwWindow, WindowIconifyFunc);

            glfwSetKeyCallback(mGlfwWindow, KeyFunc);
            glfwSetCursorPosCallback(mGlfwWindow, MousePosFunc);
            glfwSetMouseButtonCallback(mGlfwWindow, MouseButtonFunc);
            glfwSetScrollCallback(mGlfwWindow, ScrollFunc);
            glfwSetCharCallback(mGlfwWindow, CharFunc);
            
            // to do with joysticks

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
            string::WStringToString(name).c_str());
    }

    void GLWindow::updateWindowProperties(int32 x, int32 y, uint32 w, uint32 h) {
        mFrameBuffer->updateScreenSize(0, 0, w, h);

        Window::mLeft = x;
        Window::mTop = y;
        Window::mWidth = w;
        Window::mHeight = h;
    }

    void GLWindow::onWindowResize(void* wnd, WindowResizeEventArgs& args) {
        updateWindowProperties(left(), top(), args.width, args.height);

    }

    void GLWindow::onWindowMove(Window& wnd, uint32 x, uint32 y) {

    }

    int2 GLWindow::getMousePos() {
        int2 pos;
        glfwGetCursorPos(mGlfwWindow, &pos[0], &pos[1]);

        return pos;
    }

    int32 GLWindow::getMouseWheel() {
        return 0;
    }

    bool GLWindow::isKeyDown(input::KeyCode key) {
        return glfwGetKey(mGlfwWindow, ukn_key_to_glfw(key)) == GLFW_PRESS;
    }

    bool GLWindow::isMouseButtonDown(input::MouseButton btn) {
        return glfwGetKey(mGlfwWindow, ukn_mouse_to_glfw(btn)) == GLFW_PRESS;
    }

    void GLWindow::setMousePos(int32 x, int32 y) {
        glfwSetCursorPos(mGlfwWindow, x, y);
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
        
        for(int i=0; i<256; ++i) {
            std::pair<int, bool>& keyStatus = mKeyStatus[i];
            if(keyStatus.second) {
                input::KeyEventArgs args;
                args.key    = mKeyStatus[i].first;
                args.state  = input::Press;
                args.flag   = g_key_flag;
                
                this->onKeyEvent().raise(this, args);
            }
        }

        return false;
    }

#ifdef MIST_OS_WINDOWS
    HWND GLWindow::getHWnd() const {
        return 0;
    }
#endif

    GLWindow::~GLWindow() {
        glfwTerminate();
    }

    UknString GLWindow::description() const {
        static UknString des(string::StringToWString(format_string("OpenGL Window based on GLFW %s",
                                                                   glfwGetVersionString())));
        return des;
    }
    
    GLFWwindow* GLWindow::getGLFWWindow() const {
        return mGlfwWindow;
    }

    void GLWindow::swapBuffers() {
        glfwSwapBuffers(mGlfwWindow);
    }
    
    void GLWindow::onKeyDown(int key, int state) {
        if(state == GLFW_PRESS)
            mKeyStatus[key].second = true;
        else
            mKeyStatus[key].second = false;;
    }
    
    void GLWindow::onMouseDown(int btn, int state) {
        
        
    }
    
} // namespce ukn
