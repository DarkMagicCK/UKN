//
//  font.cpp
//  Project Unknown
//
//  Created by Robert Bu on 2/21/13.
//  Copyright (c) 2013 heizi. All rights reserved.
//

#include "UKN/App.h"
#include "UKN/Context.h"
#include "UKN/Window.h"
#include "UKN/Texture.h"
#include "UKN/GraphicDevice.h"
#include "UKN/GraphicBuffer.h"
#include "UKN/GraphicFactory.h"
#include "UKN/FrameBuffer.h"
#include "UKN/Texture.h"
#include "UKN/SpriteBatch.h"
#include "UKN/RenderBuffer.h"
#include "UKN/Font.h"
#include "UKN/Asset.h"
#include "UKN/Skeletal.h"
#include "UKN/Input.h"
#include "UKN/Shader.h"
#include "UKN/Model.h"
#include "UKN/RenderTarget.h"
#include "UKN/CameraController.h"
#include "UKN/Skybox.h"
#include "UKN/Terrian.h"
#include "UKN/GraphicContext.h"

#include "mist/Convert.h"

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
            .graphicFactoryName(L"D3D11Plugin.dll")
            .fps(60)
            )
    .connectUpdate([](ukn::Window* ) {
        
    })
    .connectRender([&](ukn::Window* ) {
        ukn::GraphicDevice& gd = ukn::Context::Instance().getGraphicFactory().getGraphicDevice();
        gd.clear(ukn::CM_Color, ukn::color::Black, 1.0f, 0.f);
        
        font->begin();
        font->draw(L"ABCSDAKDadSKDJADLKSAJDKASDJALSDJAKSDJDEFGHIJKLMNOPQRSTUVWRST", 0, 0, ukn::FA_Left);
        font->end();
        ukn::SpriteBatch& sb = ukn::SpriteBatch::DefaultObject();

        sb.begin();
        sb.draw(font->getTexturePlacement(0)->texture, ukn::Vector2(0, 0));
        sb.end();
        
    })
    .connectInit([&](ukn::Window* ) {
        font = ukn::Font::Create(L"Arial.ttf", 20);

        mist::log_info((mist::FormatString(L"{0}, {1} {2}, {1} test"), 1, 0.2f, "test"));

        ukn::ModelLoader::ModelDataPtr model = ukn::ModelLoader::LoadFromPly(
            ukn::ResourceLoader::Instance().loadResource(L"dragon_vrip_res4.ply"),
                                                                             true
        );
    })
    .run();
    
}
    
