//
//  Window.h
//  Project Unknown
//
//  Created by Robert Bu on 11/23/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_Window_h
#define Project_Unknown_Window_h

#include "mist/Platform.h"
#include "mist/Event.h"

#include "UKN/PreDeclare.h"
#include "UKN/GraphicSettings.h"
#include "UKN/Input.h"

#ifdef MIST_OS_WINDOWS
#include <Windows.h>
#endif

namespace ukn {
    
    /**
     * Abstract Window Presentation
     * Implementations depends on platforms and APIs
     **/
    
    struct WindowBoolEventArgs {
        bool flag;
        
        WindowBoolEventArgs(bool f):
        flag(f) {
            
        }
    };
    
    struct WindowResizeEventArgs {
        int32 width;
        int32 height;
        
        WindowResizeEventArgs(int32 w, int32 h):
        width(w),
        height(h) {
            
        }
    };
    
    class UKN_API Window {
    public:
        explicit Window(const UknString& name);
        virtual ~Window();
    
        virtual UknString description() const = 0;
        
#if defined(MIST_OS_WINDOWS)
        virtual HWND getHWnd() const = 0;
#endif
        
        // poll windows events
        // PeekMessage on Windows etc
        virtual bool pullEvents() = 0;
        
        // input 
    public:
        
        virtual int2 getMousePos() = 0;
        virtual int  getMouseWheel() = 0;
        
        virtual bool isKeyDown(input::Key::KeyCode key) = 0;
        virtual bool isMouseButtonDown(input::Mouse::MouseButton btn) = 0;
        
        virtual void setMousePos(int32 x, int32 y) = 0;
        
        void setWindowTitle(const UknString& title);
        const UknString& getWindowTitle() const;
        
    protected:
        UknString mName;
        virtual void setName(const UknString& name) = 0;
        
    public:
        int32  left() const;
        int32  top() const;
        uint32 width() const;
        uint32 height() const;
        
        
    protected:
        int32 mLeft;
        int32 mTop;
        uint32 mWidth;
        uint32 mHeight;
        
    // events
    public:
        typedef Event<NullEventArgs>            WindowCreateEvent;
        typedef Event<WindowBoolEventArgs>      ActiveEvent;
        typedef Event<WindowBoolEventArgs>      IconifyEvent;
        typedef Event<NullEventArgs>            RenderEvent;
        typedef Event<NullEventArgs>            UpdateEvent;
        typedef Event<WindowResizeEventArgs>    ResizeEvent;
        typedef Event<NullEventArgs>            SetCursorEvent;
        typedef Event<NullEventArgs>            SetIconEvent;
        typedef Event<NullEventArgs>            CloseEvent;
        typedef Event<NullEventArgs>            FrameEndEvent;
        typedef Event<NullEventArgs>            FrameStartEvent;
        typedef Event<NullEventArgs>            InitializeEvent;
        
        typedef Event<input::MouseEventArgs>    MouseEvent;
        typedef Event<input::KeyEventArgs>      KeyEvent;
        typedef Event<input::JoyStickEventArgs> JoyStickEvent;
        
        typedef Event<NullEventArgs>            GlobalUpdateEvent;
        
    private:
        WindowCreateEvent   mWindowCreateEvent;
        ActiveEvent         mActiveEvent;
        RenderEvent         mRenderEvent;
        UpdateEvent         mUpdateEvent;
        ResizeEvent         mResizeEvent;
        SetCursorEvent      mSetCursorEvent;
        SetIconEvent        mSetIconEvent;
        CloseEvent          mCloseEvent;
        IconifyEvent        mIconifyEvent;
        FrameStartEvent     mFrameStart;
        FrameEndEvent       mFrameEnd;
        InitializeEvent     mInitEvent;
        MouseEvent          mMouseEvent;
        KeyEvent            mKeyEvent;
        JoyStickEvent       mJoyStickEvent;
        
    public:
        ActiveEvent& onActive() {
            return mActiveEvent;
        }
        
        RenderEvent& onRender() {
            return mRenderEvent;
        }
        
        UpdateEvent& onUpdate() {
            return mUpdateEvent;
        }
        
        ResizeEvent& onResize() {
            return mResizeEvent;
        }
        
        SetCursorEvent& onSetCursor() {
            return mSetCursorEvent;
        }
        
        SetIconEvent& onSetIcon() {
            return mSetIconEvent;
        }
            
        CloseEvent& onClose() {
            return mCloseEvent;
        }
        
        IconifyEvent& onIconify() {
            return mIconifyEvent;
        }
        
        FrameStartEvent& onFrameStart() {
            return mFrameStart;
        }
        
        FrameEndEvent& onFrameEnd() {
            return mFrameEnd;
        }
        
        InitializeEvent& onInit() {
            return mInitEvent;
        }
        
        WindowCreateEvent& onWindowCreate() {
            return mWindowCreateEvent;
        }
        
        MouseEvent& onMouseEvent() {
            return mMouseEvent;
        }
        
        KeyEvent& onKeyEvent() {
            return mKeyEvent;
        }
        
        JoyStickEvent& onJoyStickEvent() {
            return mJoyStickEvent;
        }
        
        static GlobalUpdateEvent& OnGlobalUpdate() {
			static GlobalUpdateEvent globalUpdate;
            return globalUpdate;
        }
        
    };
    
} // namespace ukn


#endif
