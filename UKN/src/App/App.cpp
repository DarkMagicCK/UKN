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

namespace ukn {
    
    AppInstance::AppInstance(const ukn_string& name):
    mName(name),
    mInited(false) {
        
    }
    
    AppInstance::~AppInstance() {
        
    }
    
    void AppInstance::onWindowClose(Window& wnd) {
        terminate();
    }
    
    Window& AppInstance::getMainWindow() const {
        return *mMainWindow;
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
        
        if(!mMainWindow.isValid())
            return terminate();
        
        mCloseConn = mMainWindow->onClose().connect(Bind(this, &AppInstance::onWindowClose));
    }
    
    void AppInstance::terminate() {
#ifdef UKN_OS_WINDOWS
        ::PostQuitMessage(0);
#else
        exit(0);
#endif
    }
    
    void AppInstance::run() {
        FrameCounter& counter = FrameCounter::Instance();
        
        while(true) {
            counter.waitToNextFrame();
            
            mMainWindow->onFrameStart()(*mMainWindow);
            
            onUpdate();
            mMainWindow->onUpdate()(*mMainWindow);
            
            onRender();
            mMainWindow->onRender()(*mMainWindow);
            
            mMainWindow->onFrameEnd()(*mMainWindow);
        }
    }
    
    void AppInstance::onUpdate() {
        
    }
    
    void AppInstance::onRender() {
        
    }
    
} // namespace ukn