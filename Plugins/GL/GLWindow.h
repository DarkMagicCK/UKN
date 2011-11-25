//
//  GLWindow.h
//  Project Unknown
//
//  Created by Ruiwei Bu on 11/24/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_GLWindow_h
#define Project_Unknown_GLWindow_h

#include "UKN/Platform.h"
#include "UKN/Window.h"
#include "UKN/Signal.h"

namespace ukn {
    
    class GLWindow: public Window {
    public:
        GLWindow(const ukn_string& name, const RenderSettings& settings);
        ~GLWindow();
        
        const ukn_wstring& description() const;
        
        void onPullEvents(const Window& wnd);
        void onSwapBuffers(const Window& wnd);
        
    private:
        void* mGlfwWindow;
        
        Connection mPollEventConn;
        Connection mSwapBuffersConn;
    };
    
} // namespace ukn

#endif
