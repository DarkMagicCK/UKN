//
//  AppContext.cpp
//  Project Unknown
//
//  Created by Ruiwei Bu on 11/24/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#include "AppContext.h"
#include "GraphicDevice.h"
#include "GraphicFactory.h"
#include "Window.h"
#include "App.h"

namespace ukn {
    
    typedef void (*CreateGraphicDeviceFunc)(GraphicDevicePtr&);
    typedef void (*CreateGraphicFactoryFunc)(GraphicFactoryPtr&);
    typedef void (*CreateWindowFactoryFunc)(WindowFactoryPtr&);
    
    AppContext::AppContext() {
        mGraphicDevice = GraphicDevice::NullObject();
        mGraphicFactory = GraphicFactory::NullObject();
        mWindowFactory = WindowFactory::NullObject();
    }
    
    AppContext::~AppContext() {
        mGraphicDeviceLoader.close();
        mGraphicFactoryLoader.close();
        mWindowFactoryLoader.close();
    }
    
    void AppContext::setApp(AppPtr app) {
        mApp = app;
    }
    
    AppInstance& AppContext::getAppInstance() const {
        return *mApp;
    }
    
    void AppContext::setCfg(AppContextCfg& cfg) {
        mCfg = cfg;
        
        loadGraphicDevice(cfg.graphic_device_name);
        loadGraphicFactory(cfg.graphic_factory_name);
        loadWindowFactory(cfg.window_factory_name);
    }
    
    AppContextCfg AppContext::getCfg() const {
        return mCfg;
    }
    
    void AppContext::loadCfgFile(const ukn_string& name) {
        // todo
    }
    
    void AppContext::saveCfgFile(const ukn_string& name) {
        // todo
    }
    
    GraphicDevicePtr AppContext::getGraphicDevicePtr() const {
        return mGraphicDevice;
    }
    
    GraphicFactoryPtr AppContext::getGraphicFactoryPtr() const {
        return mGraphicFactory;
    }
    
    WindowFactoryPtr AppContext::getWindowFactoryPtr() const {
        return mWindowFactory;
    }
    
    GraphicDevice& AppContext::getGraphicDevice() const {
        return *mGraphicDevice;
    }
    
    GraphicFactory& AppContext::getGraphicFactory() const {
        return *mGraphicFactory;
    }
    
    WindowFactory& AppContext::getWindowFactory() const {
        return *mWindowFactory;
    }
    
    void AppContext::registerGraphicDevice(GraphicDevicePtr device) {
        mGraphicDevice = device;
    }
    
    void AppContext::registerGraphicFactory(GraphicFactoryPtr factory) {
        mGraphicFactory = factory;
    }
    
    void AppContext::registerWindowFactory(WindowFactoryPtr windowFactory) {
        mWindowFactory = windowFactory;
    }
    
    void AppContext::loadGraphicDevice(const ukn_string& name) {
        mGraphicDeviceLoader.close();
        
        if(mGraphicDeviceLoader.open(name.c_str())) {
            CreateGraphicDeviceFunc func = (CreateGraphicDeviceFunc)mGraphicDeviceLoader.getProc("CreateGraphicDevice");
            
            if(func) {
                func(mGraphicDevice);
                
                ukn_assert(mGraphicDevice.isValid());
            }
        }
    }
    
    void AppContext::loadGraphicFactory(const ukn_string& name) {
        mGraphicFactoryLoader.close();
        
        if(mGraphicFactoryLoader.open(name.c_str())) {
            CreateGraphicFactoryFunc func = (CreateGraphicFactoryFunc)mGraphicFactoryLoader.getProc("CreateGraphicDevice");
            
            if(func) {
                func(mGraphicFactory);
                
                ukn_assert(mGraphicFactory.isValid());
            }
        }
    }
    
    void AppContext::loadWindowFactory(const ukn_string& name) {
        mWindowFactoryLoader.close();
        
        if(mWindowFactoryLoader.open(name.c_str())) {
            CreateWindowFactoryFunc func = (CreateWindowFactoryFunc)mWindowFactoryLoader.getProc("CreateWindowFactory");
            
            if(func) {
                func(mWindowFactory);
                
                ukn_assert(mWindowFactory.isValid());
            }
        }
    }
    
} // namespace ukn