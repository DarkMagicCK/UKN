//
//  Context.h
//  Project Unknown
//
//  Created by Robert Bu on 11/24/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_Contexts_h
#define Project_Unknown_Contexts_h

#include "mist/Platform.h"
#include "mist/DllLoader.h"
#include "mist/StringUtil.h"
#include "mist/Thread.h"

#include "UKN/GraphicSettings.h"
#include "UKN/PreDeclare.h"

namespace ukn {
        
    /**
     * Manages dynamic registered plugins
     * May load from dynamic librarys(dll, so, dylib)
     *  or register manually
     **/
    
    struct UKN_API ContextCfg {
        UknString graphic_factory_name;
        
        RenderSettings render_cfg;
        
        ContextCfg& graphicFactoryName(const UknString& name);
        ContextCfg& isFullScreen(bool flag);
        ContextCfg& width(uint32 width);
        ContextCfg& height(uint32 height);
        ContextCfg& colorFormat(ElementFormat fmt);
        ContextCfg& depthStencilFormat(ElementFormat fmt);
        ContextCfg& resizable(bool flag);
        ContextCfg& showMouse(bool flag);
		ContextCfg& enableVSync(bool flag);
        ContextCfg& sampleCount(int count);
        ContextCfg& sampleQuality(int quality);
		ContextCfg& icon(const MistString& icon);
		ContextCfg& cursor(const MistString& cursor);
        
        static ContextCfg Default();
    };
    
    class UKN_API Context {
    public:
        static Context& Instance();
        
        void        setCfg(const ContextCfg& cfg);
        ContextCfg  getCfg() const;
        
        void loadCfgFile(const UknString& name);
        void saveCfgFile(const UknString& name);
        
        GraphicFactory&   getGraphicFactory() const;
        
        void setSceneManager(const SceneManagerPtr& ptr);
        SceneManager& getSceneManager();
        
        void registerGraphicFactory(GraphicFactoryPtr factory);
        
        void loadGraphicFactory(const UknString& name);
        
        void setApp(AppInstance* app);
        bool isAppAvailable() const;
        AppInstance& getApp() const;
        
    protected:
        Context();
        ~Context();
        
        friend class AppInstance;
        
    private:
        AppInstance* mApp;
        ContextCfg mCfg;
        
        DllLoader mGraphicFactoryLoader;
        
        GraphicFactoryPtr mGraphicFactory;
        
        SceneManagerPtr mScene;
    };
    
    
} // namespace ukn


#endif
