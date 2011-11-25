//
//  Window.h
//  Project Unknown
//
//  Created by Ruiwei Bu on 11/23/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_Window_h
#define Project_Unknown_Window_h

#include "Platform.h"
#include "PreDeclare.h"
#include "GraphicSettings.h"

#include "Signal.h"

namespace ukn {
    
    /**
     * Abstract Window Presentation
     * Implementations depends on platforms and APIs
     **/
    
    class UKN_API Window {
    public:
        explicit Window(const ukn_string& name);
        virtual ~Window();
    
        virtual const ukn_wstring& description() const = 0;
        
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
        
    protected:
        int32 mLeft;
        int32 mTop;
        uint32 mWidth;
        uint32 mHeight;
        
    public:
        typedef Signal<void(const Window&, bool)>               ActiveEvent;
        typedef Signal<void(const Window&, bool)>               IconifyEvent;
        typedef Signal<void(const Window&)>                     RenderEvent;
        typedef Signal<void(const Window&)>                     UpdateEvent;
        typedef Signal<void(const Window&, uint32, uint32)>     ResizeEvent;
        typedef Signal<void(const Window&, int32, int32)>       ScrollEvent;
        typedef Signal<void(const Window&)>                     SetCursorEvent;
        typedef Signal<void(const Window&)>                     SetIconEvent;
        typedef Signal<void(const Window&, int32 /* char */)>   CharEvent;
        typedef Signal<void(const Window&, uint32 /* key */)>   KeyDownEvent;
        typedef Signal<void(const Window&, uint32 /* key */)>   KeyUpEvent;
        typedef Signal<void(const Window&, uint32 /* btn */)>   MouseDownEvent;
        typedef Signal<void(const Window&, uint32 /* btn */)>   MouseUpEvent;
        typedef Signal<void(const Window&, uint32 /* wheel */)> MouseWheelEvent;
        typedef Signal<void(const Window&)>                     CloseEvent;
        typedef Signal<void(const Window&)>                     FrameEndEvent;
        typedef Signal<void(const Window&)>                     FrameStartEvent;
        typedef Signal<void(const Window&, uint32 /* btn */, uint32, uint32 /* pos */)>   MouseDraggedEvent;
        
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
    
    protected:
        ukn_string mName;
    };
    
} // namespace ukn


#endif
