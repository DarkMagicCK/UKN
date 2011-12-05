//
//  App.cpp
//  Project Unknown
//
//  Created by Robert Bu on 11/24/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#include "UKN/App.h"
#include "UKN/Window.h"

#include "UKN/Context.h"
#include "UKN/GraphicDevice.h"
#include "UKN/GraphicFactory.h"

#include "UKN/TimeUtil.h"

#include "UKN/Logger.h"
#include "UKN/Common.h"
#include "UKN/SysUtil.h"
#include "UKN/Camera.h"

namespace ukn {
    
    AppInstance::AppInstance(const ukn_string& name):
    mName(name),
    mInited(false) {
        Context::Instance().setApp(this);
    }
    
    AppInstance::~AppInstance() {
        
    }
    
    void AppInstance::onWindowClose(Window& wnd) {
        terminate();
    }
    
    Window& AppInstance::getMainWindow() const {
        return *mMainWindow;
    }

	WindowPtr AppInstance::getMainWindowPtr() const {
		return mMainWindow;
	}
    
    void AppInstance::init(const ukn_wstring& cfgname) {
        mInited = true;
        
        Context::Instance().loadCfgFile(cfgname);
    }
    
    void AppInstance::init(const ContextCfg& cfg) {
        mInited = true;
        
        Context::Instance().setCfg(cfg);
    }
    
    void AppInstance::create() {
        ukn_assert(mInited);
        
        GraphicDevice& graphic_device = Context::Instance().getGraphicFactory().getGraphicDevice();
        
        mMainWindow = graphic_device.createRenderWindow(mName, Context::Instance().getCfg().render_cfg);
        
        if(!mMainWindow)
            return terminate();
        
        mCloseConn = mMainWindow->onClose().connect(Bind(this, &AppInstance::onWindowClose));
        
        // log basic information
        Logger::Instance().setFeature(LF_PrependRunningTime, false);
        
        log_info(format_string("Project Unknown %d.%d Rev %d", 
                               UKN_VERSION_MARJOR,
                               UKN_VERSION_MINOR,
                               UKN_VERSION_REV));
        log_info(get_os_version());
        log_info(format_string("CPU Speed: %d mhz", 
                               get_system_processor_speed()));
        log_info(format_string("Memory Size: %d kb", get_system_memory_size() / 1024));
        
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
        
        // on init
        mMainWindow->onInit()(*mMainWindow);
        onInit();
    }
    
    void AppInstance::terminate() {
#ifdef UKN_OS_WINDOWS
        ::PostQuitMessage(0);
		exit(0);
#else
        exit(0);
#endif
    }
    
    void AppInstance::update() {
        mMainWindow->onUpdate()(*mMainWindow);

        onUpdate();
    }
    
    void AppInstance::render() {
        mMainWindow->onRender()(*mMainWindow);
        
        onRender();
    }
    
    void AppInstance::setCamera(CameraPtr camera) {
        mCamera = camera;
    }
    
    CameraPtr AppInstance::getCamera() const {
        return mCamera;
    }
    
    void AppInstance::onInit() {        
        
    }
    
    void AppInstance::run() {
        if(!mMainWindow) {
            log_error("ukn::AppInstance::run: cannot without a window");
            return ;
        }
        
        Context::Instance().getGraphicFactory().getGraphicDevice().beginRendering();
    }
    
    void AppInstance::onUpdate() {
        
    }
    
    void AppInstance::onRender() {
        
    }
    
} // namespace ukn