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
    
    Graph(float a, float b, unsigned int numPoints, F func, float scale = 1.0):
    mA(a),
    mB(b),
    mNumPoints(numPoints),
    mFunc(func) {
        build(a, b, numPoints, 0.f, 0.f, scale);
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
        
        float rScale;
        if(scale == 1.0) {
            float s1 = (float)wnd.height() / 2 / (maxy > 0.0 ? maxy: -maxy);
            float s2 = (float)wnd.height() / 2 / (miny > 0.0 ? miny: -miny);
            rScale = s1 < s2 ? s1 : s2;
        } else {
            rScale = scale;
        }
        
        for(unsigned int i = 0; i < numPoints; ++i) {
            vertices[i].x = vertices[i].x * rScale + wnd.width() / 2 + xOffset;
            vertices[i].y = vertices[i].y * rScale + wnd.height() / 2 + yOffset;
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
    
    double operator()(double x) {
        double result = 0.0;
        for(std::vector<Term>::const_iterator it = mTerms.begin(), end = mTerms.end();
            it != end;
            ++it) {
            const Term& term = *it;
            if(term.containsVariable) {
                result += term.coefficient * pow(x, term.power);
            } else {
                result += term.coefficient;
            }
        }
        return result;
    }
    
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
    
    double operator()(double x) {
        return mExpression(x);
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
    }
    
    Expression mExpression;
    
    std::vector<Vector2> mPoints;
};

struct Spline {
    typedef std::vector<double> DoubleVector;
    
    Spline() {
    }
    
    Spline(const std::initializer_list<Vector2>& points):
    mPoints(points.begin(), points.end()) {
        setup();
    }
    
    Spline(const std::vector<Vector2>& points):
    mPoints(points) {
        setup();
    }
    
    double operator()(double v) const {
        size_t D;
        if(v <= mPoints[1].x) {
            D = 0;
        } else if(v >= mPoints[mPoints.size()-2].x) {
            D = mPoints.size() - 2;
        } else {
            for(size_t j = 1; j < mPoints.size()-2; ++j) {
                if(v <= mPoints[j+1].x) {
                    D = j;
                    break;
                }
            }
        }
        
        double h = mPoints[D+1].x - mPoints[D].x;
        double s;
        if(D == 0) {
            
            double b = mPoints[0].y / h;
            double c = mPoints[1].y / h - m_A[0] * h / 6;
            s = m_A[0] * pow((v - mPoints[0].x), 3) / (6 * h) +
                b * (mPoints[1].x - v) +
                c * (v - mPoints[0].x);
            
        } else if(D == mPoints.size()-2) {
            
            double b = mPoints[mPoints.size()-2].y / h - m_A[mPoints.size()-3] * h / 6;
            double c = mPoints.back().y / h;
            s = m_A[mPoints.size()-3] * pow((mPoints.back().x - v), 3) / (6 * h) +
                b * (mPoints.back().x - v) +
                c * (v - mPoints[mPoints.size()-2].x);
            
        } else {
            
            double b = mPoints[D].y / h - m_A[D-1] * h / 6;
            double c = mPoints[D+1].y / h - m_A[D] * h / 6;
            s = m_A[D-1] * pow(mPoints[D+1].x - v, 3) / (6 * h) +
                m_A[D] * pow(v - mPoints[D].x, 3) / (6 * h) +
                b * (mPoints[D+1].x - v) +
                c * (v - mPoints[D].x);
            
        }
        return s;
    }
    
private:
    void setup() {
        DoubleVector T, H, R, D, U, L;
        
        T.resize(mPoints.size() - 1);
        H.resize(mPoints.size() - 1);
        R.resize(mPoints.size() - 2);
        D.resize(mPoints.size() - 2);
        U.resize(mPoints.size() - 2);
        L.resize(mPoints.size() - 2);
        
        for(size_t i = 0; i < mPoints.size() - 1; ++i) {
            H[i] = mPoints[i+1].x - mPoints[i].x;
            T[i] = (mPoints[i+1].y - mPoints[i].y) / H[i];
        }
        
        for(size_t i = 0; i < mPoints.size() - 2; ++i) {
            R[i] = T[i+1] - T[i];
            D[i] = (H[i] + H[i+1]) / 3;
        }
        
        for(size_t i = 0; i < mPoints.size() - 3; ++i) {
            U[i] = H[i+1] / 6;
            L[i+1] = U[i];
        }
        
        U[mPoints.size() - 3] = 0;
        L[0] = 0;
        
        auto BB_DD = this->LU_Factor_TriDiag(U, D, L);
        m_A = this->TriDiag_Solve(U, std::get<1>(BB_DD), std::get<0>(BB_DD), R);
    }
    
    std::tuple<DoubleVector, DoubleVector> LU_Factor_TriDiag(const DoubleVector& U, const DoubleVector& D, const DoubleVector& L) {
        DoubleVector BB;
        DoubleVector DD;
        
        BB.resize(D.size());
        DD.resize(D.size());
        
        BB[0] = 0;
        DD[0] = D[0];
        
        for(size_t i = 1; i < D.size(); ++i) {
            BB[i] = L[i] / DD[i-1];
            DD[i] = D[i] - BB[i] * U[i-1];
        }
        
        return std::make_tuple(BB, DD);
    }
    
    // solve uses R, BB(L), DD(D), U
    DoubleVector TriDiag_Solve(const DoubleVector& U, const DoubleVector& D, const DoubleVector& L, const DoubleVector& R) {
        DoubleVector Z;
        DoubleVector X;
        
        Z.resize(D.size());
        X.resize(D.size());
        
        Z[0] = R[0];
        
        for(size_t i = 1; i < D.size(); ++i) {
            Z[i] = R[i] - L[i] * Z[i-1];
        }
        
        X[D.size()-1] = Z[D.size()-1] / D[D.size()-1];
        for(int i = (int)D.size()-2; i >= 0; --i) {
            X[i] = (Z[i] - U[i] * X[i+1]) / D[i];
        }
        
        return X;
    }
    
    std::vector<Vector2> mPoints;
    DoubleVector m_A;
};

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
        testGraph->build(-1, 1, 5000, 0, 0, 1);
    }
        
    void onInit() {
        getWindow().onMouseEvent() += ukn::Bind(this, &MyApp::onMouseEvent);
        getWindow().onKeyEvent() += ukn::Bind(this, &MyApp::onKeyEvent);
        getWindow().onResize() += ukn::Bind(this, &MyApp::onResize);
        
        testGraph = new Graph<LagrangePolynomial>(-1.0,
                                                  1.0,
                                                  5000,
                                                  LagrangePolynomial({
                                                      Vector2(-1.0, -1.0),
                                                      Vector2(-0.96, -0.151),
                                                      Vector2(-0.86, 0.894),
                                                      Vector2(-0.79, 0.986),
                                                      Vector2(0.22, 0.895),
                                                      Vector2(0.5, 0.5),
                                                      Vector2(0.93, -0.306)
                                                  }),
                                                  260);
        testGraph2 = new Graph<Spline>(-1.0,
                                       1.0,
                                       5000,
                                       Spline({
                                            Vector2(-1.0, -1.0),
                                            Vector2(-0.96, -0.151),
                                            Vector2(-0.86, 0.894),
                                            Vector2(-0.79, 0.986),
                                            Vector2(0.22, 0.895),
                                            Vector2(0.5, 0.5),
                                            Vector2(0.93, -0.306)
                                       }),
                                       260);
       testGraph->setColor(ukn::color::White);
        
        mFont = ukn::AssetManager::Instance().load<ukn::Font>("Thonburi.ttf");
        mFont->setStyleProperty(ukn::FSP_Size, 20);
    }
    
    void onUpdate() {
       
    }
    
    void onRender() {
        ukn::GraphicDevice& gd = ukn::Context::Instance().getGraphicFactory().getGraphicDevice();
        
        gd.clear(ukn::CM_Color | ukn::CM_Depth, ukn::color::Black, 0, 0);
        
     //   testGraph->render();
        testGraph2->render();
      
        mFont->draw(L"-1",
                    0,
                    getWindow().height() / 2 + 10,
                    ukn::FA_Left, ukn::color::Red);
        mFont->draw(L"1",
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
    Graph<LagrangePolynomial>* testGraph;
    Graph<Spline>* testGraph2;
    ukn::FontPtr mFont;
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
    MyApp instance(L"Lagrange & Spline");

    // create app context
    ukn::ContextCfg cfg;
    cfg.render_cfg.width = 600;
    cfg.render_cfg.height = 600;
    instance.create(cfg);
    
    ukn::FrameCounter::Instance().setDesiredFps(60);
    
    // run app
    instance.run();
    
    
    return 0;
}

