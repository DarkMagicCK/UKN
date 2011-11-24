//
//  Window.cpp
//  Project Unknown
//
//  Created by Ruiwei Bu on 11/24/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#include "Window.h"

#if defined(UKN_OS_WINDOWS) 

#include <windows.h>

namespace ukn {
    
    class WindowsWindow: public Window {
    public:
        WindowsWindow(const ukn_wstring& name, const RenderSettings& settings):
        Window(name) {
            
        }
        
        virtual HWND getHWnd() const {
            return mHWnd;
        }
        
        virtual const ukn_wstring& description() const {
            static ukn_wstring const des(L"Windows Window");
            return des;
        }
        
        static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LAPRAM lParam);
        
        LRESULT MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
        
        HWND mHWnd;
    };
    
    class WindowsWindowFactory: public WindowFactory {
    public:
        WindowPtr createWindow(const ukn_wstring& name, const RenderSettings& settings) {
            return MakeSharedPtr<WindowsWindow>(name, settings);
        }
    };
}

#endif // UKN_OS_WINDOWS

#if defined(UKN_OS_LINUX) || defined(UKN_OS_OSX)

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

namespace ukn {
    
    class X11Window: public Window {
    public:
        X11Window(const ukn_wstring& name, const RenderSettings& settings):
        Window(name) {
            
        }
        
        virtual const ukn_wstring& description() const {
            static ukn_wstring const des(L"X11 Window");
            return des;
        }
        
        ::Display* XDisplay() const {
            return mXDisplay;
        }
        
        ::XVisualInfo* VisualInfo() const {
            return mVI;
        }
        
        ::Window XWindow() const {
            return mXWindow;
        }
        
        ::Display* mXDisplay;
#if defined(UKN_OS_LINUX)
        ::GLXFBConfig* mFBC;
#endif // UKN_OS_LINUX
        ::XVisualInfo* mVI;
        ::Window mXWindow;
        ::Atom mDeleteWindow;
    };
    
    class X11WindowFactory: public WindowFactory {
    public:
        WindowPtr createWindow(const ukn_wstring& name, const RenderSettings& settings) {
            return MakeSharedPtr<X11Window>(name, settings);
        }
    };
    
} // namespace ukn

#endif // UKN_OS_LINUX

namespace ukn {
    
    class NullWindow: public Window {
    public:
        NullWindow():
        Window(L"Null") {
             
        }
        
        const ukn_wstring& description() const {
            static ukn_wstring des(L"Null Window");
            return des;
        }
        
#if defined(UKN_OS_WINDOWS)
        HWND getHWnd() const {
            return 0;
        }
#endif
        
    };
    
    class NullWindowFactory: public WindowFactory {
    public:
        WindowPtr createWindow(const ukn_wstring& name, const RenderSettings& settings) {
            return MakeSharedPtr<NullWindow>();
        }
    };
    
    WindowFactoryPtr WindowFactory::NullObject() {
        static WindowFactoryPtr static_ptr = MakeSharedPtr<NullWindowFactory>();
        return static_ptr;
    }
    
    Window::Window(const ukn_wstring& name):
    mName(name) {
        
    }
    
    Window::~Window() {
        
    }
    
    
} // namespace ukn