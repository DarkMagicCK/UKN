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
        typedef EventObject<void(Window&, bool)>               ActiveEvent;
        typedef EventObject<void(Window&, bool)>               IconifyEvent;
        typedef EventObject<void(Window&)>                     RenderEvent;
        typedef EventObject<void(Window&)>                     UpdateEvent;
        typedef EventObject<void(Window&, uint32, uint32)>     ResizeEvent;
        typedef EventObject<void(Window&, int32, int32)>       ScrollEvent;
        typedef EventObject<void(Window&)>                     SetCursorEvent;
        typedef EventObject<void(Window&)>                     SetIconEvent;
        typedef EventObject<void(Window&, int32 /* char */)>   CharEvent;
        typedef EventObject<void(Window&, uint32 /* key */)>   KeyDownEvent;
        typedef EventObject<void(Window&, uint32 /* key */)>   KeyUpEvent;
        typedef EventObject<void(Window&, uint32 /* btn */)>   MouseDownEvent;
        typedef EventObject<void(Window&, uint32 /* btn */)>   MouseUpEvent;
        typedef EventObject<void(Window&, uint32 /* wheel */)> MouseWheelEvent;
        typedef EventObject<void(Window&)>                     CloseEvent;
        typedef EventObject<void(Window&)>                     FrameEndEvent;
        typedef EventObject<void(Window&)>                     FrameStartEvent;
        typedef EventObject<void(Window&)>                     InitializeEvent;
        typedef EventObject<void(Window&, uint32 /* btn */, uint32, uint32 /* pos */)>   MouseDraggedEvent;
        
    private:
        ActiveEvent     mActiveEvent;
        RenderEvent     mRenderEvent;
        UpdateEvent     mUpdateEvent;
        ResizeEvent     mResizeEvent;
        SetCursorEvent  mSetCursorEvent;
        SetIconEvent    mSetIconEvent;
        KeyDownEvent    mKeyDownEvent;
        KeyUpEvent      mKeyUpEvent;
        MouseDownEvent  mMouseDownEvent;
        MouseUpEvent    mMouseUpEvent;
        MouseWheelEvent mMouseWheelEvent;
        CloseEvent      mCloseEvent;
        IconifyEvent    mIconifyEvent;
        ScrollEvent     mScrollEvent;
        CharEvent       mCharEvent;
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
        
        KeyDownEvent& onKeyDown() {
            return mKeyDownEvent;
        }
        
        KeyUpEvent& onKeyUp() {
            return mKeyUpEvent;
        }
        
        MouseDownEvent& onMouseDown() {
            return mMouseDownEvent;
        }
        
        MouseUpEvent& onMouseUp() {
            return mMouseUpEvent;
        }
        
        CloseEvent& onClose() {
            return mCloseEvent;
        }
        
        IconifyEvent& onIconify() {
            return mIconifyEvent;
        }
        
        CharEvent& onChar() {
            return mCharEvent;
        }
        
        ScrollEvent& onScroll() {
            return mScrollEvent;
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
    
    protected:
        ukn_string mName;
    };
    
} // namespace ukn


#endif
