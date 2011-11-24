//
//  PluginContents.h
//  Project Unknown
//
//  Created by Ruiwei Bu on 11/24/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_PluginContents_h
#define Project_Unknown_PluginContents_h

#include "Platform.h"
#include "GraphicSettings.h"
#include "PreDeclare.h"
#include "DllLoader.h"

namespace ukn {
        
    /**
     * Manages dynamic registered plugins
     * May load from dynamic librarys(dll, so, dylib)
     *  or register by hand
     **/
    
    struct AppContextCfg {
        ukn_string graphic_device_name;
        ukn_string graphic_factory_name;
        ukn_string window_factory_name;
        
        RenderSettings render_cfg;
    };
    
    class UKN_API AppContext {
    public:
        
        void            setCfg(AppContextCfg& cfg);
        AppContextCfg   getCfg() const;
        
        void            loadCfgFile(const ukn_string& name);
        void            saveCfgFile(const ukn_string& name);
        
        GraphicDevicePtr    getGraphicDevicePtr() const;
        GraphicFactoryPtr   getGraphicFactoryPtr() const;
        WindowFactoryPtr    getWindowFactoryPtr() const;
        
        GraphicDevice&      getGraphicDevice() const;
        GraphicFactory&     getGraphicFactory() const;
        WindowFactory&      getWindowFactory() const;
        
        void registerGraphicDevice(GraphicDevicePtr device);
        void registerGraphicFactory(GraphicFactoryPtr factory);
        void registerWindowFactory(WindowFactoryPtr windowFactory);
        
        void loadGraphicDevice(const ukn_string& name);
        void loadGraphicFactory(const ukn_string& name);
        void loadWindowFactory(const ukn_string& name);
        
        void setApp(AppPtr app);
        AppInstance& getAppInstance() const;
        
    protected:
        AppContext();
        ~AppContext();
        
        friend class AppInstance;
        
    private:
        AppPtr mApp;
        AppContextCfg mCfg;
        
        DllLoader mGraphicDeviceLoader;
        DllLoader mGraphicFactoryLoader;
        DllLoader mWindowFactoryLoader;
        
        GraphicDevicePtr mGraphicDevice;
        GraphicFactoryPtr mGraphicFactory;
        WindowFactoryPtr mWindowFactory;
    };
    
    
} // namespace ukn


#endif
