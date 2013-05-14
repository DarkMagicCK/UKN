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
        
        virtual bool isKeyDown(input::KeyCode key) = 0;
        virtual bool isMouseButtonDown(input::MouseButton btn) = 0;
        
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
        typedef Event<NullEventArgs, Window*>            WindowCreateEvent;
        typedef Event<WindowBoolEventArgs, Window*>      ActiveEvent;
        typedef Event<WindowBoolEventArgs, Window*>      IconifyEvent;
        typedef Event<NullEventArgs, Window*>            RenderEvent;
        typedef Event<NullEventArgs, Window*>            UpdateEvent;
        typedef Event<WindowResizeEventArgs, Window*>    ResizeEvent;
        typedef Event<NullEventArgs, Window*>            SetCursorEvent;
        typedef Event<NullEventArgs, Window*>            SetIconEvent;
        typedef Event<NullEventArgs, Window*>            CloseEvent;
        typedef Event<NullEventArgs, Window*>            FrameEndEvent;
        typedef Event<NullEventArgs, Window*>            FrameStartEvent;
        typedef Event<NullEventArgs, Window*>            InitializeEvent;
        
        typedef Event<input::MouseEventArgs, Window*>    MouseEvent;
        typedef Event<input::KeyEventArgs, Window*>      KeyEvent;
        typedef Event<input::JoyStickEventArgs, Window*> JoyStickEvent;
        
        typedef Event<NullEventArgs, Window*>            GlobalUpdateEvent;
        
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
        FrameStartEvent     mFrameStartEvent;
        FrameEndEvent       mFrameEndEvent;
        InitializeEvent     mInitEvent;
        MouseEvent          mMouseEvent;
        KeyEvent            mKeyEvent;
        JoyStickEvent       mJoyStickEvent;
        
    public:
        ActiveEvent& onActive();
        RenderEvent& onRender();
        UpdateEvent& onUpdate();
        ResizeEvent& onResize();
        SetCursorEvent& onSetCursor();
        SetIconEvent& onSetIcon();
        CloseEvent& onClose();
        IconifyEvent& onIconify();
        FrameStartEvent& onFrameStart();
        FrameEndEvent& onFrameEnd();
        InitializeEvent& onInit();
        WindowCreateEvent& onWindowCreate();
        MouseEvent& onMouseEvent();
        KeyEvent& onKeyEvent();
        JoyStickEvent& onJoyStickEvent();
        
        static GlobalUpdateEvent& OnGlobalUpdate();
        
	public:
		/* 
			helper functions 
			very handy with lambda functions
		*/
		Connection connectInit(const InitializeEvent::signal_type::slot_type& f);
		Connection connectUpdate(const UpdateEvent::signal_type::slot_type& f);
		Connection connectRender(const RenderEvent::signal_type::slot_type& f);
		Connection connectMouse(const MouseEvent::signal_type::slot_type& f);
		Connection connectKey(const KeyEvent::signal_type::slot_type& f);
		Connection connectClose(const CloseEvent::signal_type::slot_type& f);
		Connection connectResize(const ResizeEvent::signal_type::slot_type& f);
		Connection connectFrameStart(const FrameStartEvent::signal_type::slot_type& f);
		Connection connectFrameEnd(const FrameEndEvent::signal_type::slot_type& f);
		Connection connectWindowCreate(const WindowCreateEvent::signal_type::slot_type& f);
    };
    
} // namespace ukn


#endif
