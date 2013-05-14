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

	Connection Window::connectInit(const Window::InitializeEvent::signal_type::slot_type& f) {
		return mInitEvent.connect(f);
	}

	Connection Window::connectUpdate(const Window::UpdateEvent::signal_type::slot_type& f) {
		return mUpdateEvent.connect(f);
	}

	Connection Window::connectRender(const Window::RenderEvent::signal_type::slot_type& f) {
		return mRenderEvent.connect(f);
	}

	Connection Window::connectMouse(const Window::MouseEvent::signal_type::slot_type& f) {
		return mMouseEvent.connect(f);
	}

	Connection Window::connectKey(const Window::KeyEvent::signal_type::slot_type& f) {
		return mKeyEvent.connect(f);
	}

	Connection Window::connectClose(const Window::CloseEvent::signal_type::slot_type& f) {
		return mCloseEvent.connect(f);
    }

	Connection Window::connectResize(const Window::ResizeEvent::signal_type::slot_type& f) {
		return mResizeEvent.connect(f);
	}

	Connection Window::connectFrameStart(const Window::FrameStartEvent::signal_type::slot_type& f) {
		return mFrameStartEvent.connect(f);
	}

	Connection Window::connectFrameEnd(const Window::FrameEndEvent::signal_type::slot_type& f) {
		return mFrameEndEvent.connect(f);
	}

	Connection Window::connectWindowCreate(const Window::WindowCreateEvent::signal_type::slot_type& f) {
		return mWindowCreateEvent.connect(f);
	}
    
    
} // namespace ukn