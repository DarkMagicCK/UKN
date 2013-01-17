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
#include "UKN/Texture.h"
#include "UKN/GraphicDevice.h"
#include "UKN/FrameBuffer.h"
#include "UKN/Texture.h"
#include "UKN/SpriteBatch.h"
#include "UKN/RenderBuffer.h"
#include "UKN/Font.h"
#include "UKN/Asset.h"
#include "UKN/Skeletal.h"
#include "UKN/Input.h"

#include "UKN/tmx/TMXTiledMap.h"


#include <vector>
#include <map>

template<typename F>
class Graph {
public:
    Graph(F func):
    mFunc(func) {
        
    }
    
    Graph(float a, float b, unsigned int numPoints, F func, float scale = 1.0, float xoff = 0.f, float yoff = 0.f):
    mA(a),
    mB(b),
    mNumPoints(numPoints),
    mFunc(func) {
        build(a, b, numPoints, xoff, yoff, scale);
    }
    
    Graph(float a, float b, unsigned int numPoints, F func, F func2, float scale = 1.0, float xoff = 0.f, float yoff = 0.f):
    mA(a),
    mB(b),
    mNumPoints(numPoints),
    mFunc(func),
    mFunc2(func2) {
        build(a, b, numPoints, xoff, yoff, scale);
    }
    
    
    void build(float a, float b, unsigned int numPoints, float xOffset, float yOffset, float scale) {
        ukn::GraphicFactory& gf = ukn::Context::Instance().getGraphicFactory();
        mRenderBuffer = gf.createRenderBuffer();
        mist_assert(mRenderBuffer);
        
        mVertexBuffer = gf.createVertexBuffer(ukn::GraphicBuffer::ReadWrite,
                                              ukn::GraphicBuffer::Static,
                                              numPoints,
                                              0,
                                              ukn::Vertex2D::Format());
        mist_assert(mVertexBuffer);
        
        mRenderBuffer->bindVertexStream(mVertexBuffer,
                                        ukn::Vertex2D::Format());
        mRenderBuffer->setRenderMode(ukn::RM_Point);
        
        ukn::Window& wnd = ukn::Context::Instance().getApp().getWindow();
        ukn::Vertex2D* vertices = (ukn::Vertex2D*)mVertexBuffer->map();
        
        float maxy = -100000.f, miny = 1000000.f;
        float maxx = -100000.f, minx = 1000000.f;
        for(unsigned int i = 0; i < numPoints; ++i) {
            float x = a + (b - a) * (float)i / numPoints;
            vertices[i].x = mFunc2 ? mFunc2(x) : x;
            vertices[i].y = -mFunc(x);
            vertices[i].z = 0;
            vertices[i].color = ukn::color::Red;
            
            if(maxy < vertices[i].y)
                maxy = vertices[i].y;
            if(miny > vertices[i].y)
                miny = vertices[i].y;
            if(maxx < vertices[i].x)
                maxx = vertices[i].x;
            if(minx > vertices[i].x)
                minx = vertices[i].x;
        }
        
        float rScaleX;
        float rScaleY;
        float rScale;
        if(scale == 1.0) {
            float s1 = (float)wnd.height() / 2 / (maxy > 0.0 ? maxy: -maxy + 1.0);
            float s2 = (float)wnd.height() / 2 / (miny > 0.0 ? miny: -miny + 1.0);
            rScaleY = s1 < s2 ? s1 : s2;
            
            s1 = (float)wnd.width() / 2 / (maxx > 0.0 ? maxx: -maxx + 1.0);
            s2 = (float)wnd.width() / 2 / (minx > 0.0 ? minx: -minx + 1.0);
            rScaleX = s1 < s2 ? s1 : s2;
            
            rScale = rScaleX < rScaleY ? rScaleX : rScaleY;
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
            vertices[i].color = 0x22FF0000;
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
    F mFunc2;
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
    
    double operator()(double x) const {
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
    LagrangePolynomial() {}
    
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
        doCalculateExpression();
    }
    
    const Expression& getExpression() const  {
        return mExpression;
    }
    
    double operator()(double x) const {
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
    
    void setPoints(const std::vector<Vector2>& vec) {
        mPoints = vec;
        setup();
    }
    
    const DoubleVector& getA() const {
        return m_A;
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

#define PI 3.141592653589793238462643383279502884

#include <cassert>
#include <array>

/* Simple template based matrixNM, M(Column) * N(Row) matrix */
template<size_t N, size_t M, typename _DATA_TYPE = float>
struct MatrixMN {
public:
    typedef _DATA_TYPE data_type;
    typedef MatrixMN<M, N, _DATA_TYPE> self_type;
    typedef typename std::array<_DATA_TYPE, N> row_type;
    typedef typename std::array<_DATA_TYPE, M> column_type;
    
    /* enum hack to get dimension and length here */
    enum { Column = M };
    enum { Row = N };
    enum { Length = M * N };
    
    /* union to simplify data storage and access, column->row */
    union {
        data_type c[N][M];
        data_type d[Length];
    };
    
public:
    MatrixMN() {
        memset(this->d, 0, sizeof(_DATA_TYPE) * Length);
    }
    /* std::initializer_list constructor, for faster construction */
    MatrixMN(std::initializer_list<_DATA_TYPE> list) {
        /* initializer list size must larger than N*M */
        assert(list.size() >= Length);
        memcpy(this->d, list.begin(), sizeof(_DATA_TYPE) * Length);
    }
    MatrixMN(const self_type& rhs) {
        memcpy(this->d, rhs.d, sizeof(_DATA_TYPE) * Length);
    }
    // rvalue constructor
    MatrixMN(const self_type&& rval) {
        memcpy(this->d, rval.d, sizeof(_DATA_TYPE) * Length);
    }
    
    self_type& operator=(const self_type& rhs) {
        memcpy(this->d, rhs.d, sizeof(_DATA_TYPE) * Length);
    }
    self_type operator+(const self_type& rhs) const {
        MatrixMN temp = *this;
        for(size_t i = 0; i < Length; ++i) {
            temp.d[i] += rhs.d[i];
        }
        return temp;
    }
    self_type operator-(const self_type& rhs) const {
        MatrixMN temp = *this;
        for(size_t i = 0; i < Length; ++i) {
            temp.d[i] -= rhs.d[i];
        }
        return temp;
    }
    self_type& operator+=(const self_type& rhs) {
        for(size_t i = 0; i < Length; ++i) {
            this->d[i] += rhs.d[i];
        }
        return *this;
    }
    self_type& operator-=(const self_type& rhs) {
        for(size_t i = 0; i < Length; ++i) {
            this->d[i] += rhs.d[i];
        }
        return *this;
    }
    
    bool operator==(const self_type& rhs) const {
        for(size_t i = 0; i < Length; ++i) {
            if(this->d[i] != rhs.d[i])
                return false;
        }
        return true;
    }
    /* implicit convertion to std::array */
    operator std::array<_DATA_TYPE, Length>() const {
        std::array<_DATA_TYPE, Length> a;
        for(size_t i = 0; i < Length; ++i) {
            a[i] = this->d[i];
        }
        return a;
    }
    
    /* for output */
    friend std::ostream& operator<<(std::ostream& os, const MatrixMN& m) {
        for(size_t i = 0; i < m.Row; ++i) {
            for(size_t j = 0; j < m.Column; ++j) {
                os << m.c[i][j] << " ";
            }
            os << std::endl;
        }
        return os;
    }
};

template<typename T>
T dabs(T v) {
    return v < 0 ? -v : v;
}

template<int MIN_ITR, int MAX_ITR>
double RombergIntegration(double a, double b, const std::function<double(double)>& f, double epsilon) {
    double h = b - a;
    
    MatrixMN<2, MAX_ITR> r;
    r.c[0][0] = h * (f(a) + f(b)) / 2.0;
    
    for(size_t i=1; i<MAX_ITR; ++i) {
        double sum = 0.0;
        for(size_t k=1; k<=pow(2, i-1); ++k) {
            sum += f(a + (k-0.5) * h);
        }
        r.c[1][0] = 0.5 * (r.c[0][0] + h * sum);
        for(size_t j=1; j<i; ++j) {
            r.c[1][j] = (pow(4.0, j) * r.c[1][j-1] - r.c[0][j-1]) / (pow(4.0, j) - 1);
        }
        
        if(dabs(r.c[1][i-1] - r.c[0][i-2]) < epsilon && i >= MIN_ITR) {
            return r.c[1][i-1];
        } else {
            h = h / 2.0;
            for(size_t j=0; j<i; ++j) {
                r.c[0][j] = r.c[1][j];
            }
        }
    }
    
    return r.c[1][MAX_ITR-1];
}

struct FresnelFunction {
    double operator()(double x) {
        return RombergIntegration<10, 100>(0.0, x, _f, _epsilon);
    }
    
    std::function<double(double)> _f;
    double _a;
    double _epsilon;
    
    FresnelFunction(const std::function<double(double)>& f, double epsilon):
    _f(f),
    _epsilon(epsilon) {
        
    }
    FresnelFunction() {
        
    }
};

struct SleepingCurve {
    SleepingCurve(const std::vector<Vector2>& time):
    m_time(time) {
        
    }
    
    double operator()(const double& x) const{
        double dist = x - floor(x);
        unsigned index = floor(x);
        return m_time[index].x * 60 + m_time[index].y;
    }
    
    std::vector<Vector2> m_time;
};

class MyApp: public ukn::AppInstance {
public:
    MyApp(const ukn::UknString& name):
    ukn::AppInstance(name) {
        
    }
    
    void onMouseEvent(void* sender, ukn::input::MouseEventArgs& e) {
   
    }
    
    void onKeyEvent(void* sender, ukn::input::KeyEventArgs& e) {
        
    }
    
    void onResize(void * sender, ukn::WindowResizeEventArgs& args) {
    }
        
    void onInit() {
        getWindow().onMouseEvent() += ukn::Bind(this, &MyApp::onMouseEvent);
        getWindow().onKeyEvent() += ukn::Bind(this, &MyApp::onKeyEvent);
        getWindow().onResize() += ukn::Bind(this, &MyApp::onResize);
       
        std::vector<Vector2> vec {
            Vector2(33, 1),
            Vector2(35, 2),
            Vector2(34, 3),
            Vector2(10, 4),
            Vector2(12, 5),
            Vector2(34, 6),
            Vector2(74, 7),
            Vector2(91, 8),
            Vector2(88, 9),
            Vector2(66, 10),
            Vector2(44, 11),
            Vector2(40, 12),
            Vector2(36, 13),
        };
        std::vector<Vector2> vec2 {
            Vector2(2, 1),
            Vector2(18, 2),
            Vector2(22, 3),
            Vector2(24, 4),
            Vector2(36, 5),
            Vector2(42, 6),
            Vector2(42, 7),
            Vector2(36, 8),
            Vector2(24, 9),
            Vector2(24, 10),
            Vector2(22, 11),
            Vector2(18, 12),
            Vector2(2, 13),
        };
        
        std::for_each(vec.begin(), vec.end(), [](Vector2& t) {
            float tt = t.x;
            t.x = t.y;
            t.y = tt;
        });
        std::for_each(vec2.begin(), vec2.end(), [](Vector2& t) {
            float tt = t.x;
            t.x = t.y;
            t.y = tt;
        });
        /*
        std::vector<Vector2> vec2 {
            Vector2(2, 1),
            Vector2(18, 2),
            Vector2(22, 3),
            Vector2(24, 4),
            Vector2(36, 5),
            Vector2(42, 6),
        };
        */
        float scale = 100;
        
        s1 = Spline(vec);
        s2 = Spline(vec2);
        
        std::vector<Vector2> points;
        for(int i=1; i<=11; ++i) {
            double x = -1 + (i-1.0) / 5.0;
            points.push_back(Vector2(x, pow(1 + 25 * x * x, -1)));
        }
        
        s1 = Spline(points);
        
        
        points.clear();
        for(int i=1; i<=11; ++i) {
            double x = cos(PI * (23- 2*i) /22.0);
            points.push_back(Vector2(x, pow(1 + 25 * x * x, -1)));
        }
        
        s2 = Spline(points);
        
                
     /*   lag2->setColor(ukn::color::White);
        spline1->setColor(ukn::color::Pink);
        spline2->setColor(ukn::color::Green);
       */ 
        runge = new Graph<std::function<float(float)> >(1,
                                            13,
                                            13000,
                                            [&](float x) -> float {
                                                return s2(x);
                                            },
                                            [&](float x) -> float {
                                                return s1(x);
                                            },
                                                        1.f,
                                                        -500.f,
                                                        200.f);
        runge->setColor(ukn::color::Pink);
        
        r1 = r2 = r3 = r4 = false;
        
        
        sx = new Graph<std::function<double(double)>>(0.0,
                                                      5.0,
                                                      3000,
                                                      FresnelFunction([](double x) ->double { return sin(x*x); },
                                                                      1E-10));
        cx = new Graph<std::function<double(double)>>(0.0,
                                                      5.0,
                                                      3000,
                                                      FresnelFunction([](double x) ->double { return cos(x*x); },
                                                                      1E-10));
        printf("%f, %f, %f, %f", sx->maxY(), sx->minY(), cx->maxY(), cx->minY());
        cx->setColor(ukn::color::Blue);
        
        SleepingCurve curve = SleepingCurve({
            Vector2( 3, 0 ),
            Vector2( 4, 0 ),
            Vector2( -6, 0 ),
            Vector2( 2, 0 ),
            Vector2( 4, 30),
            Vector2( 0, 0 ),
            Vector2( 2, 30 ),
            Vector2( 0, 44),
            Vector2( 2, 50),
            Vector2( 2, 30),
            Vector2( 4, 0),
            Vector2( 0, -30),
            Vector2( -6, 0),
            Vector2(4, 0),
            Vector2(3, 30),
            Vector2(3, 0),
            Vector2(2, 30),
            Vector2(3, 30),
            Vector2( -6 , 0),
            Vector2(6, 0),
            Vector2(4, 0),
            Vector2(0, -50),
            Vector2(0, -45),
            Vector2(0, 0),
            Vector2(0, 0),
            Vector2(-2, 0),
            Vector2(0, 0),
            Vector2(0, 0),
            Vector2(0, -50),
            Vector2(2, 22),
            Vector2(3, 30)
        });
        std::cout<<std::endl;
        for(int i=0; i<=31; ++i) {
            std::cout<<"Vector2("<<i+1<<", "<<curve(i) / 60<<"),"<<std::endl;
        }
        
        Spline s = Spline({
            Vector2(1, 3),
            Vector2(2, 4),
            Vector2(3, -6),
            Vector2(4, 2),
            Vector2(5, 4.5),
            Vector2(6, 0),
            Vector2(7, 2.5),
            Vector2(8, 0.733333),
            Vector2(9, 2.83333),
            Vector2(10, 2.5),
            Vector2(11, 4),
            Vector2(12, -0.5),
            Vector2(13, -6),
            Vector2(14, 4),
            Vector2(15, 3.5),
            Vector2(16, 3),
            Vector2(17, 2.5),
            Vector2(18, 3.5),
            Vector2(19, -6),
            Vector2(20, 6),
            Vector2(21, 4),
            Vector2(22, -0.833333),
            Vector2(23, -0.75),
            Vector2(24, 0),
            Vector2(25, 0),
            Vector2(26, -2),
            Vector2(27, 0),
            Vector2(28, 0),
            Vector2(29, -0.833333),
            Vector2(30, 2.36667),
            Vector2(31, 3.5),
            Vector2(32, 0),
        });
        
        
        sleeping = new Graph<std::function<double(double)>>(0.0,
                                                            32,
                                                            30000,
                                                            [=](double x) ->double { return s(x); },
                                                            1.f,
                                                            -400.f);
        sleeping->setColor(ukn::color::Blue);
    }
    
    void onUpdate() {
       
    }
    
    void onRender() {
        ukn::GraphicDevice& gd = ukn::Context::Instance().getGraphicFactory().getGraphicDevice();
        
        gd.clear(ukn::CM_Color | ukn::CM_Depth, ukn::color::Black, 0, 0);
        
        if(r1) lag1->render();
        if(r2) lag2->render();
        if(r3) spline1->render();
        if(r4) spline2->render();
        
        sx->render();
        cx->render();
    //    sleeping->render();
        //runge->render();
    }
    
private:
    Spline s1, s2;
    
    Graph<std::function<double(double)>>* sleeping;
    
    Graph<std::function<float(float)>>* lag1;
    Graph<std::function<float(float)>>* spline1;
    Graph<std::function<float(float)>>* lag2;
    Graph<std::function<float(float)>>* spline2;
    
    Graph<std::function<double(double)>>* sx;
    Graph<std::function<double(double)>>* cx;
    
    
    Graph<std::function<float(float)>>* runge;
    
    ukn::FontPtr mFont;
    
    bool r1, r2, r3, r4, r5;
};

#include "ukn/asset.h"
#include "ukn/LeapMotion.h"
#include "mist/Convert.h"



template<typename T>
struct Vector: public std::vector<T> {
    Vector() {
        
    }
    
    Vector(const std::initializer_list<T>& init):
    std::vector<T>(init) {
        
    }
    
    Vector operator + (const Vector& rhs) const {
        Vector tmp = *this;
        for(size_t i = 0; i < tmp.size(); ++i){
            tmp[i] += rhs[i];
        }
        return tmp;
    }
    Vector operator - (const Vector& rhs) const {
        Vector tmp = *this;
        for(size_t i = 0; i < tmp.size(); ++i){
            tmp[i] -= rhs[i];
        }
        return tmp;
    }
    
    friend Vector operator * (double k, const Vector& rhs) {
        Vector tmp = rhs;
        for(size_t i = 0; i < tmp.size(); ++i){
            tmp[i] *= k;
        }
        return tmp;
    }
    
    friend std::ostream& operator<<(std::ostream& os, const Vector& rhs) {
        os << "[";
        os.precision(6);
        os.setf(std::ios::fixed, std::ios::floatfield);
        
        std::copy(rhs.begin(),
                  rhs.end(),
                  std::ostream_iterator<double>(os, " "));
        os << "]" << std::endl;
        
        return os;
    }
};

typedef std::function< Vector<double>(double, Vector<double> ) > fp3;

std::vector<std::pair<double, Vector<double> > > PredictorCorrectorVX(double x0, double xmax, double h, const Vector<double>& y0, const fp3& f) {
    Vector<double> fval[5];
    fval[0] = f(x0, y0);
    
    double x = x0;
    Vector<double> y = y0;
    
    std::vector<std::pair<double, Vector<double> > > result;
    
    result.push_back(std::make_pair(x, y));
    
    for(int i = 1; i <= 3; ++i) {
        Vector<double> k1 = fval[i-1];
        Vector<double> k2 = f(x + h/2, y + h/2 * k1);
        Vector<double> k3 = f(x + h/2, y + h/2 * k2);
        Vector<double> k4 = f(x + h, y + h * k3);
        
        y = y + h/6 * (k1 + 2 * k2 + 2 * k3 + k4);
        x = x + h;
        
        result.push_back(std::make_pair(x, y));
        fval[i] = f(x, y);
    }
    
    
    while(x < xmax) {
        Vector<double> yp = y + h/24 * (55 * fval[3] - 59 * fval[2] + 37 * fval[1] - 9 * fval[0]);
        x = x + h;
        fval[4] = f(x, yp);
        
        y = y + h/24 * (9 * fval[4] + 19 * fval[3] - 5 * fval[2] + fval[1]);
        result.push_back(std::make_pair(x, y));
        
        for(int i = 0; i < 3; ++i) {
            fval[i] = fval[i+1];
        }
        fval[3] = f(x, y);
    }
    return result;
}

#include "UKN/GraphicContext.h"
#include "mist/Serial.h"

#include "mist/Thread.h"

class TestApp: public ukn::AppInstance, public ukn::input::LeapMotionListener {
public:
    TestApp(const ukn::UknString& name):
    ukn::AppInstance(name) {
  
    }
    virtual ~TestApp() {
        
    }
    
    void onLeapMotionFrame(const Leap::Controller& c) {
        Leap::Frame frame = c.frame(0);
        for(const Leap::Hand& h: frame.hands()) {
            for(const Leap::Finger& f: h.fingers()) {
                ukn::Vertex2D vert;
                vert.x = f.tip().position.x * 3 + getWindow().width() / 2;
                vert.y = f.tip().position.y * 2;
                vert.z = f.tip().position.z;
                vert.color = ukn::ColorHSV(1.0 - 0.2*f.id(), 1.0 - 0.2*f.id(), 0.5, 1.0).toRGBA();
               // vert.color = ukn::color::Skyblue.toRGBA();
                
                mVertexBuffer->push(vert);
            }
        }
        
        
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
        
        if(!mArduinoSerial
           .port("/dev/tty.usbmodemfa141")
           .baudRate(9600)
           .open()) {
            mist::log_error("error opening arduino serial");
        }
    }
    
    void onUpdate() {
        
    }
    
    void onRender() {
        ukn::GraphicDevice& gd = ukn::Context::Instance().getGraphicFactory().getGraphicDevice();
        
        gd.bindFrameBuffer(mFrameBuffer);
        gd.clear(ukn::CM_Color, ukn::Color(0, 0, 0, 1.0), 0, 0);
        
        gd.setRenderState(ukn::RS_PointSize, mist::Convert::ReinterpretConvert<float, ukn::uint32>(4.0));
        gd.setRenderState(ukn::RS_Blend, ukn::RSP_Enable);
        gd.setRenderState(ukn::RS_SrcAlpha, ukn::RSP_BlendFuncOneMinusSrcAlpha);
        gd.setRenderState(ukn::RS_ColorOp, ukn::RSP_ColorOpModulate);
        
        gd.onRenderBuffer(mRenderBuffer);
        
        
        gd.bindFrameBuffer(gd.getScreenFrameBuffer());
        gd.clear(ukn::CM_Color | ukn::CM_Depth, ukn::color::Black, 0, 0);

                
        ukn::SpriteBatch& sb = ukn::SpriteBatch::DefaultObject();
        sb.begin();
        sb.draw(mSquareTexture,
                ukn::Vector2(0, 0));
        sb.end();
        
        
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

#include "LeapTestApp.h"

#include "mist/Query.h"

#ifndef MIST_OS_WINDOWS
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
    
    mist::query::_TestQuery();
    
    mist::thread::Future<double> f = mist::thread::RunAsync<double>([]()->double {
        double result = 0.f;
        for(int i=0; i<100; ++i) {
            result += std::erf(i);
            printf("%f\n", result);
        }
        return result;
    });
    
    f.wait();
    printf("%f\n", f.getResult());
    // register plugins manually for test
   /* ukn::GraphicFactoryPtr gl_factory;
    ukn::CreateGraphicFactory(gl_factory);

    ukn::Context::Instance().registerGraphicFactory(gl_factory);
   
    
     TestApp(L"LeapMotion Test")
        .create(
                ukn::ContextCfg::Default()
                  .width(800)
                  .height(600)
               )
        .run();
    */
    return 0;
}

