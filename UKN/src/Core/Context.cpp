//
//  Context.cpp
//  Project Unknown
//
//  Created by Robert Bu on 11/24/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//


#include "mist/ConfigParser.h"
#include "mist/Resource.h"
#include "mist/Stream.h"
#include "mist/Logger.h"
#include "mist/Common.h"

#include "UKN/Context.h"
#include "UKN/GraphicDevice.h"
#include "UKN/GraphicFactory.h"
#include "UKN/Window.h"
#include "UKN/App.h"
#include "UKN/Asset.h"

namespace ukn {
    
    ContextCfg& ContextCfg::graphicFactoryName(const UknString& name) {
        graphic_factory_name = name;
        return *this;
    }
    
    ContextCfg& ContextCfg::isFullScreen(bool flag) {
        render_cfg.full_screen = flag;
        return *this;
    }
    
    ContextCfg& ContextCfg::width(uint32 width) {
        render_cfg.width = width;
        return *this;
    }
    
    ContextCfg& ContextCfg::height(uint32 height) {
        render_cfg.height = height;
        return *this;
    }
    
    ContextCfg& ContextCfg::colorFormat(ElementFormat fmt) {
        render_cfg.color_fmt = fmt;
        return *this;
    }
    
    ContextCfg& ContextCfg::depthStencilFormat(ElementFormat fmt) {
        render_cfg.depth_stencil_fmt = fmt;
        return *this;
    }
    
    ContextCfg& ContextCfg::resizable(bool flag) {
        render_cfg.resizable = flag;
        return *this;
    }
    
    ContextCfg& ContextCfg::showMouse(bool flag) {
        render_cfg.show_mouse = flag;
        return *this;
    }
    
    ContextCfg& ContextCfg::sampleCount(int count) {
        render_cfg.sample_count = count;
        return *this;
    }
    
    ContextCfg& ContextCfg::sampleQuality(int quality) {
        render_cfg.sample_quality = quality;
        return *this;
    }
    
    
	ContextCfg& ContextCfg::icon(const MistString& icon) {
		render_cfg.icon = icon;
		return *this;
	}

	ContextCfg& ContextCfg::enableVSync(bool flag) {
		render_cfg.vsync = flag;
		return *this;
	}

	ContextCfg& ContextCfg::cursor(const MistString& cursor) {
		render_cfg.cursor = cursor;
		return *this;
	}

    ContextCfg& ContextCfg::fps(uint32 fps) {
        desired_fps = fps;
        return *this;
    }

    ContextCfg ContextCfg::Default() {
        return ContextCfg()
                    .width(640)
                    .height(480)
                    .isFullScreen(false)
                    .resizable(false)
                    .showMouse(true)
                    .colorFormat(EF_RGBA8)
                    .depthStencilFormat(EF_D16);
    }
    
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
    
    void Context::setApp(AppLauncher* app) {
        mApp = app;
    }
    
    AppLauncher& Context::getApp() const {
        return *mApp;
    }
    
    void Context::setSceneManager(const SceneManagerPtr& ptr) {
        mScene = ptr;
    }
    
    SceneManager& Context::getSceneManager() {
        mist_assert(mScene);
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
    
    inline int fmt_string_to_element_format(const UknString& str) {
        if(str == L"EF_RGBA8")
            return EF_RGBA8;
        if(str == L"EF_D16")
            return EF_D16;
        if(str == L"EF_D24S8")
            return EF_D24S8;
        if(str == L"EF_D32")
            return EF_D32;
        if(str == L"EF_RGB565")
            return EF_RGB565;
        if(str == L"EF_RGB5A1")
            return EF_RGB5A1;
        if(str == L"EF_RGBA4444")
            return EF_RGBA4444;
        return -1;
    }
    
    inline UknString element_format_to_string(ElementFormat fmt) {
        switch(fmt) {
            case EF_RGBA8:
                return L"EF_RGBA8";
            case EF_D16:
                return L"EF_D16";
            case EF_D32:
                return L"EF_D32";
            case EF_D24S8:
                return L"EF_D24S8";
            case EF_RGB565:
                return L"EF_RGB565";
            case EF_RGB5A1:
                return L"EF_RGB5A1";
            case EF_RGBA4444:
                return L"EF_RGBA4444";
            case EF_Float4:
                return L"EF_Float4";
            case EF_Float:
                return L"EF_Float";
            case EF_Float2:
                return L"EF_Float2";
            case EF_Float3:
                return L"EF_Float3";
            case EF_UInt32:
                return L"EF_UInt32";
            default:
                return L"unknown";
        }
        return L"unknown";
    }
    
    void Context::loadCfgFile(const UknString& name) {
        ContextCfg cfg;
        cfg.render_cfg.width = 800;
        cfg.render_cfg.height = 600;
        cfg.render_cfg.top = 0;
        cfg.render_cfg.left = 0;
        cfg.render_cfg.color_fmt = EF_RGBA8;
        cfg.render_cfg.depth_stencil_fmt = EF_D16;
        cfg.render_cfg.resizable = false;
        cfg.render_cfg.full_screen = false;
        cfg.render_cfg.show_mouse = true;
        cfg.render_cfg.sample_count = 0;
        cfg.render_cfg.sample_quality = 0;
        
        ConfigParserPtr configParser = AssetManager::Instance().load<ConfigParser>(name);
        if(configParser) {            
            if(configParser->toNode(L"/cfg/graphics")) {
                cfg.render_cfg.width        = configParser->getInt(L"width", cfg.render_cfg.width);
                cfg.render_cfg.height       = configParser->getInt(L"height", cfg.render_cfg.height);
                cfg.render_cfg.top          = configParser->getInt(L"top", cfg.render_cfg.top);
                cfg.render_cfg.left         = configParser->getInt(L"left", cfg.render_cfg.left);
                
                cfg.render_cfg.resizable    = configParser->getBool(L"resizable", cfg.render_cfg.resizable);
                cfg.render_cfg.show_mouse   = configParser->getBool(L"show_mouse", cfg.render_cfg.show_mouse);
                cfg.render_cfg.full_screen  = configParser->getBool(L"full_screen", cfg.render_cfg.full_screen);
                
                cfg.render_cfg.sample_count     = configParser->getInt(L"sample_count", cfg.render_cfg.sample_count);
                cfg.render_cfg.sample_quality   = configParser->getInt(L"sample_quality", cfg.render_cfg.sample_quality);
                
                UknString fmt_string;
                int fmt_id;
                
                fmt_string = configParser->getString(L"color_fmt", L"EF_ARGB8");
                fmt_id = fmt_string_to_element_format(fmt_string);
                if(fmt_id == -1) {
                    log_warning(format_string("ukn::Context::loadCfgFile: unknown color format %s, using EF_ARGB8", fmt_string.c_str()));
                    
                    cfg.render_cfg.color_fmt = (ElementFormat)fmt_id;
                }
                
                fmt_string = configParser->getString(L"depth_stencil_fmt", L"EF_D16");
                fmt_id = fmt_string_to_element_format(fmt_string);
                if(fmt_id == -1) {
                    log_warning(format_string("ukn::Context::loadCfgFile: unknown depth stencil format %s, using EF_D16", fmt_string.c_str()));
                    
                    cfg.render_cfg.depth_stencil_fmt = (ElementFormat)fmt_id;
                }
            }
            
            if(configParser->toNode(L"/cfg/plugins")) {
                cfg.graphic_factory_name = configParser->getString(L"graphic_factory", L"");
            }
        } else {
            log_warning(L"ukn::Context::loadCfgFile: unable to open config file " + name);
        }
        
        setCfg(cfg);

		
		if(!configParser) {
			saveCfgFile(L"config.xml");
		}
    }
    
    void Context::saveCfgFile(const UknString& name) {
        ConfigParserPtr configParser = ConfigParser::MakeEmptyParser(CPT_XML);
        if(configParser) {
            configParser->beginNode(L"cfg");
            configParser->beginNode(L"graphics");
            configParser->setInt(L"width", mCfg.render_cfg.width);
            configParser->setInt(L"height", mCfg.render_cfg.height);
            configParser->setInt(L"top", mCfg.render_cfg.top);
            configParser->setInt(L"left", mCfg.render_cfg.left);
            configParser->setBool(L"show_mouse", mCfg.render_cfg.show_mouse);
            configParser->setBool(L"full_screen", mCfg.render_cfg.full_screen);
            configParser->setBool(L"resizable", mCfg.render_cfg.resizable);
            configParser->setInt(L"sample_count", mCfg.render_cfg.sample_count);
            configParser->setInt(L"sample_quality", mCfg.render_cfg.sample_quality);
            configParser->setString(L"color_fmt", element_format_to_string(mCfg.render_cfg.color_fmt));
            configParser->setString(L"depth_stencil_fmt", element_format_to_string(mCfg.render_cfg.depth_stencil_fmt));
            
            configParser->toNode(L"/cfg");
            configParser->beginNode(L"plugins");
            if(!mCfg.graphic_factory_name.empty()) 
                configParser->setString(L"graphic_factory", mCfg.graphic_factory_name);
        }
        
        UknString formattedString = configParser->writeToString();
        ResourcePtr resource = ResourceLoader::Instance().createFileResource(name);
        if(resource) {
            resource->getResourceStream()->write((const uint8*)formattedString.data(), 
                                                 formattedString.size());
        }
    }
    
    GraphicFactory& Context::getGraphicFactory() const {
        mist_assert(mGraphicFactory.get());
        return *mGraphicFactory;
    }
    
    void Context::registerGraphicFactory(GraphicFactoryPtr factory) {
        mGraphicFactory = factory;
    }
    
    void Context::loadGraphicFactory(const UknString& name) {
        mGraphicFactoryLoader.close();
        
        if(mGraphicFactoryLoader.open(string::WStringToString(name).c_str())) {
            CreateGraphicFactoryFunc func = (CreateGraphicFactoryFunc)mGraphicFactoryLoader.getProc("CreateGraphicFactory");
            
            if(func) {
                func(mGraphicFactory);
                
                mist_assert(mGraphicFactory.isValid());
            }
        }
    }

    void Context::terminate() {
        mGraphicFactory.deref();
    }
       
} // namespace ukn