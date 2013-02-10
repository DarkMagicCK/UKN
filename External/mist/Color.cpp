#include "mist/Color.h"

namespace mist {

    Color::Color():
        c(1, 1, 1, 1) {}

    Color::Color(int32 _r, int32 _g, int32 _b):
        c(_r / 255.0f, _g / 255.0f,_b / 255.0f, 1) {
            normalize();
    }

    Color::Color(float _r, float _g, float _b, float _a):
        c(_r, _g, _b, _a) {
            normalize();
    }

    Color::Color(const float4& _c):
        c(_c) {

    }

    Color::Color(uint32 col) { 
        *this = col;
    }

    void Color::set(float _r, float _g, float _b, float _a) { 
        c = float4(_r, _g, _b, _a);
    }

    Color Color::operator = (const Color& rhs) {
        this->c = rhs.c;
        return *this;
    }

    Color Color::operator = (uint32 col) {
        this->c = float4(((col & 0xFF))/255.0f, 
                         ((col>>8) & 0xFF)/255.0f,
                         ((col>>16) & 0xFF)/255.0f,
                         ((col>>24) & 0xFF)/255.0f);
        return *this;
    }

    Color& Color::operator += (const Color& rhs) {
        this->c += rhs.c;
        return *this;
    }

    Color& Color::operator -= (const Color& rhs) {
        this->c -= rhs.c; 
        return *this;
    }
    Color& Color::operator *= (const Color& rhs) {
        for(int i=0; i<4; ++i)
            this->c.value[i] *= rhs.c[i];
        return *this;
    }
    Color& Color::operator /= (float s) {
        for(int i=0; i<4; ++i)
            this->c.value[i] /= s;
        return *this;
    }
    Color& Color::operator *= (float s) {
        for(int i=0; i<4; ++i)
            this->c.value[i] *= s;
        return *this; 
    }

    Color Color::operator - (const Color& rhs)	const {
        return Color(this->c - rhs.c);
    }

    Color Color::operator + (float s)	const {
        return Color(this->c + s);
    }
    Color Color::operator - (float s)	const {
        return Color(this->c - s);
    }
    Color Color::operator * (float s)	const {
        return Color(this->c * s);
    }
    Color Color::operator / (float s)	const {
        mist_assert(s != 0.f);
        return Color(this->c / s);
    }
    Color Color::operator * (const Color& rhs) const {
        return Color(this->r() * rhs.r(), 
            this->g() * rhs.g(), 
            this->b() * rhs.b(), 
            this->a() * rhs.a());
    }

    Color Color::operator + (const Color& rhs) const {
        return Color(this->c + rhs.c);
    }

    bool Color::operator == (const Color& rhs)	const {
        return (float4)this->c == rhs.c;
    }
    bool Color::operator != (const Color& rhs) const {
        return !(*this == rhs);
    }

    uint32 Color::toRGBA() const {
        return ((unsigned int)(this->a()*255.0f)) +
            ((unsigned int)(this->b()*255.0f)<<8) +
            ((unsigned int)(this->g()*255.0f)<<16) +
            ((unsigned int)(this->r()*255.0f)<<24);
    }

    uint32 Color::FromRGBA(float oR, float og, float ob, float oa)  {
        return ((unsigned int)(oa*255.0f)) +
            ((unsigned int)(ob*255.0f)<<8) +
            ((unsigned int)(og*255.0f)<<16) +
            ((unsigned int)(oR*255.0f)<<24);
    }

    uint32 Color::toARGB() const {
        return ((unsigned int)(this->a()*255.0f)<<24) +
            ((unsigned int)(this->r()*255.0f)<<16) +
            ((unsigned int)(this->g()*255.0f)<<8) +
            ((unsigned int)(this->b()*255.0f));
    }

    uint32 Color:: FromARGB(float oR, float og, float ob, float oa)  {
        return ((unsigned int)(oa*255.0f)<<24) +
            ((unsigned int)(oR*255.0f)<<16) +
            ((unsigned int)(og*255.0f)<<8) +
            ((unsigned int)(ob*255.0f));
    }

    Color Color::FromHWColor(uint32 col) {
        return Color(col);
    }

    Color& Color::normalize() {
        this->r() = math::clamp(0.f, 1.f, r());
        this->g() = math::clamp(0.f, 1.f, g());
        this->b() = math::clamp(0.f, 1.f, b());
        this->a() = math::clamp(0.f, 1.f, a());
        return *this;
    }

    void ColorHSV::fromRGBA(uint32 col) {
        float r, g, b;
        float minv, maxv, delta;
        float del_R, del_G, del_B;
        
        this->a() = (col & 0xFF) / 255.0f;
        r = ((col>>24) & 0xFF) / 255.0f;
        g = ((col>>16) & 0xFF) / 255.0f;
        b = ((col>>8)  & 0xFF) / 255.0f;
        
        minv = std::min(std::min(r, g), b);
        maxv = std::max(std::max(r, g), b);
        delta = maxv - minv;
        
        this->v() = maxv;
        
        if (delta == 0) {
            this->h() = 0;
            this->s() = 0;
        }
        else {
            this->s() = delta / maxv;
            del_R = (((maxv - r) / 6) + (delta / 2)) / delta;
            del_G = (((maxv - g) / 6) + (delta / 2)) / delta;
            del_B = (((maxv - b) / 6) + (delta / 2)) / delta;
            
            if      (r == maxv) {this->h() = del_B - del_G;}
            else if (g == maxv) {this->h() = (1 / 3) + del_R - del_B;}
            else if (b == maxv) {this->h() = (2 / 3) + del_G - del_R;}
            
            if (this->h() < 0) this->h() += 1;
            if (this->h() > 1) this->h() -= 1;
        }
    }
    
    uint32 ColorHSV::toRGBA() const {
        float r, g, b;
        float xh, i, p1, p2, p3;
        
        if (this->s() == 0) {
            r = this->v();
            g = this->v();
            b = this->v();
        }
        else {
            xh = this->h() * 6;
            if(xh == 6) xh=0;
            i = floorf(xh);
            p1 = this->v() * (1 - this->s());
            p2 = this->v() * (1 - this->s() * (xh - i));
            p3 = this->v() * (1 - this->s() * (1 - (xh - i)));
            
            if      (i == 0) {r = this->v();  g = p3; b = p1;}
            else if (i == 1) {r = p2; g = this->v();  b = p1;}
            else if (i == 2) {r = p1; g = this->v();  b = p3;}
            else if (i == 3) {r = p1; g = p2; b = this->v(); }
            else if (i == 4) {r = p3; g = p1; b = this->v(); }
            else			 {r = this->v();  g = p1; b = p2;}
        }
        
        return (uint32(this->a()*255.0f)) + 
                (uint32(r*255.0f)<<24) + 
                (uint32(g*255.0f)<<16) + 
                (uint32(b*255.0f)<<8);
    }
    

}