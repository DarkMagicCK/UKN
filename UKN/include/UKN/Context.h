//
//  PluginContents.h
//  Project Unknown
//
//  Created by Robert Bu on 11/24/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_PluginContents_h
#define Project_Unknown_PluginContents_h

#include "UKN/Platform.h"
#include "UKN/GraphicSettings.h"
#include "UKN/PreDeclare.h"
#include "UKN/DllLoader.h"
#include "UKN/StringUtil.h"

namespace ukn {
        
    /**
     * Manages dynamic registered plugins
     * May load from dynamic librarys(dll, so, dylib)
     *  or register by hand
     **/
    
    struct ContextCfg {
        ukn_string graphic_factory_name;
        
        RenderSettings render_cfg;
    };
    
    class UKN_API Context {
    public:
        static Context& Instance();
        
        void        setCfg(const ContextCfg& cfg);
        ContextCfg  getCfg() const;
        
        void loadCfgFile(const String& name);
        void saveCfgFile(const String& name);
        
        GraphicFactoryPtr getGraphicFactoryPtr() const;
        GraphicFactory&   getGraphicFactory() const;
        
        void setScene(const SceneManagerPtr& ptr);
        SceneManager& getScene();
        
        void registerGraphicFactory(GraphicFactoryPtr factory);
        
        void loadGraphicFactory(const ukn_string& name);
        
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
