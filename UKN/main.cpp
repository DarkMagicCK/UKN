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
#include "UKN/Texture.h"
#include "UKN/Class.h"
#include "UKN/Event.h"
#include "UKN/RandomUtil.h"
#include "UKN/SpriteBatch.h"
#include "UKN/RenderBuffer.h"
#include "UKN/MathUtil.h"
#include "UKN/Font.h"
#include "UKN/Asset.h"
#include "UKN/ConfigParser.h"
#include "UKN/Resource.h"

#include <vector>

class MyApp: public ukn::AppInstance {
public:
    MyApp(const ukn::ukn_string& name):
    ukn::AppInstance(name) {
        
    }
    
    void onInit() {
        mSpriteBatch = ukn::Context::Instance().getGraphicFactory().createSpriteBatch();
        
        ukn::ConfigParserPtr cfg = ukn::MakeConfigParser(ukn::ResourceLoader::Instance().loadResource(L"asset.xml"));
        ukn::AssetManager::Instance().unserialize(cfg);
        
        mFont = ukn::AssetManager::Instance().load<ukn::Font>(L"liheipro");
        mTexture = ukn::AssetManager::Instance().load<ukn::Texture>(L"索拉");
    }
    
    void onUpdate() {
        
    }
    
    void onRender() {
        ukn::Context::Instance().getGraphicFactory().getGraphicDevice().getCurrFrameBuffer()->clear(ukn::CM_Color, ukn::ColorBlack, 0, 0);
        
        ukn::Timestamp time;
        
        mSpriteBatch->onRenderBegin();
        if(mTexture)
            mSpriteBatch->draw(mTexture, 0.f, 0.f);
        mSpriteBatch->render();
        mSpriteBatch->onRenderEnd();
        
        if(mFont) {
            mFont->onRenderBegin();
            mFont->draw(L"是的是的 Hello World!", 0, 0, ukn::FA_Left);

            mFont->render();
            mFont->onRenderEnd();
        }
    }
    
private:
    ukn::SharedPtr<ukn::SpriteBatch> mSpriteBatch;

    
    ukn::FontPtr mFont;
    
    ukn::TexturePtr mTexture;
};

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
    
    MyApp instance("Test App");

    // create app context
    instance.create(L"config.xml");
    
    
    ukn::FrameCounter::Instance().setDesiredFps(0);
    
    // run app
    instance.run();
    
    
    return 0;
}

