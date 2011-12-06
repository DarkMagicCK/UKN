//
//  App.h
//  Project Unknown
//
//  Created by Robert Bu on 11/24/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_App_h
#define Project_Unknown_App_h

#include "UKN/Platform.h"
#include "UKN/PreDeclare.h"
#include "UKN/Uncopyable.h"
#include "UKN/Signal.h"
#include "UKN/GraphicSettings.h"

namespace ukn {
    
    // Game App Luncher
    
    class AppInstance: public Uncopyable {
    public:
        AppInstance(const ukn_string& name);
        ~AppInstance();
        
        Window&     getMainWindow() const;
		WindowPtr   getMainWindowPtr() const;
        
        void create(const ukn_wstring& cfgname=L"config.xml");
        void create(const ContextCfg& cfg);
        
        void run();
        void terminate();
        
        void update();
        void render();
        
        void        setCamera(CameraPtr camera);
        CameraPtr   getCamera() const;
        
    protected:
        virtual void onRender();
        virtual void onUpdate();
        virtual void onInit();
        
    private:
        void doCreate();
        void onWindowClose(Window& wnd);
        
        ukn_string mName;
        bool mInited;    

        WindowPtr mMainWindow;
        CameraPtr mCamera;
        
        Connection mCloseConn;
    };
    
} // namespace ukn

#endif
