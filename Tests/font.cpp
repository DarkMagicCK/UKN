//
//  font.cpp
//  Project Unknown
//
//  Created by Robert Bu on 2/21/13.
//  Copyright (c) 2013 heizi. All rights reserved.
//

#include "UKN/UKN.h"

#include <vector>
#include <map>


#include <numeric>
#ifndef MIST_OS_WINDOWS

#include "../Plugins/gl/GLGraphicFactory.h"

int main (int argc, const char * argv[])
{
#else
#pragma comment(linker, "/NODEFAULTLIB:libcmt.lib")
    
#pragma comment(lib, "mist.lib")
#pragma comment(lib, "ukn_dll.lib")
    
int CALLBACK WinMain(__in  HINSTANCE hInstance,
                     __in  HINSTANCE hPrevInstance,
                     __in  LPSTR lpCmdLine,
                     __in  int nCmdSho) {
#endif
    
#ifndef MIST_OS_WINDOWS
    ukn::GraphicFactoryPtr factory;
    ukn::CreateGraphicFactory(factory);
    ukn::Context::Instance().registerGraphicFactory(factory);
#endif

    ukn::FontPtr font;
    
    ukn::AppLauncher(L"Windows Test")
    .create(
            ukn::ContextCfg::Default()
            .width(800)
            .height(600)
            .sampleCount(1)
            .showMouse(true)
            .isFullScreen(false)
            .graphicFactoryName(L"D3D11Plugin")
            .fps(60)
            )
    .connectUpdate([](ukn::Window* ) {
        
    })
    .connectRender([&](ukn::Window* ) {
        ukn::GraphicDevice& gd = ukn::Context::Instance().getGraphicFactory().getGraphicDevice();
        gd.clear(ukn::CM_Color, ukn::color::Black, 1.0f, 0.f);
        
        font->begin();
        font->draw(L"ABCDEFGHIJKLMNOPQRSTUVWXYZ", 0, 0, ukn::FA_Left);
        font->end();
        ukn::SpriteBatch& sb = ukn::SpriteBatch::DefaultObject();
        
        gd.clear(ukn::CM_Color, ukn::color::Black, 1.0f, 0.f);

        sb.begin();
        sb.draw(font->getTexturePlacement(0)->texture, ukn::Vector2(0, 0));
        sb.end();
        
    })
    .connectInit([&](ukn::Window* ) {
        font = ukn::Font::Create(L"Courier New.ttf", 20);
    })
    .run();
    
}
    
