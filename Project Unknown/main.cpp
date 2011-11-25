//
//  main.cpp
//  Project Unknown
//
//  Created by Robert Bu on 11/19/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#include <iostream>


#include "../Plugins/GL/GLGraphicFactory.h"

#include "UKN/App.h"
#include "UKN/Context.h"

#include <vector>

int main (int argc, const char * argv[])
{
    // not building plugin as dynamic libraries
    // register it by hand
    // should be in dll and configured by config file
    ukn::GraphicFactoryPtr gl_factory;
    ukn::CreateGraphicFactory(gl_factory);
    
    ukn::Context::Instance().registerGraphicFactory(gl_factory);
    
    ukn::ContextCfg cfg;
    
    cfg.render_cfg.width        = 800;
    cfg.render_cfg.height       = 600;
    cfg.render_cfg.top          = 0;
    cfg.render_cfg.left         = 0;
    cfg.render_cfg.full_screen  = false;
    
    ukn::AppInstance instance("Test App");
    // init with custom cfg
    instance.init(cfg);
    
    // create app context
    instance.create();
    
    // run app
    instance.run();
    
    return 0;
}

