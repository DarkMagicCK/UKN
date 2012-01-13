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
#include "UKN/ZipUtil.h"

#include <vector>
#include <map>

class MyApp: public ukn::AppInstance {
public:
    MyApp(const ukn::ukn_string& name):
    ukn::AppInstance(name) {
        
    }
    
    void onInit() {
        mSpriteBatch = ukn::Context::Instance().getGraphicFactory().createSpriteBatch();
        
        ukn::ConfigParserPtr cfg = ukn::MakeConfigParser(ukn::ResourceLoader::Instance().loadResource(L"asset.xml"));
        ukn::AssetManager::Instance().deserialize(cfg);
        
        mFont = ukn::AssetManager::Instance().load<ukn::Font>(L"liheipro");
        mTexture = ukn::AssetManager::Instance().load<ukn::Texture>(L"索拉");
        
        ukn::ConfigParserPtr cfg2 = ukn::MakeConfigParser(ukn::ResourceLoader::Instance().loadResource(L"text/girl.uknanm"));
		if(cfg2) {
			skAnim.deserialize(cfg2);
        
			skAnim.play("NewAnim");
			skAnim.setPosition(ukn::Vector2(300, 200));
		}
    }
    
    void onUpdate() {
    }
    
    void onRender() {
        ukn::Context::Instance().getGraphicFactory().getGraphicDevice().clear(ukn::CM_Color | ukn::CM_Depth, ukn::color::Lightskyblue, 0, 0);
        
        
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
            mFont->draw("Hello World! 测试 ", 0, 0, ukn::FA_Left, ukn::color::Black);

            mFont->render();
        }
        

    }
    
private:
    ukn::SharedPtr<ukn::SpriteBatch> mSpriteBatch;

    
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
    ukn_logged_assert(1 == 0, "just a test");

    // register plugins by hand for testing purpose
    ukn::GraphicFactoryPtr gl_factory;
    ukn::CreateGraphicFactory(gl_factory);
    
    ukn::Array<ukn::uint8> data(ukn::base64_decode("eJzt1jsKgDAQQEELtVKvYuGvEDvvfya3MBeQQAzMwCPdsum2af5lL71AJv7xL7X+Y46WtzXayq7z2RGd0f2+V6a5bdRFfaZ5pQzRGE2lFwEAoArpDk7Veg+nOzjlHgYAAAAAIIcHvboDlQ=="));
    ukn::log_info((const char*)data.begin());
    ukn::Array<ukn::uint8> cpdata = ukn::zlib_compress(data.begin(), data.size());
    ukn::Array<ukn::uint8> dedata = ukn::zlib_decompress((ukn::uint8*)cpdata.begin(), cpdata.size());
    
    ukn::log_info((const char*)dedata.begin());
    
    ukn::Context::Instance().registerGraphicFactory(gl_factory);
    
    MyApp instance("Test App");

    // create app context
    instance.create(L"config.xml");
    
    
    ukn::FrameCounter::Instance().setDesiredFps(60);
    
    // run app
    instance.run();
    
    
    return 0;
}

