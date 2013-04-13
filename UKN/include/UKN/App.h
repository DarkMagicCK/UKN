//
//  App.h
//  Project Unknown
//
//  Created by Robert Bu on 11/24/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_App_h
#define Project_Unknown_App_h

#include "mist/Platform.h"
#include "mist/Uncopyable.h"
#include "mist/Signal.h"
#include "mist/Event.h"

#include "UKN/PreDeclare.h"
#include "UKN/GraphicSettings.h"
#include "UKN/Window.h"

namespace ukn {
    
    // Game App Luncher
    
    class UKN_API AppLauncher: public Uncopyable {
    public:
        AppLauncher(const UknString& name);
        ~AppLauncher();
        
        Window&     getWindow() const;
		WindowPtr   getWindowPtr() const;
        
        AppLauncher& create(const UknString& cfgname=L"config.xml");
        AppLauncher& create(const ContextCfg& cfg);
        
        AppLauncher& run();
        void terminate();
        
        void update();
        void render();
		
	public:
		/* 
			helper functions 
			very handy with lambda functions
		*/
		AppLauncher& connectInit(const Window::InitializeEvent::signal_type::slot_type& f);
		AppLauncher& connectUpdate(const Window::UpdateEvent::signal_type::slot_type& f);
		AppLauncher& connectRender(const Window::RenderEvent::signal_type::slot_type& f);
		AppLauncher& connectMouse(const Window::MouseEvent::signal_type::slot_type& f);
		AppLauncher& connectKey(const Window::KeyEvent::signal_type::slot_type& f);
		AppLauncher& connectClose(const Window::CloseEvent::signal_type::slot_type& f);
		AppLauncher& connectResize(const Window::ResizeEvent::signal_type::slot_type& f);
		AppLauncher& connectFrameStart(const Window:: FrameStartEvent::signal_type::slot_type& f);
		AppLauncher& connectFrameEnd(const Window::FrameEndEvent::signal_type::slot_type& f);
		AppLauncher& connectWindowCreate(const Window::WindowCreateEvent::signal_type::slot_type& f);
        
    private:
        void doCreate();
        void onWindowClose(Window* wnd);
        
        UknString mName;
        bool mInited;    

        WindowPtr mMainWindow;
    };
    
} // namespace ukn

#endif
