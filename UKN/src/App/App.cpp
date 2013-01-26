//
//  App.cpp
//  Project Unknown
//
//  Created by Robert Bu on 11/24/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#include "mist/TimeUtil.h"
#include "mist/Logger.h"
#include "mist/Common.h"
#include "mist/SysUtil.h"
#include "mist/Stream.h"
#include "mist/Module.h"
#include "mist/Thread.h"
#include "mist/FileUtil.h"

#include "UKN/App.h"

#include "UKN/Context.h"
#include "UKN/GraphicDevice.h"
#include "UKN/GraphicFactory.h"
#include "UKN/SpriteBatch.h"

#include "UKN/Camera.h"

#include "UKN/UKN.h"

namespace ukn {
    
    AppLauncher::AppLauncher(const UknString& name):
    mName(name),
    mInited(false) {
        Context::Instance().setApp(this);
    }
    
    AppLauncher::~AppLauncher() {
        
    }
    
    void AppLauncher::onWindowClose(Window* wnd) {
        terminate();
    }
    
    Window& AppLauncher::getWindow() const {
        return *mMainWindow;
    }

	WindowPtr AppLauncher::getWindowPtr() const {
		return mMainWindow;
	}
    
    AppLauncher& AppLauncher::create(const UknString& cfgname) {
        if(!mInited) {
            mist::File::DeleteFile(L"ukn_log.txt");
            mInited = true;
        
            Context::Instance().loadCfgFile(cfgname);
            doCreate();
        } else {
            MIST_THROW_EXCEPTION(L"app already created");
        }
        return *this;
    }
    
    AppLauncher& AppLauncher::create(const ContextCfg& cfg) {
        if(!mInited) {
            mist::File::DeleteFile(L"ukn_log.txt");
            mInited = true;
        
            Context::Instance().setCfg(cfg);
            
            FrameCounter::Instance().setDesiredFps(cfg.desired_fps);
            doCreate();
        } else {
            MIST_THROW_EXCEPTION(L"app already created");
        }
        return *this;
    }
    
    void AppLauncher::doCreate() {
        mist_assert(mInited);
        
		GraphicDevice& graphic_device = Context::Instance().getGraphicFactory().getGraphicDevice();
        	
		try {
			mMainWindow = graphic_device.createRenderWindow(mName, Context::Instance().getCfg().render_cfg);
		} catch(mist::Exception& e) {
			mist::MessageBox::Show(e.what(),
								   L"Error",
								   mist::MBB_OK | mist::MBO_IconError);
		}
        if(!mMainWindow)
            return terminate();
        
        mMainWindow->onClose() += Bind(this, &AppLauncher::onWindowClose);
        
        // log basic information
        Logger::Instance().setFeature(LF_PrependRunningTime, false);
        
        log_info(format_string("Project Unknown %d.%d Rev %d", 
                               UKN_VERSION_MAJOR,
                               UKN_VERSION_MINOR,
                               UKN_VERSION_REV));
        log_info(SystemInformation::GetOSVersion());
        log_info(format_string("CPU Speed: %d mhz", 
                               SystemInformation::GetProcessorSpeed()));
        log_info(format_string("Memory Size: %d kb", SystemInformation::GetMemorySize() / 1024));
        
        CpuInfo cpuinfo;
        log_info(format_string("CPU: %s, %s, Cores: %d Threads: %d", 
                               cpuinfo.getCPUString().c_str(),
                               cpuinfo.getCPUBrandString().c_str(),
                               cpuinfo.getNumCores(),
                               cpuinfo.getNumHWThreads()));
      
        
        log_info(graphic_device.description());
        
        GraphicDeviceCaps caps;
        graphic_device.fillGraphicCaps(caps);
        log_info(format_string("Graphic Device Caps:\n\tMaxTextureSize:\t(%d, %d)\n\tMaxCubeMapSize:\t%d\n\tMaxIndices:\t%d\n\tMaxVertices:\t%d\n\tMaxPixelTextureUnits:\t%d\n\tMaxVertexTextureUnits:\t%d\n",
                               caps.max_texture_width,
                               caps.max_texture_height,
                               caps.max_texture_cube_map_size,
                               caps.max_indices,
                               caps.max_vertices,
                               caps.max_pixel_texture_units,
                               caps.max_vertex_texture_units));
        
        Logger::Instance().setFeature(LF_PrependRunningTime, true);
        
    }
    
    void AppLauncher::terminate() {
        mist::ModuleManager::Destroy();
        
        try {
#ifdef MIST_OS_WINDOWS
        ::PostQuitMessage(0);
#else
        exit(0);
#endif
        } catch(...) {
            
        }
    }
    
    void AppLauncher::update() {
        mMainWindow->OnGlobalUpdate().raise(0, _NullEventArgs);
        mMainWindow->onUpdate().raise(mMainWindow, _NullEventArgs);
        mist::thread::ThreadTaskPool::DefaultObject().run();
    }
    
    void AppLauncher::render() {
        mMainWindow->onRender().raise(mMainWindow, _NullEventArgs);
    }
    
    AppLauncher& AppLauncher::run() {
        if(!mMainWindow) {
            log_error("ukn::AppLauncher::run: cannot without a AppLauncher");
            return *this;
        }
        
        // on init
        mMainWindow->onInit().raise(mMainWindow, _NullEventArgs);
        
        Context::Instance().getGraphicFactory().getGraphicDevice().beginRendering();

		return *this;
    }
    
    
	AppLauncher& AppLauncher::connectInit(const Window::InitializeEvent::signal_type::SlotType& f) {
		mMainWindow->connectInit(f);
		return *this;
	}

	AppLauncher& AppLauncher::connectUpdate(const Window::UpdateEvent::signal_type::SlotType& f) {
		mMainWindow->connectUpdate(f);
		return *this;
	}

	AppLauncher& AppLauncher::connectRender(const Window::RenderEvent::signal_type::SlotType& f) {
		mMainWindow->connectRender(f);
		return *this;
	}

	AppLauncher& AppLauncher::connectMouse(const Window::MouseEvent::signal_type::SlotType& f) {
		mMainWindow->connectMouse(f);
		return *this;
	}

	AppLauncher& AppLauncher::connectKey(const Window::KeyEvent::signal_type::SlotType& f) {
		mMainWindow->connectKey(f);
		return *this;
	}

	AppLauncher& AppLauncher::connectClose(const Window::CloseEvent::signal_type::SlotType& f) {
		mMainWindow->connectClose(f);
		return *this;
	}

	AppLauncher& AppLauncher::connectResize(const Window::ResizeEvent::signal_type::SlotType& f) {
		mMainWindow->connectResize(f);
		return *this;
	}

	AppLauncher& AppLauncher::connectFrameStart(const Window::FrameStartEvent::signal_type::SlotType& f) {
		mMainWindow->connectFrameStart(f);
		return *this;
	}

	AppLauncher& AppLauncher::connectFrameEnd(const Window::FrameEndEvent::signal_type::SlotType& f) {
		mMainWindow->connectFrameEnd(f);
		return *this;
	}

	AppLauncher& AppLauncher::connectWindowCreate(const Window::WindowCreateEvent::signal_type::SlotType& f) {
		mMainWindow->connectWindowCreate(f);
		return *this;
	}
} // namespace ukn