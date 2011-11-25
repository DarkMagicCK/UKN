//
//  Context.cpp
//  Project Unknown
//
//  Created by Ruiwei Bu on 11/24/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#include "UKN/Context.h"
#include "UKN/GraphicDevice.h"
#include "UKN/GraphicFactory.h"
#include "UKN/Window.h"
#include "UKN/App.h"

namespace ukn {
    
    typedef void (*CreateGraphicFactoryFunc)(GraphicFactoryPtr&);
    
    Context& Context::Instance() {
        static Context static_instance;
        return static_instance;
    }
    
    Context::Context() {
        mGraphicFactory = GraphicFactory::NullObject();
    }
    
    Context::~Context() {
        mGraphicFactoryLoader.close();
    }
    
    void Context::setApp(AppPtr app) {
        mApp = app;
    }
    
    AppInstance& Context::getAppInstance() const {
        return *mApp;
    }
    
    void Context::setCfg(const ContextCfg& cfg) {
        mCfg = cfg;
        
        if(!cfg.graphic_factory_name.empty()) 
            loadGraphicFactory(cfg.graphic_factory_name);
    }
    
    ContextCfg Context::getCfg() const {
        return mCfg;
    }
    
    void Context::loadCfgFile(const ukn_string& name) {
        // todo
    }
    
    void Context::saveCfgFile(const ukn_string& name) {
        // todo
    }
 
    GraphicFactoryPtr Context::getGraphicFactoryPtr() const {
        return mGraphicFactory;
    }
    
    GraphicFactory& Context::getGraphicFactory() const {
        return *mGraphicFactory;
    }
    
    void Context::registerGraphicFactory(GraphicFactoryPtr factory) {
        mGraphicFactory = factory;
    }
    
    void Context::loadGraphicFactory(const ukn_string& name) {
        mGraphicFactoryLoader.close();
        
        if(mGraphicFactoryLoader.open(name.c_str())) {
            CreateGraphicFactoryFunc func = (CreateGraphicFactoryFunc)mGraphicFactoryLoader.getProc("CreateGraphicFactory");
            
            if(func) {
                func(mGraphicFactory);
                
                ukn_assert(mGraphicFactory.isValid());
            }
        }
    }
    
} // namespace ukn