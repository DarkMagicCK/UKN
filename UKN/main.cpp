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
#include "UKN/Skeletal.h"
#include "UKN/Base64.h"
#include "UKN/RandomUtil.h"
#include "UKN/ZipUtil.h"
#include "UKN/StreamWrapper.h"
#include "UKN/Operations.h"

#include "UKN/tmx/TMXTiledMap.h"

#include "UKN/reflection/TypeDatabase.h"


#include <vector>
#include <map>

class MyApp: public ukn::AppInstance {
public:
    MyApp(const ukn::ukn_string& name):
    ukn::AppInstance(name) {
        
    }
    
    ukn::Rectangle viewRect;
    
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
        
        ukn::ConfigParserPtr cfg3 = ukn::AssetManager::Instance().load<ukn::ConfigParser>(L"isometric_grass_and_water.tmx");
        
        viewRect = ukn::Rectangle(0,
                                  0,
                                  getWindow().width(),
                                  getWindow().height());

        if(cfg3) {
            mMap = new ukn::tmx::Map();
            mMap->deserialize(cfg3);
            
         //  mMap->setMapViewRect(ukn::Rectangle(0, 0, 1280, 300));
        } else 
            mMap = 0;
    }
    
    void onUpdate() {
        if(getWindow().isKeyDown(ukn::input::Key::Left)) 
            viewRect.x1 -= 1.f;
        if(getWindow().isKeyDown(ukn::input::Key::Right)) 
            viewRect.x1 += 1.f;
        if(getWindow().isKeyDown(ukn::input::Key::Up)) 
            viewRect.y1 -= 1.f;
        if(getWindow().isKeyDown(ukn::input::Key::Down)) 
            viewRect.y1 += 1.f;
        if(getWindow().isKeyDown(ukn::input::Key::W)) 
            viewRect.y2 -= 1.f;
        if(getWindow().isKeyDown(ukn::input::Key::S)) 
            viewRect.y2 += 1.f;
        if(getWindow().isKeyDown(ukn::input::Key::A)) 
            viewRect.x2 -= 1.f;
        if(getWindow().isKeyDown(ukn::input::Key::D)) 
            viewRect.x2 += 1.f;
        
        
        mMap->setMapViewRect(viewRect);
       // mMap->setPosition(ukn::Vector2(viewRect.x1, viewRect.y1));
    }
    
    void onRender() {
        ukn::Context::Instance().getGraphicFactory().getGraphicDevice().clear(ukn::CM_Color | ukn::CM_Depth, ukn::color::Lightskyblue, 0, 0);
        
        mSpriteBatch->begin(ukn::SBS_BackToFront);
        {
            UKN_PROFILE("sk_anim");
            skAnim.update();
            skAnim.render(*mSpriteBatch.get());
            
            if(mMap) 
                mMap->render();

            ukn::ProfileData data = ukn::Profiler::Instance().get("sk_anim");
            

//            printf("%s\n", data.toFormattedString().c_str());
        }       
        mSpriteBatch->end();
        
        
        if(mFont) {
            mFont->draw("咿呀咿呀哟哦哦哦", 0, 0, ukn::FA_Left, ukn::color::Black);
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

class myClass {
public:
    int test[5];
    
    myClass() {
        for(int i=0; i<5; ++i)
            test[i] = i;
    }
};

namespace ukn {
    
    namespace reflection {
        
        template<>
        struct TypeNameRetriever<myClass> { static const char* Name() { return "myClass"; } };
        
    }
    
}

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
        
    ukn::reflection::TypeDB& db = ukn::reflection::TypeDB::Instance();
    UKN_ENUMERABLE_FOREACH(const ukn::reflection::Type&, t, db) {
        printf("%s\n", t.name.text);
    }
 
    ukn::reflection::Type* type = db.getType("vector2");
    if(type != 0) {
        const ukn::reflection::Field* x = type->getField("x");
        const ukn::reflection::Field* y = type->getField("y");
        
        ukn_assert(x != 0 && y != 0);
        
        printf("field name = %s, type = %s\n", 
               x->name.text,
               x->type->name.text);
        printf("field name = %s, type = %s\n", 
               y->name.text,
               x->type->name.text);

        ukn::Vector2 my_vec;
        my_vec.x = 10;
        my_vec.y = 20;
        float* xv = ukn::reflection::FieldCast<float>(x, &my_vec);
        ukn_assert(*xv == 10);
    }
    
    MyApp instance("Test App");

    // create app context
    instance.create(L"config.xml");
    
    
    ukn::FrameCounter::Instance().setDesiredFps(60);
    
    // run app
    instance.run();
    
    
    return 0;
}

