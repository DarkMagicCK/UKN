//
//  Context.cpp
//  Project Unknown
//
//  Created by Robert Bu on 11/24/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#include "UKN/Context.h"
#include "UKN/GraphicDevice.h"
#include "UKN/GraphicFactory.h"
#include "UKN/Window.h"
#include "UKN/App.h"
#include "UKN/ConfigParser.h"
#include "UKN/Resource.h"
#include "UKN/Stream.h"
#include "UKN/Logger.h"
#include "UKN/Common.h"
#include "UKN/Scene.h"
#include "UKN/Asset.h"

namespace ukn {
    
    typedef void (*CreateGraphicFactoryFunc)(GraphicFactoryPtr&);
    
    Context& Context::Instance() {
        static Context static_instance;
        return static_instance;
    }
    
    Context::Context():
    mApp(0) {
        mGraphicFactory = GraphicFactory::NullObject();
    }
    
    Context::~Context() {
        mGraphicFactoryLoader.close();
    }
    
    void Context::setApp(AppInstance* app) {
        mApp = app;
    }
    
    AppInstance& Context::getApp() const {
        return *mApp;
    }
    
    void Context::setScene(const SceneManagerPtr& ptr) {
        mScene = ptr;
    }
    
    SceneManager& Context::getScene() {
        ukn_assert(mScene);
        return *mScene;
    }
    
    bool Context::isAppAvailable() const {
        return mApp != 0;
    }
    
    void Context::setCfg(const ContextCfg& cfg) {
        mCfg = cfg;
        
        if(!cfg.graphic_factory_name.empty()) 
            loadGraphicFactory(cfg.graphic_factory_name);
    }
    
    ContextCfg Context::getCfg() const {
        return mCfg;
    }
    
    inline int fmt_string_to_element_format(const ukn_string& str) {
        if(str == "EF_ARGB8")
            return EF_ARGB8;
        else if(str == "EF_D16")
            return EF_D16;
        else if(str == "EF_D24S8")
            return EF_D24S8;
        else if(str == "EF_D32")
            return EF_D32;
        return -1;
    }
    
    inline ukn_string element_format_to_string(ElementFormat fmt) {
        switch(fmt) {
            case EF_ARGB8:
                return "EF_ARGB8";
            case EF_D16:
                return "EF_D16";
            case EF_D32:
                return "EF_D32";
            case EF_D24S8:
                return "EF_D24S8";
        }
        return "unknown_fmt";
    }
    
    void Context::loadCfgFile(const String& name) {
        ContextCfg cfg;
        cfg.render_cfg.width = 800;
        cfg.render_cfg.height = 600;
        cfg.render_cfg.top = 0;
        cfg.render_cfg.left = 0;
        cfg.render_cfg.color_fmt = EF_ARGB8;
        cfg.render_cfg.depth_stencil_fmt = EF_D16;
        cfg.render_cfg.resizable = false;
        cfg.render_cfg.full_screen = false;
        cfg.render_cfg.show_mouse = true;
        cfg.render_cfg.sample_count = 0;
        cfg.render_cfg.sample_quality = 0;
        cfg.render_cfg.fsaa_samples = 0;
        
        ConfigParserPtr configParser = AssetManager::Instance().load<ConfigParser>(name);
        if(configParser) {            
            if(configParser->toNode("/cfg/graphics")) {
                cfg.render_cfg.width        = configParser->getInt("width", cfg.render_cfg.width);
                cfg.render_cfg.height       = configParser->getInt("height", cfg.render_cfg.height);
                cfg.render_cfg.top          = configParser->getInt("top", cfg.render_cfg.top);
                cfg.render_cfg.left         = configParser->getInt("left", cfg.render_cfg.left);
                
                cfg.render_cfg.resizable    = configParser->getBool("resizable", cfg.render_cfg.resizable);
                cfg.render_cfg.show_mouse   = configParser->getBool("show_mouse", cfg.render_cfg.show_mouse);
                cfg.render_cfg.full_screen  = configParser->getBool("full_screen", cfg.render_cfg.full_screen);
                
                cfg.render_cfg.sample_count     = configParser->getInt("sample_count", cfg.render_cfg.sample_count);
                cfg.render_cfg.sample_quality   = configParser->getInt("sample_quality", cfg.render_cfg.sample_quality);
                cfg.render_cfg.fsaa_samples     = configParser->getInt("fsaa_samples", cfg.render_cfg.fsaa_samples);
                
                ukn_string fmt_string;
                int fmt_id;
                
                fmt_string = configParser->getString("color_fmt", "EF_ARGB8");
                fmt_id = fmt_string_to_element_format(fmt_string);
                if(fmt_id == -1) {
                    log_warning(format_string("ukn::Context::loadCfgFile: unknown color format %s, using EF_ARGB8", fmt_string.c_str()));
                    
                    cfg.render_cfg.color_fmt = (ElementFormat)fmt_id;
                }
                
                fmt_string = configParser->getString("depth_stencil_fmt", "EF_D16");
                fmt_id = fmt_string_to_element_format(fmt_string);
                if(fmt_id == -1) {
                    log_warning(format_string("ukn::Context::loadCfgFile: unknown depth stencil format %s, using EF_D16", fmt_string.c_str()));
                    
                    cfg.render_cfg.depth_stencil_fmt = (ElementFormat)fmt_id;
                }
            }
            
            if(configParser->toNode("/cfg/plugins")) {
                cfg.graphic_factory_name = configParser->getString("graphic_factory", "");
            }
        } else {
            log_warning(L"ukn::Context::loadCfgFile: unable to open config file "+configParser->getName());
        }
        
        setCfg(cfg);

		
		if(!configParser) {
			saveCfgFile(L"config.xml");
		}
    }
    
    void Context::saveCfgFile(const String& name) {
        ConfigParserPtr configParser = ConfigParser::MakeEmptyParser(CPT_XML);
        if(configParser) {
            configParser->beginNode("cfg");
            configParser->beginNode("graphics");
            configParser->setInt("width", mCfg.render_cfg.width);
            configParser->setInt("height", mCfg.render_cfg.height);
            configParser->setInt("top", mCfg.render_cfg.top);
            configParser->setInt("left", mCfg.render_cfg.left);
            configParser->setBool("show_mouse", mCfg.render_cfg.show_mouse);
            configParser->setBool("full_screen", mCfg.render_cfg.full_screen);
            configParser->setBool("resizable", mCfg.render_cfg.resizable);
            configParser->setInt("sample_count", mCfg.render_cfg.sample_count);
            configParser->setInt("sample_quality", mCfg.render_cfg.sample_quality);
            configParser->setInt("fsaa_samples", mCfg.render_cfg.fsaa_samples);
            configParser->setString("color_fmt", element_format_to_string(mCfg.render_cfg.color_fmt));
            configParser->setString("depth_stencil_fmt", element_format_to_string(mCfg.render_cfg.depth_stencil_fmt));
            
            configParser->toNode("/cfg");
            configParser->beginNode("plugins");
            if(!mCfg.graphic_factory_name.empty()) 
                configParser->setString("graphic_factory", mCfg.graphic_factory_name);
        }
        
        ukn_string formattedString = configParser->writeToString();
        ResourcePtr resource = ResourceLoader::Instance().createFileResource(name);
        if(resource) {
            resource->getResourceStream()->write((const uint8*)formattedString.data(), 
                                                 formattedString.size());
        }
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