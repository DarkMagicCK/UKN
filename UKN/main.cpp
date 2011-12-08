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

#include "../Plugins/GL/GLPreq.h"

#include <vector>

class MyClassTest {
public:
    UKN_DEF_CLASS_2(MyClassTest, (UKN_DEF_PROP("hello", ukn::ukn_string, "hello world"), UKN_DEF_PROP("i", int, 0)));
    
    
    ukn::EventObject<void(MyClassTest, ukn::Event*)> TestEvent;
    
};

class MyApp: public ukn::AppInstance {
public:
    MyApp(const ukn::ukn_string& name):
    ukn::AppInstance(name) {
        
    }
    
    void onInit() {
        mSpriteBatch = ukn::Context::Instance().getGraphicFactory().createSpriteBatch();
        
        mTexture = ukn::Context::Instance().getGraphicFactory().load2DTexture(L"pong3d_title.tga");

    }
    
    void onUpdate() {
        
    }
    
    void onRender() {
        ukn::Context::Instance().getGraphicFactory().getGraphicDevice().getCurrFrameBuffer()->clear(ukn::CM_Color, ukn::ColorCadetblue, 0, 0);
       
        
        ukn::Timestamp time;
        
        mSpriteBatch->onRenderBegin();
        
        ukn::RandomArea randomobj(ukn::Vector2(0, 0), ukn::Vector2(1024, 768));
        
        for(int i=0; i<10; ++i) {
            ukn::Vector2 pos = randomobj.randomize();
            mSpriteBatch->draw(mTexture, pos.x, pos.y);
        }
        mSpriteBatch->render();

        mSpriteBatch->onRenderEnd();
        
        printf("%lu\n", time.elapsed());
    }
    
private:
    ukn::SharedPtr<ukn::SpriteBatch> mSpriteBatch;
    
    ukn::RenderBufferPtr mRenderBuffer;
    ukn::GraphicBufferPtr mVertexBuffer;
    
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
    
    ukn::RandomRange range(0, 1000);
    for(int i=0; i<10; ++i) {
        printf("%f, ", range.randomize());
    }

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

