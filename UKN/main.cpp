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
#include "UKN/Input.h"

#include "UKN/tmx/TMXTiledMap.h"

#include "UKN/reflection/TypeDatabase.h"

#include <vector>
#include <map>

template<typename F>
class Graph {
public:
    Graph(F func):
    mFunc(func) {
        
    }
    
    Graph(float a, float b, unsigned int numPoints, F func):
    mA(a),
    mB(b),
    mNumPoints(numPoints),
    mFunc(func) {
        build(a, b, numPoints, 0.f, 0.f, 1.0);
    }
    
    void build(float a, float b, unsigned int numPoints, float xOffset, float yOffset, float scale) {
        ukn::GraphicFactory& gf = ukn::Context::Instance().getGraphicFactory();
        mRenderBuffer = gf.createRenderBuffer();
        ukn_assert(mRenderBuffer);
        
        mVertexBuffer = gf.createVertexBuffer(ukn::GraphicBuffer::ReadWrite,
                                              ukn::GraphicBuffer::Static,
                                              numPoints,
                                              0,
                                              ukn::Vertex2D::Format());
        ukn_assert(mVertexBuffer);
        
        mRenderBuffer->bindVertexStream(mVertexBuffer,
                                        ukn::Vertex2D::Format());
        mRenderBuffer->setRenderMode(ukn::RM_Point);
        
        ukn::Window& wnd = ukn::Context::Instance().getApp().getWindow();
        ukn::Vertex2D* vertices = (ukn::Vertex2D*)mVertexBuffer->map();
        
        for(unsigned int i = 0; i < numPoints; ++i) {
            float x = a + (b - a) * (float)i / numPoints;
            vertices[i].x = x;
            vertices[i].y = -mFunc(x);
            vertices[i].z = 0;
            vertices[i].color = 0xFF0000FF;
        }
        
        float rScale = (wnd.width() / (b - a)) * scale;
        for(unsigned int i = 0; i < numPoints; ++i) {
            vertices[i].x = vertices[i].x * rScale + wnd.width() / 2 + xOffset;
            vertices[i].y = vertices[i].y * rScale + wnd.height() / 2 + yOffset;
        }
        
        mVertexBuffer->unmap();
        
        mA = a;
        mB = b;
        mNumPoints = numPoints;
        mScale = scale;
        mXOffset = xOffset;
        mYOffset = yOffset;
    }
    
    void setColor(const ukn::Color& clr) {
        ukn::Vertex2D* vertices = (ukn::Vertex2D*)mVertexBuffer->map();
        for(unsigned int i = 0; i < mNumPoints; ++i) {
            vertices[i].color = clr.toHWColor();
        }
        mVertexBuffer->unmap();
    }
    
    void render() {
        ukn::GraphicDevice& gd = ukn::Context::Instance().getGraphicFactory().getGraphicDevice();
        gd.onRenderBuffer(mRenderBuffer);
    }
    
    float a() const { return mA; }
    float b() const { return mB; }
    float scale() const { return mScale; }
    unsigned int numPoints() const { return mNumPoints; }
    
private:
    F mFunc;
    float mA;
    float mB;
    float mXOffset;
    float mYOffset;
    float mScale;
    unsigned int mNumPoints;
    
    ukn::RenderBufferPtr mRenderBuffer;
    ukn::GraphicBufferPtr mVertexBuffer;
};

static float testGraphFunc(float x) {
    return pow(2.71828182846f, -x*x / 10) * cos(x * x);
}

class MyApp: public ukn::AppInstance {
public:
    MyApp(const ukn::ukn_string& name):
    ukn::AppInstance(name) {
        
    }
    
    void onMouseEvent(void* sender, ukn::input::MouseEventArgs& e) {
   
    }
    
    void onKeyEvent(void* sender, ukn::input::KeyEventArgs& e) {
        if(e.state == ukn::input::Key::Press) {
            switch(e.key) {
                case ukn::input::Key::Equals:
                    testGraph->build(-5, 5, 5000, 0, 0, testGraph->scale() * 1.57);
                    break;
                
                case ukn::input::Key::Minus:
                    testGraph->build(-5, 5, 5000, 0, 0, testGraph->scale() * 0.717);
                    break;
                
                default:
                    break;
            }
        }
    }
        
    void onInit() {
        getWindow().onMouseEvent() += ukn::Bind(this, &MyApp::onMouseEvent);
        getWindow().onKeyEvent() += ukn::Bind(this, &MyApp::onKeyEvent);
        
        testGraph = new Graph<float (*)(float)>(-5, 5, 5000, testGraphFunc);
    }
    
    void onUpdate() {
       
    }
    
    void onRender() {
        ukn::GraphicDevice& gd = ukn::Context::Instance().getGraphicFactory().getGraphicDevice();
        
        gd.clear(ukn::CM_Color | ukn::CM_Depth, ukn::color::Black, 0, 0);
        
        testGraph->render();
        
    }
    
private:
    Graph<float (*)(float)>* testGraph;
};

#include "UKN/Thread.h"

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
    MyApp instance("Graph It!");

    // create app context
    instance.create(L"config.xml");
    
    ukn::FrameCounter::Instance().setDesiredFps(60);
    
    // run app
    instance.run();
    
    
    return 0;
}

