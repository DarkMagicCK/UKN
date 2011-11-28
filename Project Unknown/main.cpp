//
//  main.cpp
//  Project Unknown
//
//  Created by Robert Bu on 11/19/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#include <iostream>


#include "../Plugins/GL/GLGraphicFactory.h"

#include "UKN/App.h"
#include "UKN/Context.h"
#include "UKN/Window.h"
#include "UKN/TimeUtil.h"

#include <vector>

void update(ukn::Window& window) {
    printf("fps: %f\n", ukn::FrameCounter::Instance().getCurrentFps());
}
#ifndef UKN_OS_WINDOWS
int main (int argc, const char * argv[])
{
#else
int CALLBACK WinMain(
  __in  HINSTANCE hInstance,
  __in  HINSTANCE hPrevInstance,
  __in  LPSTR lpCmdLine,
  __in  int nCmdShow
) {
#endif
    // register plugins by hand for testing purpose
    ukn::GraphicFactoryPtr gl_factory;
    ukn::CreateGraphicFactory(gl_factory);
    
    ukn::Context::Instance().registerGraphicFactory(gl_factory);
    
    ukn::ContextCfg cfg;
    
    cfg.render_cfg.width        = 800;
    cfg.render_cfg.height       = 600;
    cfg.render_cfg.top          = 100;
    cfg.render_cfg.left         = 100;
    cfg.render_cfg.fsaa_samples = 0;
    cfg.render_cfg.show_mouse   = true;
    cfg.render_cfg.color_fmt    = ukn::EF_ARGB8;
    cfg.render_cfg.depth_stencil_fmt = ukn::EF_D16;   
    cfg.render_cfg.full_screen  = false;
	cfg.render_cfg.resizable = true;
    
    ukn::AppInstance instance("Test App");
    // init with custom cfg
    instance.init(cfg);
    
    // create app context
    instance.create();
    
    ukn::Connection updateConn = instance.getMainWindow().onUpdate().connect(update);
    
    // run app
    instance.run();
    
    return 0;
}

