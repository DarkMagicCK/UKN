//
//  SqaureGame.h
//  Project Unknown
//
//  Created by Robert Bu on 8/31/12.
//  Copyright (c) 2012 heizi. All rights reserved.
//

#ifndef Project_Unknown_SqaureGame_h
#define Project_Unknown_SqaureGame_h


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
#include "UKN/Input.h"

class SquareGame: public ukn::AppInstance {
public:
    SquareGame(const ukn::UknString& name):
    ukn::AppInstance(name) {
        
    }
    
    void onMouseEvent(void* sender, ukn::input::MouseEventArgs& e) {
        if(mRect.contains(e.x, e.y)) {
            mRect.setXY(ukn::Random::RandomFloat(0,
                                                 getWindow().width() - mRect.width()),
                        ukn::Random::RandomFloat(0,
                                                 getWindow().height() - mRect.height()));
            
            mScore += 1;
            mTimeLeft = 5.f;
        }
    }
    
    void onKeyEvent(void* sender, ukn::input::KeyEventArgs& e) {
        
    }
    
    void onResize(void * sender, ukn::WindowResizeEventArgs& args) {
        
    }
    
    void onInit() {
        getWindow().onMouseEvent() += ukn::Bind(this, &SquareGame::onMouseEvent);
        getWindow().onKeyEvent() += ukn::Bind(this, &SquareGame::onKeyEvent);
        getWindow().onResize() += ukn::Bind(this, &SquareGame::onResize);
                
        mFont = ukn::AssetManager::Instance().load<ukn::Font>("Thonburi.ttf");
        mFont->setStyleProperty(ukn::FSP_Size, 20);
        
        mRect = ukn::Rectangle(0, 0, 100, 100);
        
        ukn::GraphicFactory& gf = ukn::Context::Instance().getGraphicFactory();
        mSquareTexture = gf.create2DTexture(mRect.width(),
                                            mRect.height(),
                                            0,
                                            ukn::EF_ARGB8,
                                            0);
        mScore = 0;
        mTimeLeft = 5.f;
    }
    
    void onUpdate() {
        mTimeLeft -= ukn::FrameCounter::Instance().getPrevDelta();
        getWindow().setWindowTitle(ukn::String::StringToWString(ukn::format_string("Square Game [Score = %d, TimeLeft = %.2f]", mScore, mTimeLeft)));
        
        if(mTimeLeft < 0.f) {
            ukn::MessageBox::Show(L"Timeout!", L"Game", ukn::MBO_OK);
            mTimeLeft = 5.f;
            mScore = 0.f;
        }
    }
    
    void onRender() {
        ukn::GraphicDevice& gd = ukn::Context::Instance().getGraphicFactory().getGraphicDevice();
        
        gd.clear(ukn::CM_Color | ukn::CM_Depth, ukn::color::Skyblue, 0, 0);
        
        ukn::SpriteBatch& sb = ukn::SpriteBatch::DefaultObject();
        sb.begin();
        sb.draw(mSquareTexture,
                ukn::Vector2(mRect.x(),
                             mRect.y()),
                ukn::color::Red);
        sb.end();
        
        mFont->render();
    }
    
private:
    ukn::TexturePtr mSquareTexture;
    ukn::Rectangle mRect;
    ukn::FontPtr mFont;
    int mScore;
    float mTimeLeft;
};

#endif
