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
#include "UKN/Profiler.h"
#include "UKN/Thread.h"
#include "UKN/Animation.h"
#include "UKN/Class.h"
#include "UKN/Skeletal.h"
#include "UKN/Base64.h"
#include "UKN/RandomUtil.h"
#include "UKN/TMXTiledMap.h"
#include "UKN/ZipUtil.h"

#include <vector>
#include <map>

int x =0 , y=0;

class MyApp: public ukn::AppInstance {
public:
    MyApp(const ukn::ukn_string& name):
    ukn::AppInstance(name) {
        
    }
    
    void onInit() {
        mSpriteBatch = ukn::Context::Instance().getGraphicFactory().createSpriteBatch();
        
        ukn::ConfigParserPtr cfg = ukn::AssetManager::Instance().load<ukn::ConfigParser>(L"asset.xml");
        ukn::AssetManager::Instance().deserialize(cfg);
        
        mFont = ukn::AssetManager::Instance().load<ukn::Font>(L"liheipro");
        mTexture = ukn::AssetManager::Instance().load<ukn::Texture>(L"索拉");
        
        ukn::ConfigParserPtr cfg2 = ukn::AssetManager::Instance().load<ukn::ConfigParser>(L"text/girl.uknanm");
		if(cfg2) {
			skAnim.deserialize(cfg2);
        
			skAnim.play("NewAnim");
			skAnim.setPosition(ukn::Vector2(300, 200));
		}
        
        ukn::ConfigParserPtr cfg3 = ukn::AssetManager::Instance().load<ukn::ConfigParser>(L"perspective_walls.tmx");

        if(cfg3) {
            mMap = new ukn::tmx::Map();
            mMap->deserialize(cfg3);
        
            mMap->setMapViewSize(ukn::Vector2(this->getMainWindow().getWidth() / mMap->getTileWidth() + 1,
                                              this->getMainWindow().getHeight() / mMap->getTileHeight()+ 1));
        } else 
            mMap = 0;
    }
    
    void onUpdate() {
        x++;
        y++;
     //   mMap->setMapPosition(ukn::Vector2(x, 0));
    }
    
    void onRender() {
        ukn::Context::Instance().getGraphicFactory().getGraphicDevice().clear(ukn::CM_Color | ukn::CM_Depth, ukn::color::Lightskyblue, 0, 0);
        
        if(mMap) 
            mMap->render();

        mSpriteBatch->begin(ukn::SBS_BackToFront);
        {
            UKN_PROFILE("sk_anim");
            skAnim.update();
            skAnim.render(*mSpriteBatch.get());

            ukn::ProfileData data = ukn::Profiler::Instance().get("sk_anim");
            

            printf("%s\n", data.toFormattedString().c_str());
        }       
        mSpriteBatch->end();
        
        
        if(mFont) {
            mFont->draw("HAPPY NEW YEAR", 0, 0, ukn::FA_Left, ukn::color::Black);

            mFont->render();
        }
    }
    
private:
    ukn::SharedPtr<ukn::SpriteBatch> mSpriteBatch;

    ukn::tmx::Map* mMap;
    
    ukn::SkeletalAnimation skAnim;
    ukn::FontPtr mFont;
    
    ukn::TexturePtr mTexture;
    ukn::StoryBoard mAnimation;
    int x, y;
};

#ifndef UKN_OS_WINDOWS
int main (int argc, const char * argv[])
{
#else
#pragma comment(linker, "/NODEFAULTLIB:libcmt.lib")

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
    
    
    ukn::FrameCounter::Instance().setDesiredFps(60);
    
    // run app
    instance.run();
    
    
    return 0;
}

