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
#include "UKN/Stream.h"
#include "UKN/SysUtil.h"
#include "UKN/Ptr.h"
#include "UKN/Texture.h"
#include "UKN/GraphicDevice.h"
#include "UKN/FrameBuffer.h"
#include "UKN/Logger.h"
#include "UKN/Common.h"

#include <vector>

void update(ukn::Window& window) {
    ukn::log_warning(ukn::format_string("fps: %f\n", ukn::FrameCounter::Instance().getCurrentFps()));
    
    ukn::Context::Instance().getGraphicFactory().getGraphicDevice().getCurrFrameBuffer()->clear(ukn::CM_Color, ukn::ColorRed, 0, 0);

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
    
    ukn::AppInstance instance("Test App");
    // init with custom cfg
    instance.init(L"config.xml");
        
    // create app context
    instance.create();
    
    instance.getMainWindow().onUpdate().connect(update);
    
    ukn::FrameCounter::Instance().setDesiredFps(0);
    
    // run app
    instance.run();
    
    
    return 0;
}

