//
//  LeapTestApp.cpp
//  Project Unknown
//
//  Created by Robert Bu on 11/28/12.
//  Copyright (c) 2012 heizi. All rights reserved.
//

#include "LeapTestApp.h"

#include "UKN/UKN.h"
#include "UKN/LeapMotion.h"

#include "mist/Serial.h"

class TestApp: public ukn::AppInstance, public ukn::input::LeapMotionListener {
public:
    TestApp(const ukn::UknString& name):
    ukn::AppInstance(name) {
        
    }
    virtual ~TestApp() {
        
    }
    
    void onLeapMotionFrame(const Leap::Controller& c) {
        mLastFrame = c.frame();
        mPrevFrame = c.frame(1);
      
    }
    
    void writeCommand(ukn::uint8 instr, const ukn::uint8* d, ukn::uint16 len) {
        ukn::uint16 real_len = (ukn::uint16)(len + 5);
        
        ukn::uint8* data = (ukn::uint8*)mist::mist_malloc(real_len);
        data[0] = 0xFF;
        data[1] = 0xFF;
        data[2] = instr;
        data[3] = (ukn::uint8)(real_len & 0xFF);
        data[4] = (ukn::uint8)((real_len >> 8) & 0xFF);
        if(d && len > 0)
            memcpy(&data[5], d, len);
        
        mArduinoSerial.write((const char*)data, real_len);
    }
    
    void onMouseEvent(void* sender, ukn::input::MouseEventArgs& e) {
        if(e.state == ukn::input::Mouse::Press) {
            this->writeCommand(0, 0, 0);
        }
    }
    
    void onKeyEvent(void* sender, ukn::input::KeyEventArgs& e) {
        mArduinoSerial.write((const char*)&e.key, 1);
    }
    
    void onResize(void * sender, ukn::WindowResizeEventArgs& args) {
        
    }
    
    void onInit() {
        getWindow().onMouseEvent() += mist::Bind(this, &TestApp::onMouseEvent);
        getWindow().onKeyEvent() += mist::Bind(this, &TestApp::onKeyEvent);
        getWindow().onResize() += mist::Bind(this, &TestApp::onResize);
        
        
        ukn::GraphicFactory& gf = ukn::Context::Instance().getGraphicFactory();
        mSquareTexture = gf.create2DTexture(getWindow().width(),
                                            getWindow().height(),
                                            0,
                                            ukn::EF_RGBA8,
                                            0);
        mFrameBuffer = gf.createFrameBuffer();
        mFrameBuffer->attach(ukn::ATT_Color0, gf.create2DRenderView(mSquareTexture));
        mFrameBuffer->attach(ukn::ATT_DepthStencil, gf.create2DDepthStencilView(gf.create2DTexture(getWindow().width(),
                                                                                                   getWindow().height(),
                                                                                                   0,
                                                                                                   ukn::EF_D16,
                                                                                                   0)));
        
        
        mRenderBuffer = gf.createRenderBuffer();
        mist_assert(mRenderBuffer);
        
        mVertexBuffer = gf.createMemoryVertexBuffer<ukn::Vertex2D>(0, 0);
        mist_assert(mVertexBuffer);
        
        mRenderBuffer->bindVertexStream(mVertexBuffer,
                                        ukn::Vertex2D::Format());
        mRenderBuffer->setRenderMode(ukn::RM_Point);
        
        
        mLeapModule = new ukn::input::LeapMotionModule();
        mLeapModule->attachListener(this);
        ukn::ModuleManager::Instance().addModule(mLeapModule);
        
        mContext = new ukn::GraphicContext();
        
        mFont = ukn::AssetManager::Instance().load<ukn::Font>(L"Courier New.ttf");
        mist_assert(mFont);
        
        this->loadImage(L"angel.png");
        
        if(!mArduinoSerial
            .port("/dev/tty.usbmodemfa141")
            .baudRate(9600)
            .open()) {
            mist::log_error("error opening arduino serial");
        }
    }
    
    void onUpdate() {
    }
    
    void loadImage(const ukn::MistString& path) {
        
        mCurrentImage = ukn::AssetManager::Instance().load<ukn::Texture>(path);
        mist_assert(mCurrentImage);
        
        mCurrentPos.x = getWindow().width() / 2;
        mCurrentPos.y = getWindow().height() / 2;
        
        mCurrentRot = 0.f;
        
        double scale = std::min((double)getWindow().width() / mCurrentImage->getWidth(),
                                (double)getWindow().height() / mCurrentImage->getHeight());
        mCurrentScale = ukn::Vector2(scale, scale);
    }
    
    void onRender() {
        ukn::GraphicDevice& gd = ukn::Context::Instance().getGraphicFactory().getGraphicDevice();
        
        gd.bindFrameBuffer(mFrameBuffer);
        gd.clear(ukn::CM_Color | ukn::CM_Depth, ukn::color::Black, 0, 0);
        
        gd.setRenderState(ukn::RS_PointSize, mist::Convert::ReinterpretConvert<float, ukn::uint32>(4.0));
        gd.setRenderState(ukn::RS_Blend, ukn::RSP_Enable);
        gd.setRenderState(ukn::RS_SrcAlpha, ukn::RSP_BlendFuncOneMinusSrcAlpha);
        gd.setRenderState(ukn::RS_ColorOp, ukn::RSP_ColorOpModulate);
        
        gd.onRenderBuffer(mRenderBuffer);
        
        gd.bindFrameBuffer(gd.getScreenFrameBuffer());
        gd.clear(ukn::CM_Color | ukn::CM_Depth, ukn::color::Black, 0, 0);
        
        
        ukn::SpriteBatch& sb = ukn::SpriteBatch::DefaultObject();
        sb.begin();
        
        sb.draw(mCurrentImage,
                mCurrentPos,
                ukn::Vector2(mCurrentImage->getWidth() / 2,
                             mCurrentImage->getHeight() / 2),
                mCurrentRot,
                mCurrentScale);
        
        sb.end();
        
        mContext->begin();
        
        for(const Leap::Hand& h: mLastFrame.hands()) {
            for(const Leap::Finger& f: h.fingers()) {
                
                mContext->setFillColor(ukn::color::Skyblue);
                mContext->fillCircle(f.tip().position.x * 3 + getWindow().width() / 2,
                                     getWindow().height() - f.tip().position.y * 3,
                                     0,
                                     10);
            }
        }
        
        mContext->end();
        
        
        if(mPrevFrame.hands().size() == 1 and mLastFrame.hands().size() == 1) {
            if(mPrevFrame.hands()[0].fingers().size() == 1 &&
               mLastFrame.hands()[0].fingers().size() == 1) {
                Leap::Finger prevF = mPrevFrame.hands()[0].fingers()[0];
                Leap::Finger currF = mLastFrame.hands()[0].fingers()[0];
                
                Leap::Vector delta = Leap::Vector(prevF.tip().position.x - currF.tip().position.x,
                                                  prevF.tip().position.y - currF.tip().position.y,
                                                  prevF.tip().position.z - currF.tip().position.z);
                
                mCurrentPos.x = currF.tip().position.x * 3 + getWindow().width() / 2;
                mCurrentPos.y = getWindow().height() - currF.tip().position.y * 3;
                printf("%f, %f, %f\n", delta.x, delta.y, delta.z);
            } else {
                
                
            }
        }
        
        if(mLastFrame.hands().size() > 0) {
            if(mLastFrame.hands()[0].fingers().size() > 0) {
                Leap::Finger f = mLastFrame.hands()[0].fingers()[0];
                
                ukn::uint8 d[2];
                
                d[0] = ((float)(f.tip().position.x * 3 + getWindow().width() / 2) / getWindow().width()) * 128;
                d[1] = ((float)(getWindow().height() - f.tip().position.y * 3) / getWindow().height()) * 160;
                
                this->writeCommand(2, d, 2);
            }
        }
        
    }
    
private:
    ukn::TexturePtr mSquareTexture;
    ukn::FontPtr mFont;
    ukn::FrameBufferPtr mFrameBuffer;
    
    ukn::GraphicContext* mContext;
    
    ukn::RenderBufferPtr mRenderBuffer;
    ukn::SharedPtr<ukn::MemoryGraphicBuffer<ukn::Vertex2D> > mVertexBuffer;
    
    ukn::input::LeapMotionModule* mLeapModule;
    Leap::Frame mLastFrame, mPrevFrame;
    
    ukn::TexturePtr mCurrentImage;
    float mCurrentRot;
    ukn::Vector2 mCurrentPos;
    ukn::Vector2 mCurrentScale;

    
    mist::Serial mArduinoSerial;
};

void StartLeapApp() {
    ukn::FrameCounter::Instance().setDesiredFps(15);
    TestApp(L"LeapMotion & Arduino Test")
        .create(
                ukn::ContextCfg::Default()
                .width(800)
                .height(600)
                )
        .run();
}