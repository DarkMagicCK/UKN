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
        mRenderBuffer->setRenderMode(ukn::RM_LineLoop);
        
        ukn::Window& wnd = ukn::Context::Instance().getApp().getWindow();
        ukn::Vertex2D* vertices = (ukn::Vertex2D*)mVertexBuffer->map();
        
        float maxy = -100000.f, miny = 1000000.f;
        for(unsigned int i = 0; i < numPoints; ++i) {
            float x = a + (b - a) * (float)i / numPoints;
            vertices[i].x = x;
            vertices[i].y = -mFunc(x);
            vertices[i].z = 0;
            vertices[i].color = ukn::color::Red;
            
            if(maxy < vertices[i].y)
                maxy = vertices[i].y;
            if(miny > vertices[i].y)
                miny = vertices[i].y;
        }
        
        float rScale = (wnd.width() / (b - a)) * scale;
        for(unsigned int i = 0; i < numPoints; ++i) {
            vertices[i].x = vertices[i].x * rScale + wnd.width() / 2 + xOffset;
            vertices[i].y = vertices[i].y * rScale * 2 + wnd.height() / 2 + yOffset;
        }
        
        mVertexBuffer->unmap();
        
        mA = a;
        mB = b;
        mNumPoints = numPoints;
        mScale = rScale;
        mXOffset = xOffset;
        mYOffset = yOffset;
        mMaxY = -miny;
        mMinY = -maxy;
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
    float maxY() const { return mMaxY; }
    float minY() const { return mMinY; }
    unsigned int numPoints() const { return mNumPoints; }
    
private:
    F mFunc;
    float mA;
    float mB;
    float mXOffset;
    float mYOffset;
    float mScale;
    float mMaxY;
    float mMinY;
    unsigned int mNumPoints;
    
    ukn::RenderBufferPtr mRenderBuffer;
    ukn::GraphicBufferPtr mVertexBuffer;
};


struct Vector2 {
    Vector2(): x(0.0), y(0.0) { }
    Vector2(double x, double y): x(x), y(y) { }
    
    Vector2& operator+=(const Vector2& rhs) { x += rhs.x; y += rhs.y; return *this; }
    Vector2 operator+(const Vector2& rhs) const { return Vector2(x + rhs.x, y + rhs.y); }
    Vector2& operator-=(const Vector2& rhs) { x -= rhs.x; y -= rhs.y; return *this; }
    Vector2 operator-(const Vector2& rhs) const { return Vector2(x - rhs.x, y - rhs.y); }
    
    double x, y;
};

struct Expression;

struct Term {
    Term():coefficient(0.0), power(0.0), containsVariable(false) { }
    Term(double co, double power, bool var):
    coefficient(co),
    power(power),
    containsVariable(var) { }
    
    double coefficient;
    double power;
    bool containsVariable;
};

struct Expression {
    Expression() {
    }
    Expression(const Term& term) {
        mTerms.push_back(term);
    }
    Expression(const std::initializer_list<Term>& terms) {
        mTerms = std::vector<Term>(terms.begin(), terms.end());
    }
    Expression(const std::vector<Term>& terms):
    mTerms(terms) { }
    
    Expression& operator=(const Expression& rhs) { mTerms = rhs.mTerms; return *this; }
    
    Expression operator+(const Expression& rhs) const;
    Expression operator-(const Expression& rhs) const;
    Expression operator/(const Expression& rhs) const;
    Expression operator*(const Expression& rhs) const;
    
    Expression operator+(const Term& rhs) const;
    Expression operator-(const Term& rhs) const;
    Expression operator/(const Term& rhs) const;
    Expression operator*(const Term& rhs) const;
    
    Expression& operator+=(const Term& rhs);
    Expression& operator-=(const Term& rhs);
    Expression& operator/=(const Term& rhs);
    Expression& operator*=(const Term& rhs);
    
    friend std::ostream& operator<<(std::ostream& os, const Expression& exp) {
        for(std::vector<Term>::const_iterator it = exp.terms().begin(), end = exp.terms().end();
            it != end;
            ++it) {
            if(it->containsVariable == 0) {
                os << it->coefficient;
            } else {
                os << it->coefficient << "*x^" << it->power;
            }
            if(it != exp.mTerms.end() - 1)
                os << " + ";
        }
        return os;
    }
    
    const std::vector<Term>& terms() const { return mTerms; }
    std::vector<Term>& terms() { return mTerms; }
    
private:
    std::vector<Term> mTerms;
};

Expression& Expression::operator+=(const Term& rhs) {
    for(std::vector<Term>::iterator it = mTerms.begin(), end = mTerms.end();
        it != end;
        ++it) {
        Term& term = *it;
        if(!term.containsVariable && !rhs.containsVariable) {
            term.coefficient += rhs.coefficient;
            return *this;
        } else if(term.containsVariable && rhs.containsVariable) {
            if(term.power == rhs.power) {
                term.coefficient += rhs.coefficient;
                return *this;
            }
        }
    }
    mTerms.push_back(rhs);
    return *this;
}

Expression& Expression::operator-=(const Term& rhs) {
    return Expression::operator+=(Term(-rhs.coefficient, rhs.power, rhs.containsVariable));
}

Expression& Expression::operator/=(const Term& rhs) {
    return Expression::operator*=(Term(1/rhs.coefficient, -1/rhs.power, rhs.containsVariable));
}

Expression& Expression::operator*=(const Term& rhs) {
    std::for_each(mTerms.begin(),
                  mTerms.end(),
                  [=](Term& term) {
                      if(!rhs.containsVariable) {
                          term.coefficient *= rhs.coefficient;
                      } else if(rhs.containsVariable && term.containsVariable) {
                          term.power += rhs.power;
                          term.coefficient *= rhs.coefficient;
                      } else if(rhs.containsVariable && !term.containsVariable) {
                          term.coefficient *= rhs.coefficient;
                          term.power = rhs.power;
                          term.containsVariable = true;
                      }
                  });
    return *this;
}

Expression Expression::operator+(const Term& rhs) const {
    Expression exp = *this;
    exp += rhs;
    return exp;
}

Expression Expression::operator-(const Term& rhs) const {
    return Expression::operator+(Term(-rhs.coefficient, rhs.power, rhs.containsVariable));
}

Expression Expression::operator/(const Term& rhs) const {
    return Expression::operator*(Term(1/rhs.coefficient, -1/rhs.power, rhs.containsVariable));
}

Expression Expression::operator*(const Term& rhs) const {
    Expression exp = *this;
    exp *= rhs;
    return exp;
}

Expression Expression::operator+(const Expression& rhs) const {
    Expression exp = *this;
    for(std::vector<Term>::const_iterator it = rhs.terms().begin(), end = rhs.terms().end();
        it != end;
        ++it) {
        exp += *it;
    }
    return exp;
}

Expression Expression::operator-(const Expression& rhs) const {
    Expression exp = *this;
    for(std::vector<Term>::const_iterator it = rhs.terms().begin(), end = rhs.terms().end();
        it != end;
        ++it) {
        exp -= *it;
    }
    return exp;
}

Expression Expression::operator*(const Expression& rhs) const {
    if(this->mTerms.size() == 0) {
        return rhs;
    } else if(rhs.terms().size() == 0) {
        return *this;
    }
    Expression exp = *this;
    Expression result;
    for(std::vector<Term>::const_iterator it = rhs.terms().begin(), end = rhs.terms().end();
        it != end;
        ++it) {
        result = result + exp * *it;
        std::cout << "result: " << result << std::endl;
        
    }
    return result;
}

Expression Expression::operator/(const Expression& rhs) const {
    if(this->mTerms.size() == 0) {
        return rhs;
    } else if(rhs.terms().size() == 0) {
        return *this;
    }
    Expression exp = *this;
    Expression result;
    for(std::vector<Term>::const_iterator it = rhs.terms().begin(), end = rhs.terms().end();
        it != end;
        ++it) {
        result = result + exp / *it;
    }
    return result;
}

struct LagrangePolynomial {
    LagrangePolynomial();
    
    LagrangePolynomial(const std::initializer_list<Vector2>& points) {
        mPoints = std::vector<Vector2>(points.begin(), points.end());
        doCalculateExpression();
    }
    
    LagrangePolynomial(const std::vector<Vector2>& points):
    mPoints(points) {
        doCalculateExpression();
    }
    
    LagrangePolynomial(const LagrangePolynomial& rhs):
    mPoints(rhs.mPoints) {
        doCalculateExpression();
    }
    
    void setPoints(const std::vector<Vector2>& points) {
        mPoints = points;
    }
    
private:
    void doCalculateExpression() {
        Expression result;
        for(size_t i = 0; i < mPoints.size(); ++i) {
            Expression jterm;
            for(size_t j = 0; j < mPoints.size(); ++j) {
                if(j != i) {
                    Expression dominator = Expression({Term(1, 1, true), Term(-mPoints[j].x, 0, false) });
                    
                    Expression xterm = dominator / Term(mPoints[i].x - mPoints[j].x, 0, false);
                    jterm = xterm * jterm;
                }
            }
            jterm = jterm * Term(mPoints[i].y, 0, false);
            result = result + jterm;
        }
        mExpression = result;
        
        std::cout << mExpression << std::endl;
    }
    
    Expression mExpression;
    
    std::vector<Vector2> mPoints;
};

static float testGraphFunc(float x) {
    return pow(2.71828182846, -(x*x / 10)) * cos(x * x);
}

class MyApp: public ukn::AppInstance {
public:
    MyApp(const ukn::UknString& name):
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
                    
                case ukn::input::Key::Left:
                    testGraph->build(testGraph->a() - 0.5,
                                     testGraph->b(),
                                     5000,
                                     0,
                                     0,
                                     testGraph->scale());
                    break;
                    
                case ukn::input::Key::Right:
                    testGraph->build(testGraph->a() + 0.5f,
                                     testGraph->b() + 0.5f,
                                     5000,
                                     0,
                                     0,
                                     testGraph->scale());
                    break;
                
                default:
                    break;
            }
        }
    }
    
    void onResize(void * sender, ukn::WindowResizeEventArgs& args) {
        testGraph->build(-5, 5, 5000, 0, 0, 1);
    }
        
    void onInit() {
        getWindow().onMouseEvent() += ukn::Bind(this, &MyApp::onMouseEvent);
        getWindow().onKeyEvent() += ukn::Bind(this, &MyApp::onKeyEvent);
        getWindow().onResize() += ukn::Bind(this, &MyApp::onResize);
        
        testGraph = new Graph<float (*)(float)>(-5, 5, 1000, testGraphFunc);
        
        mFont = ukn::AssetManager::Instance().load<ukn::Font>("Thonburi.ttf");
        mFont->setStyleProperty(ukn::FSP_Size, 20);
    }
    
    void onUpdate() {
       
    }
    
    void onRender() {
        ukn::GraphicDevice& gd = ukn::Context::Instance().getGraphicFactory().getGraphicDevice();
        
        gd.clear(ukn::CM_Color | ukn::CM_Depth, ukn::color::Skyblue, 0, 0);
        
        testGraph->render();
        
        mFont->draw((L"MaxY: " + ukn::String::AnyToWString(testGraph->maxY())).c_str(),
                    0,
                    getWindow().height() / 2 - testGraph->maxY() * testGraph->scale() * 2,
                    ukn::FA_Left,
                    ukn::color::Red);
        mFont->draw((L"MinY: " + ukn::String::AnyToWString(testGraph->minY())).c_str(),
                    0,
                    getWindow().height() / 2 - testGraph->minY() * testGraph->scale() * 2,
                    ukn::FA_Left, ukn::color::Red);
        mFont->draw(L"-5",
                    0,
                    getWindow().height() / 2 + 10,
                    ukn::FA_Left, ukn::color::Red);
        mFont->draw(L"5",
                    getWindow().width() - 20,
                    getWindow().height() / 2 + 10,
                    ukn::FA_Left, ukn::color::Red);
        mFont->draw(L"0",
                    getWindow().width() / 2,
                    getWindow().height() / 2 + 10,
                    ukn::FA_Left, ukn::color::Red);
        mFont->render();
    }
    
private:
    Graph<float (*)(float)>* testGraph;
    ukn::FontPtr mFont;
};

#include "UKN/Thread.h"
#include "SqaureGame.h"

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
    SquareGame instance(L"SquareGame!");

    // create app context
    ukn::ContextCfg cfg;
    cfg.render_cfg.width = 1024;
    cfg.render_cfg.height = 600;
    instance.create(cfg);
    
    ukn::FrameCounter::Instance().setDesiredFps(60);
    
    // run app
    instance.run();
    
    
    return 0;
}

