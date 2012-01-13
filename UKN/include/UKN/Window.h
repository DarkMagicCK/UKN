//
//  Window.h
//  Project Unknown
//
//  Created by Robert Bu on 11/23/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_Window_h
#define Project_Unknown_Window_h

#include "UKN/Platform.h"
#include "UKN/PreDeclare.h"
#include "UKN/GraphicSettings.h"
#include "UKN/Event.h"

#ifdef UKN_OS_WINDOWS
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
        explicit Window(const ukn_string& name);
        virtual ~Window();
    
        virtual ukn_string description() const = 0;
        
#if defined(UKN_OS_WINDOWS)
        virtual HWND getHWnd() const = 0;
#endif
        
        int32 getLeft() const {
            return mLeft;
        }
        
        int32 getTop() const {
            return mTop;
        }
        
        uint32 getWidth() const {
            return mWidth;
        }
        
        uint32 getHeight() const {
            return mHeight;
        }
        
        // poll windows events
        // PeekMessage on Windows etc
        virtual bool pullEvents() = 0;
        
    protected:
        int32 mLeft;
        int32 mTop;
        uint32 mWidth;
        uint32 mHeight;
        
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
        
        typedef Event<NullEventArgs>            GlobalUpdateEvent;
        
    private:
        WindowCreateEvent mWindowCreateEvent;
        ActiveEvent     mActiveEvent;
        RenderEvent     mRenderEvent;
        UpdateEvent     mUpdateEvent;
        ResizeEvent     mResizeEvent;
        SetCursorEvent  mSetCursorEvent;
        SetIconEvent    mSetIconEvent;
        CloseEvent      mCloseEvent;
        IconifyEvent    mIconifyEvent;
        FrameStartEvent mFrameStart;
        FrameEndEvent   mFrameEnd;
        InitializeEvent mInitEvent;
        
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
        
        static GlobalUpdateEvent& OnGlobalUpdate() {
			static GlobalUpdateEvent globalUpdate;
            return globalUpdate;
        }
    
    protected:
        ukn_string mName;
    };
    
} // namespace ukn


#endif
