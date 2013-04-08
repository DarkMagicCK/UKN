//
//  Window.cpp
//  Project Unknown
//
//  Created by Robert Bu on 11/24/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#include "UKN/Window.h"

namespace ukn {

    Window::Window(const UknString& name):
    mName(name) {
        
    }
    
    Window::~Window() {
        
    }
    
    void Window::setWindowTitle(const UknString& title) {
        mName = title;
        this->setName(title);
    }
    
    const UknString& Window::getWindowTitle() const {
        return mName;
    }
    
    int32 Window::left() const {
        return mLeft;
    }
    
    int32 Window::top() const {
        return mTop;
    }
    
    uint32 Window::width() const {
        return mWidth;
    }
    
    uint32 Window::height() const {
        return mHeight;
    }

	Window::ActiveEvent& Window::onActive() {
        return mActiveEvent;
    }
        
    Window::RenderEvent& Window::onRender() {
        return mRenderEvent;
    }
        
    Window::UpdateEvent& Window::onUpdate() {
        return mUpdateEvent;
    }
        
    Window::ResizeEvent& Window::onResize() {
        return mResizeEvent;
    }
        
    Window::SetCursorEvent& Window::onSetCursor() {
        return mSetCursorEvent;
    }
        
    Window::SetIconEvent& Window::onSetIcon() {
        return mSetIconEvent;
    }
            
    Window::CloseEvent& Window::onClose() {
        return mCloseEvent;
    }
        
    Window::IconifyEvent& Window::onIconify() {
        return mIconifyEvent;
    }
        
    Window::FrameStartEvent& Window::onFrameStart() {
        return mFrameStartEvent;
    }
        
    Window::FrameEndEvent& Window::onFrameEnd() {
        return mFrameEndEvent;
    }
        
    Window::InitializeEvent& Window::onInit() {
        return mInitEvent;
    }
        
    Window::WindowCreateEvent& Window::onWindowCreate() {
        return mWindowCreateEvent;
    }
        
    Window::MouseEvent& Window::onMouseEvent() {
        return mMouseEvent;
    }
        
    Window::KeyEvent& Window::onKeyEvent() {
        return mKeyEvent;
    }
        
    Window::JoyStickEvent& Window::onJoyStickEvent() {
        return mJoyStickEvent;
    }
        
    Window::GlobalUpdateEvent& Window::OnGlobalUpdate() {
		static GlobalUpdateEvent globalUpdate;
        return globalUpdate;
    }

	Window& Window::connectInit(const Window::InitializeEvent::signal_type::slot_type& f) {
		mInitEvent.connect(f);
		return *this;
	}

	Window& Window::connectUpdate(const Window::UpdateEvent::signal_type::slot_type& f) {
		mUpdateEvent.connect(f);
		return *this;
	}

	Window& Window::connectRender(const Window::RenderEvent::signal_type::slot_type& f) {
		mRenderEvent.connect(f);
		return *this;
	}

	Window& Window::connectMouse(const Window::MouseEvent::signal_type::slot_type& f) {
		mMouseEvent.connect(f);
		return *this;
	}

	Window& Window::connectKey(const Window::KeyEvent::signal_type::slot_type& f) {
		mKeyEvent.connect(f);
		return *this;
	}

	Window& Window::connectClose(const Window::CloseEvent::signal_type::slot_type& f) {
		mCloseEvent.connect(f);
		return *this;
	}

	Window& Window::connectResize(const Window::ResizeEvent::signal_type::slot_type& f) {
		mResizeEvent.connect(f);
		return *this;
	}

	Window& Window::connectFrameStart(const Window::FrameStartEvent::signal_type::slot_type& f) {
		mFrameStartEvent.connect(f);
		return *this;
	}

	Window& Window::connectFrameEnd(const Window::FrameEndEvent::signal_type::slot_type& f) {
		mFrameEndEvent.connect(f);
		return *this;
	}

	Window& Window::connectWindowCreate(const Window::WindowCreateEvent::signal_type::slot_type& f) {
		mWindowCreateEvent.connect(f);
		return *this;
	}
    
    
} // namespace ukn