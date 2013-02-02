//
//  Math.h
//  Mist
//
//  Created by Robert Bu on 11/22/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef MISTMath_h
#define MISTMath_h

#include "mist/Platform.h"
#include "mist/Preprocessor.h"
#include "mist/Basic.h"

#include "mist/detail/TypeTraits.h"
#include "mist/reflection/TypeName.h"

#include <cmath>

namespace mist {

    const float pi    = 3.14159265358f;
    const float pi_2  = 1.57079632679f;
    const float pi_4  = 0.78539826339f;
    const float pi_6  = 0.5233333f;
    const float pi_12 = 0.2616667f;

    const double d_pi	  =	3.1415926535897932384626433832795;
    const double d_pi_2   = 1.5707963267948966192313216916398;
    const double d_pi_4   = 0.7853981633974483096156608458198;
    const double d_pi_8   = 0.3926990816987241548078304229099;

#ifndef MIST_TINY
#define MIST_TINY 0.00000001f
#endif

#define MIST_MIN(a, b) (a) < (b) ? (a) : (b)
#define MIST_MAX(a, b) (a) > (b) ? (a) : (b)
#define MIST_ABS(a) (a) < 0 ? -(a) : (a)

    namespace math {

        MIST_API real degree_to_radius(real dgr);
        MIST_API real radius_to_degree(real rad);
        MIST_API real sin_dgr(real dgr);
        MIST_API real cos_dgr(real dgr);
        MIST_API real tan_dgr(real dgr);
        MIST_API bool float_equal(real f1, real f2);
        // fast inv sqrt by Join Carmack
        MIST_API real fast_inv_sqrt(real x);

        MIST_API int is_pow_of_2(uint32 x);

        MIST_API uint32 next_pow_of_2(uint32 x);

        // compress a unit float to nBits integer
        // there would be some data loss but good for some condition
        // it is safe to cast the result to nBits integer, such as uint16 if you pass nBits as 16
        MIST_API uint32 compress_unit_float(real unitFloat, uint32 nBits);
        // decompress a unit float from a quantized uint
        MIST_API real decompress_unit_float(uint32 quantized, uint32 nBits);
        // compress a float within a specific range
        MIST_API uint32 compress_float(real value, real min, real max, uint32 nBits);
        MIST_API real decompress_float(uint32 quantized, real min, real max, uint32 nBits);

        MIST_API uint16 flip_bytes(uint16 value);
        MIST_API uint32 flip_bytes(uint32 value);

        MIST_API int16 flip_bytes(int16 value);
        MIST_API int32 flip_bytes(int32 value);

        template <typename T>
        inline T lerp(T t1, T t2, real t) {
            if(t < 0.f) t = 0.f;
            else if(t > 1.f) t = 1.f;
            return t1 + (t2-t1) * t;
        }

        template<typename T>
        inline T slerp(T t1, T t2, real t) {
            if(t < 0.f) t = 0.f;
            else if(t > 1.f) t = 1.f;

            float a = acos(t1 * t2);
            float sina = sin(a);
            return (sin(1-t) * a) / sina * t1 + sin(t * a) / sina * t2;
        }

        template<typename T>
        inline T clamp(T t1, T t2, real v) {
            if(v > t2) return t2;
            else if(v < t1) return t1;
            return v;
        }
    }
    // represents a value range
    template<typename T>
    struct Range {
        Range() {

        }

        Range(const Range& rhs):
            mBegin(rhs.mBegin),
            mEnd(rhs.mEnd) {

        }

        Range(T begin, T end):
            mBegin(begin),
            mEnd(end) {

        }

        void set(T begin, T end) {
            mBegin = begin;
            mEnd = end;
        }

        T& begin() { return mBegin; }
        T& end() { return mEnd; }

        const T& begin() const { return mBegin; }
        const T& end()   const { return mEnd; }

        Range operator + (const Range& rhs) const {
            return Range(this->mBegin + rhs.mBegin,
                this->mEnd + rhs.mEnd);
        }

        Range& operator += (const Range& rhs) {
            this->mBegin += rhs.mBegin;
            this->mEnd += rhs.mEnd;
            return *this;
        }

        Range operator - (const Range& rhs) const {
            return Range(this->mBegin - rhs.mBegin,
                this->mEnd - rhs.mEnd);
        }

        Range& operator -= (const Range& rhs) {
            this->mBegin -= rhs.mBegin;
            this->mEnd -= rhs.mEnd;
            return *this;
        }

        Range operator + (T value) const {
            return Range(this->mBegin + value,
                this->mEnd + value);
        }

        Range& operator += (T value) {
            this->mBegin += value;
            this->mEnd += value;
            return *this;
        }

        Range& operator -= (T value) {
            this->mBegin -= value;
            this->mEnd -= value;
            return *this;
        }

        Range operator * (float value) const {
            return Range(this->mBegin * value,
                this->mEnd * value);
        }

        Range operator - (T value) const {
            return Range(this->mBegin - value,
                this->mEnd - value);
        }

        bool operator == (const Range& rhs) const {
            return this->mBegin == rhs.mBegin &&
                this->mEnd == rhs.mEnd;
        }

    private:
        T mBegin;
        T mEnd;
    };


    template<int Count, typename Type>
    struct PointTemplate {

        typedef PointTemplate<Count, Type> SelfType;
        typedef Type PointType;

        enum {
            Size = Count,
        };

        mist_static_assert(Count > 1);

        template<typename T2>
        SelfType operator+(const PointTemplate<Count, T2>& rhs) const {
            SelfType tmp = *this;
            for(int i=0; i<Size; ++i) {
                tmp.value[i] += Type(rhs.value[i]);
            }
            return tmp;
        }

        template<typename T2>
        SelfType operator-(const PointTemplate<Count, T2>& rhs) const {
            SelfType tmp = *this;
            for(int i=0; i<Size; ++i) {
                tmp.value[i] -= Type(rhs.value[i]);
            }
            return tmp;
        }

        template<typename T2>
        SelfType& operator=(const PointTemplate<Count, T2>& rhs) {
            for(int i=0; i<Size; ++i) {
                this->value[i] = Type(rhs.value[i]);
            }
            return *this;
        }

        template<typename T2>
        bool operator==(const PointTemplate<Count, T2>& rhs) {
            for(int i=0; i<Size; ++i) {
                if(this->value[i] != Type(rhs.value[i]))
                    return false;
            }
            return true;
        }

        template<typename T2>
        bool operator!=(const PointTemplate<Count, T2>& rhs) {
            return !(*this == rhs);
        }

        template<typename T2>
        SelfType& operator+=(const PointTemplate<Count, T2>& rhs) {
            for(int i=0; i<Size; ++i) {
                this->value[i] += Type(rhs.value[i]);
            }
            return *this;
        }

        template<typename T2>
        SelfType& operator-=(const PointTemplate<Count, T2>& rhs) {
            for(int i=0; i<Size; ++i) {
                this->value[i] -= Type(rhs.value[i]);
            }
            return *this;
        }

        Type& operator[](uint32 index) {
            mist_assert(index >= 0 && index < Size);
            return this->value[index];
        }

        const Type& operator[](uint32 index) const {
            mist_assert(index >= 0 && index < Size);
            return this->value[index];
        }


        Type value[Count];
    };


#define MIST_POINT_TEMP_PARAM(n, d) d MIST_CAT(value, n)
#define MIST_POINT_TEMP_PARAM_ARGS(n, d) MIST_ENUM_N(n, MIST_POINT_TEMP_PARAM, d)
#define MIST_POINT_PARAM_EXPAND(n, d) d[n] = d##n;

#define DEF_POINT_TEMPLATE_N(n, Type) \
    template<>                                         \
    struct MIST_API PointTemplate<n, Type> {                             \
    typedef PointTemplate<n, Type> SelfType;                \
    typedef Type PointType;                                     \
    enum {                                                      \
    Size = n,                                         \
    };                                                          \
    \
    Type value[Size];\
    PointTemplate() {                                       \
    }                                                           \
    PointTemplate(MIST_POINT_TEMP_PARAM_ARGS(n, Type)) {     \
    MIST_REPEAT_N(n, MIST_POINT_PARAM_EXPAND, value)        \
    } \
    \
    SelfType operator -() const { \
    return this->negate(); \
    }\
    SelfType negate() const {\
    SelfType tmp = *this;                                       \
    for(int i=0; i<Size; ++i) {                           \
    tmp.value[i] = -this->value[i];        \
    }                                                           \
    return tmp;\
    }\
    template<typename T2>                                           \
    SelfType operator+(const PointTemplate<n, T2>& rhs) const { \
    SelfType tmp = *this;                                       \
    for(int i=0; i<Size; ++i) {                           \
    tmp.value[i] += Type(rhs.value[i]);        \
    }                                                           \
    return tmp;                                                 \
    }                                                               \
    template<typename T2>                                           \
    SelfType operator-(const PointTemplate<n, T2>& rhs) const { \
    SelfType tmp = *this;                                       \
    for(int i=0; i<Size; ++i) {                           \
    tmp.value[i] -= Type(rhs.value[i]);        \
    }\
    return tmp;\
    }\
    template<typename T2>\
    SelfType& operator=(const PointTemplate<n, T2>& rhs) {\
    for(int i=0; i<Size; ++i) {\
    this->value[i] = Type(rhs.value[i]);\
    }\
    return *this;\
    }\
    template<typename T2>\
    bool operator==(const PointTemplate<n, T2>& rhs) {\
       return this->equals(rhs); \
    }\
    template<typename T2> \
    bool equals(const PointTemplate<n, T2> &rhs) const {\
    for(int i=0; i<Size; ++i) {\
    if(this->value[i] != Type(rhs.value[i]))\
    return false;\
    }\
    return true;\
    }\
    template<typename T2>\
    bool operator!=(const PointTemplate<n, T2>& rhs) {\
    return !(*this == rhs);\
    }\
    template<typename T2>\
    SelfType& operator+=(const PointTemplate<n, T2>& rhs) {\
    for(int i=0; i<Size; ++i) {\
    this->value[i] += Type(rhs.value[i]);\
    }\
    return *this;\
    }\
    template<typename T2>\
    SelfType& operator-=(const PointTemplate<n, T2>& rhs) {\
    for(int i=0; i<Size; ++i) {\
    this->value[i] -= Type(rhs.value[i]);\
    }\
    return *this;\
    }\
    Type& operator[](int index) {\
    mist_assert(index >= 0 && index < Size);\
    return this->value[index];\
    }\
    const Type& operator[](int index) const {\
    mist_assert(index >= 0 && index < Size);\
    return this->value[index];\
    }\
    SelfType operator*(real r) const {\
    SelfType tmp = *this;\
    for(int i=0; i<Size; ++i) {\
    tmp.value[i] = Type(tmp.value[i] * r);\
    }\
    return tmp;\
    }\
    SelfType operator/(real r) const {\
    SelfType tmp = *this;\
    for(int i=0; i<Size; ++i) {\
    tmp.value[i] = Type(tmp.value[i] / r);\
    }\
    return tmp;\
    }\
    SelfType& operator*=(real r) {\
    for(int i=0; i<Size; ++i) {\
    this->value[i] = Type(this->value[i] * r);\
    }\
    return *this;\
    }\
    SelfType& operator/=(real r) {\
    for(int i=0; i<Size; ++i) {\
    this->value[i] = Type(this->value[i] / r);\
    }\
    return *this;\
    }\
    \
    Type dot(const SelfType& vec) const {\
    Type result(0);\
    for(int i=0; i<Size; ++i)\
    result += this->value[i] * vec.value[i];\
    return result;\
    }\
    Type sqrLength() const {\
    return Type(this->dot(*this));\
    }\
    \
    Type length() const {\
    return Type(sqrt(sqrLength()));\
    }\
    SelfType normalize() const {\
    real invLength = 1.f / length();\
    SelfType result;\
    for(int i=0; i<Size; ++i)\
    result.value[i] = Type(this->value[i] * invLength);\
    return result;\
    }\
    \
    };

    DEF_POINT_TEMPLATE_N(1, float);
    DEF_POINT_TEMPLATE_N(2, float);
    DEF_POINT_TEMPLATE_N(3, float);
    DEF_POINT_TEMPLATE_N(4, float);

    DEF_POINT_TEMPLATE_N(1, int32);
    DEF_POINT_TEMPLATE_N(2, int32);
    DEF_POINT_TEMPLATE_N(3, int32);
    DEF_POINT_TEMPLATE_N(4, int32);

#undef DEF_POINT_TEMPLATE_N
#undef MIST_POINT_TEMP_PARAM
#undef MIST_POINT_TEMP_PARAM_ARGS
#undef MIST_POINT_PARAM_EXPAND

    template<typename T>
    class Vector2_T: public PointTemplate<2, T> {
    public:
        Vector2_T(const T& _x, const T& _y) {
            this->set(_x, _y);
        }

        Vector2_T() {

        }

        Vector2_T(const Vector2_T& rhs) {
            this->set(rhs.x(), rhs.y());
        }

        Vector2_T(const PointTemplate<2, T>& rhs) {
            this->set(rhs.value[0], rhs.value[1]);
        }

        T x() const { return this->value[0]; }
        T y() const { return this->value[1]; }
        T& x() { return this->value[0]; }
        T& y() { return this->value[1]; }

        void set(const T& _x, const T& _y) {
            this->value[0] = _x;
            this->value[1] = _y;
        }

        T angle(const Vector2_T* rhs = 0) const {
            if(rhs) {
                Vector2_T s = *this, t = *rhs;
                s.normalize();
                t.normalize();
                return acosf(s.dot(t));
            } else
                return atan2f(this->y(), this->x());
        }

        void clamp(const T max) {
            if(this->length() > max) {
                this->normalize();
                this->x() *= max;
                this->y() *= max;
            }
        }

        Vector2_T& rotate(T a) {
            Vector2_T v;
            v.x = T(this->x() * cos(a) - this->y() * sin(a));
            v.y = T(this->x() * sin(a) + this->y() * cos(a));
            this->x() = v.x; this->y() = v.y;
            return *this;
        }

        Vector2_T normal(const Vector2_T& rhs) const {
            Vector2_T normal;

            normal.x = T(this->y() - rhs.y());
            normal.y = T(rhs.x() - this->x());
            real len = sqrt(normal.x * normal.x + normal.y * normal.y);
            if(len == 0.f) {
                return Vector2_T();
            }

            normal.x /= len;
            normal.y /= len;

            return normal;
        }

        Vector2_T& operator=(const PointTemplate<2, T>& rhs) {
            this->value[0] = rhs.value[0];
            this->value[1] = rhs.value[1];
            return *this;
        }

        friend Vector2_T operator * (const Vector2_T& lhs, const Vector2_T& rhs) {
            return Vector2_T(lhs.x() * rhs.x(), lhs.y() * rhs.y());
        }
    };
    typedef Vector2_T<float> Vector2;

    template<typename T>
    class Vector3_T: public PointTemplate<3, T> {
    public:		
        Vector3_T() {
        }

        Vector3_T(const T& _x, const T& _y, const T& _z) {
            this->set(_x, _y, _z);
        }

        Vector3_T(const Vector3_T& rhs) {
            this->set(rhs.x(), rhs.y(), rhs.z());
        }


        Vector3_T(const PointTemplate<3, T>& rhs) {
            this->set(rhs.value[0], rhs.value[1], rhs.value[2]);
        }

        void set(const T& _x, const T& _y, const T& _z) {
            this->value[0] = _x;
            this->value[1] = _y;
            this->value[2] = _z;
        }

        T x() const { return this->value[0]; }
        T y() const { return this->value[1]; }
        T z() const { return this->value[2]; }
        T& x() { return this->value[0]; }
        T& y() { return this->value[1]; }
        T& z() { return this->value[2]; }

        Vector3_T cross(const Vector3_T& rhs) const {
            return Vector3_T(-this->z()*rhs.y() + this->y()*rhs.z(), 
                this->z()*rhs.x() - this->x()*rhs.z(), 
                -this->y()*rhs.x() + this->x()*rhs.y());
        }

        Vector3_T& operator=(const PointTemplate<3, T>& rhs) {
            this->value[0] = rhs.value[0];
            this->value[1] = rhs.value[1];
            this->value[2] = rhs.value[2];
            return *this;
        }

        friend Vector3_T operator *(const Vector3_T& lhs, const Vector3_T& rhs) {
            return Vector3_T(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z);
        }
    };
    typedef Vector3_T<float> Vector3;

    template<typename T>
    class Vector4_T: public PointTemplate<4, T> {
    public:
        Vector4_T() {
        }

        Vector4_T(const T& _x, const T& _y, const T& _z, const T& _w) {
            this->set(_x, _y, _z, _w);
        }

        Vector4_T(const Vector3_T<T>& v) {
            this->set(v.x(), v.y(), v.z(), 1.f);
        }

        Vector4_T(const PointTemplate<4, T>& rhs) {
            this->set(rhs.value[0], rhs.value[1], rhs.value[2], rhs.value[3]);
        }

        Vector4_T& operator=(const PointTemplate<4, T>& rhs) {
            this->set(rhs.value[0], rhs.value[1], rhs.value[2], rhs.value[3]);
            return *this;
        }

        T x() const { return this->value[0]; }
        T y() const { return this->value[1]; }
        T z() const { return this->value[2]; }
        T w() const { return this->value[3]; }
        T& x() { return this->value[0]; }
        T& y() { return this->value[1]; }
        T& z() { return this->value[2]; }
        T& w() { return this->value[3]; }

        void set(const T& _x, const T& _y, const T& _z, const T& _w) {
            this->value[0] = _x;
            this->value[1] = _y;
            this->value[2] = _z;
            this->value[3] = _w;
        }
    };
    typedef Vector4_T<float> Vector4;

    class AABB2 {
    public:
        real x1, y1;
        real x2, y2;

        AABB2(real _x1, real _y1, real _x2, real _y2, bool asWH=false):
            x1(_x1), y1(_y1),
            x2(asWH ? _x1 + _x2: _x2), y2(asWH ? _y1 + _y2 : _y2) {

        }

        AABB2():
            x1(0), y1(0),
            x2(0), y2(0) {   
        }

        AABB2(const Vector2& upper, const Vector2& lower):
            x1(upper.x()), y1(upper.y()),
            x2(lower.x()), y2(lower.y()) {

        }

        AABB2(const Vector2& pos, real r) {
            setAsBox(pos.x(), pos.y(), r);
        }

        AABB2(const AABB2& rhs) {
            this->x1 = rhs.x1;
            this->y1 = rhs.y1;
            this->x2 = rhs.x2;
            this->y2 = rhs.y2;
        }

        void set(const Vector2& upper, const Vector2& lower) {
            this->x1 = upper.x();
            this->y1 = upper.y();
            this->x2 = lower.x();
            this->y2 = lower.y();
        }

        void setXY(real _x1, real _y1) {
            this->x2 = _x1 + this->width();
            this->y2 = _y1 + this->height();
            this->x1 = _x1;
            this->y1 = _y1;
        }

        void set(real _x1, real _y1, real _x2, real _y2, bool asWH=false) {
            this->x1 = _x1; 
            this->y1 = _y1;
            this->x2 = asWH ? _x1 + _x2 : _x2;
            this->y2 = asWH ? _y1 + _y2 : _y2;
        }

        void setAsBox(real x, real y, real r) {
            this->x1 = x - r;
            this->y1 = y - r;
            this->x2 = x + r;
            this->y2 = y + r;
        }

        AABB2& operator=(const AABB2& rhs) {
            if(this != &rhs) {
                this->x1 = rhs.x1;
                this->y1 = rhs.y1;
                this->x2 = rhs.x2;
                this->y2 = rhs.y2;
            }
            return *this;
        }

        real width() const {
            return this->x2 - this->x1;
        }

        real height() const {
            return this->y2 - this->y1;
        }

        Vector2 upper() const {
            return Vector2(this->x1, this->y1);
        }

        Vector2 lower() const {
            return Vector2(this->x2, this->y2);
        }

        real right() const {
            return this->x2;
        }

        real bottom() const {
            return this->y2;
        }

        real left() const {
            return this->x1;
        }

        real top() const {
            return this->y1;
        }

        real x() const {
            return this->x1;
        }

        real y() const {
            return this->y1;
        }

        void encapsulate(real x, real y) {
            if(x<this->x1) this->x1 = x;
            if(x>this->x2) this->x2 = x;
            if(y<this->y1) this->y1 = y;
            if(y>this->y2) this->y2 = y;
        }

        bool contains(real x, real y) const {
            return x>=x1 && x<=x2 && y>=y1 && y<=y2;
        }

        bool contains(const Vector2& pos) const {
            return contains(pos.x(), pos.y());
        }

        bool intersect(const AABB2& rhs) const {
            if(fabs(this->x1 + this->x2 - rhs.x1 - rhs.x2) < (this->x2 - this->x1 + rhs.x2 - rhs.x1)) {
                if(fabs(this->y1 + this->y2 - rhs.y1 - rhs.y2) < (this->y2 - this->y1 + rhs.y2 - rhs.y1)) {
                    return true;
                }
            }
            return false;
        }

        bool isEmpty() const {
            return (x1 == x2) && (y1 == y2);
        }

        Vector2 getPosition() const {
            return Vector2((this->x1 + this->x2)/2,
                (this->y1 + this->y2)/2);
        }
    };

    typedef AABB2 Rectangle;

    class Quaternion {
    public:
        Quaternion(real _x, real _y, real _z, real _w):
            w(_w), x(_x), y(_y), z(_z) {
        }

        Quaternion(const Quaternion& quad):
            w(quad.w), x(quad.x), y(quad.y), z(quad.z) {
        }

        Quaternion():
            w(0.f), x(0.f), y(0.f), z(0.f) {
        }

        Quaternion(real eulerX, real eulerY, real eulerZ) {
            Quaternion roll( sinf( eulerX * 0.5f ), 0, 0, cosf( eulerX * 0.5f ) );
            Quaternion pitch( 0, sinf( eulerY * 0.5f ), 0, cosf( eulerY * 0.5f ) );
            Quaternion yaw( 0, 0, sinf( eulerZ * 0.5f ), cosf( eulerZ * 0.5f ) );

            // Order: y * x * z
            *this = pitch * roll * yaw;
        }

        void set(real _x, real _y, real _z, real _w) {
            this->x = _x; 
            this->y = _y; 
            this->z = _z; 
            this->w = _w;
        }

        void set(const Quaternion& quad) {
            this->x = quad.x; 
            this->y = quad.y; 
            this->z = quad.z; 
            this->w = quad.w;
        }

        Vector3 get() const {
            return Vector3(x, y, z);
        }

        float length() const {
            return sqrt(x * x + y * y + z * z + w * w);
        }

        inline Quaternion& operator=(const Quaternion& rhs) {
            this->x = rhs.x; 
            this->y = rhs.y;
            this->z = rhs.z; 
            this->w = rhs.w;
            return *this;
        }

        inline Quaternion operator*(const Quaternion& rhs) {
            Quaternion ans;
            real d1, d2, d3, d4;
            d1 = this->w*rhs.w;
            d2 = -this->x*rhs.x;
            d3 = -this->y*rhs.y;
            d4 = -this->z*rhs.z;
            ans.w = d1+d2+d3+d4;

            d1 = this->w*rhs.x;
            d2 = this->x*rhs.w;
            d3 = this->y*rhs.z;
            d4 = -this->z*rhs.y;
            ans.x = d1+d2+d3+d4;

            d1 = this->w*rhs.y;
            d2 = this->y*rhs.w;
            d3 = this->z*rhs.x;
            d4 = -this->x*rhs.z;
            ans.y = d1+d2+d3+d4;

            d1 = this->w*rhs.z;
            d2 = this->z*rhs.w;
            d3 = this->x*rhs.y;
            d4 = -this->y*rhs.x;
            ans.z = d1+d2+d3+d4;

            *this = ans;
            return *this;
        }

        void makeRotate(real angle, real _x, real _y, real _z) {
            real inversenorm = (real)1.0f/sqrtf(x*x + y*y + z*z);
            real coshalfangle = cosf(0.5f*angle);
            real sinhalfangle = sinf(0.5f*angle);

            this->x = _x * sinhalfangle * inversenorm;
            this->y = _y * sinhalfangle * inversenorm;
            this->z = _z * sinhalfangle * inversenorm;
            this->w = coshalfangle;
        }

        void makeRotate(real angle, const Vector3& vec) {
            makeRotate(angle, vec.x(), vec.y(), vec.z());
        }

        void makeRotate(real roll, real pitch, real yaw) {
            real sh0 = sinf(0.5f*roll);
            real ch0 = cosf(0.5f*roll);
            real sh1 = sinf(0.5f*pitch);
            real ch1 = cosf(0.5f*pitch);
            real sh2 = sinf(0.5f*yaw);
            real ch2 = cosf(0.5f*yaw);

            this->x = sh0*ch1*ch2 + ch0*sh1*sh2;
            this->y = ch0*sh2*ch1 + sh0*ch2*sh1;
            this->z = ch0*ch2*sh1 - sh0*sh2*ch1;
            this->w = ch0*ch1*ch2 - sh0*sh1*sh2;
        }

        Vector3 rotate(const Vector3& v) const {
            real ux = this->w*v.x() + this->y*v.z() - this->z*v.y();
            real uy = this->w*v.y() + this->z*v.x() - this->x*v.z();
            real uz = this->w*v.z() + this->x*v.y() - this->y*v.x();
            real uw = -this->x*v.x() - this->y*v.y() - this->z*v.z();
            real vx = -uw*this->x + ux*this->w - uy*this->z + uz*this->y;
            real vy = -uw*this->y + uy*this->w - uz*this->x + ux*this->z;
            real vz = -uw*this->z + uz*this->w - ux*this->y + uz*this->x;
            return Vector3(vx, vy, vz);
        }

        void getRotate(real& angle, real& _x, real& _y, real& _z) const {
            real sinhalfangle = sqrtf(this->x*this->x + this->y*this->y + this->z*this->z);
            angle = 2.f * atan2f(sinhalfangle, this->w);
            if(sinhalfangle) {
                _x = x / sinhalfangle;
                _y = y / sinhalfangle;
                _z = z / sinhalfangle;
            } else {
                _x = 0.0f;
                _y = 0.0f;
                _z = 0.0f;
            }
        }

        void getRotate(real& angle, Vector3& vec) const {
            getRotate(angle, vec.x(), vec.y(), vec.z());
        }

        void slerp(real t, const Quaternion& from, const Quaternion& to) {
            const real epsilon = 0.00001f;
            real cosomega, scale_from, scale_to;

            Quaternion quatTo(to);

            cosomega = from.x*to.x + from.y*to.y + from.z*to.z + from.w*to.w;
            if(cosomega < 0.0f) {
                cosomega = -cosomega;
                quatTo.set(-to.x, -to.y, -to.z, -to.w);
            }

            if((1.0 - cosomega) > epsilon) {
                real omega = acosf(cosomega);
                real invsinomega = 1.0f/sinf(omega);
                scale_from = sinf((1.0f-t)*omega)*invsinomega;
                scale_to = sinf(t*omega)*invsinomega;
            } else {
                scale_from = 1.0f - t;
                scale_to = t;
            }

            this->x = from.x*scale_from + quatTo.x*scale_to;
            this->y = from.y*scale_from + quatTo.y*scale_to;
            this->z = from.z*scale_from + quatTo.z*scale_to;
            this->w = from.w*scale_from + quatTo.w*scale_to;
        }

        real w, x, y, z;
    };

    class Matrix4 {
    public:
        union {
            real c[4][4];	// Column major order for OpenGL: c[column][row]
            real x[16];
        };

        static Matrix4 TransMat( real x, real y, real z ) {
            Matrix4 m;

            m.c[3][0] = x;
            m.c[3][1] = y;
            m.c[3][2] = z;

            return m;
        }

        static Matrix4 ScaleMat( real x, real y, real z ) {
            Matrix4 m;

            m.c[0][0] = x;
            m.c[1][1] = y;
            m.c[2][2] = z;

            return m;
        }

        static Matrix4 RotMat( real x, real y, real z ) {
            // Rotation order: YXZ [* Vector]
            return Matrix4( Quaternion( x, y, z ) );
        }

        static Matrix4 RotMat( Vector3 axis, real angle ) {
            axis = axis * sinf( angle * 0.5f );
            return Matrix4( Quaternion( axis.x(), axis.y(), axis.z(), cosf( angle * 0.5f ) ) );
        }

        static Matrix4 RotX(real x) {
            real sx = sin(x), cx = cos(x);

            Matrix4 m;
            m.c[1][1] = cx;
            m.c[1][2] = -sx;
            m.c[2][1] = sx;
            m.c[2][2] = cx;
            return m;
        }

        static Matrix4 RotY(real y) {
            real sy = sin(y), cy = cos(y);

            Matrix4 m;
            m.c[0][0] = cy;
            m.c[0][2] = sy;
            m.c[3][0] = -sy;
            m.c[3][2] = cy;
            return m;
        }

        static Matrix4 RotZ(real z) {
            real sz = sin(z), cz = cos(z);

            Matrix4 m;
            m.c[0][0] = cz;
            m.c[0][1] = -sz;
            m.c[1][0] = sz;
            m.c[1][1] = cz;
            return m;
        }

        static Matrix4 LHToRH(const Matrix4& mat) {
            Matrix4 result = mat;
            result.c[0][2] = -result.c[0][2];
            result.c[1][2] = -result.c[1][2];
            result.c[2][2] = -result.c[2][2];
            result.c[3][2] = -result.c[3][2];
            return result;
        }

        static Matrix4 PerspectiveFovMatLH(real field_of_view, real aspect, real n, real f) {
            Matrix4 m;
            real ff = 1.0f / tan(field_of_view / 2.0f);
            real q = f / (f - n);
            m.c[0][0] = ff / aspect;
            m.c[1][1] = ff;
            m.c[2][2] = q;
            m.c[2][3] = -q * n;
            m.c[3][2] = 1.f;
            m.c[3][3] = 0;
            return m;
        }

        static Matrix4 PerspectiveFovMatRH(real field_of_view, real aspect, real n, real f) {
            return LHToRH(PerspectiveFovMatLH(field_of_view, aspect, n, f));
        }

        static Matrix4 PerspectiveMatLH(real width, real height, real n, real f) {
            Matrix4 m;
            real n2 = n + f;
            real q = f / (f - n);
            m.c[0][0] = n2 / width;
            m.c[1][1] = n2 / height;
            m.c[2][2] = q;
            m.c[2][3] = -q * n;
            m.c[3][2] = 1.f;
            m.c[3][3] = 0;
            return m;
        }

        static Matrix4 PerspectiveMatRH(real width, real height, real n, real f) {
            return LHToRH(PerspectiveMatLH(width, height, n, f ));
        }

        static Matrix4 PerspectiveOffCenterMatLH(real l, real r, real b, real t, real n, real f) {
            Matrix4 m;
            real n2 = n + f;
            real q = f / (f - n);
            real invW = 1 / (r-l);
            real invH = 1 / (t-b);
            m.c[0][0] = n2 * invW;
            m.c[0][2] = -(l + r) * invW;
            m.c[1][1] = n2 * invH;
            m.c[1][2] = -(t + b) * invH; 
            m.c[2][2] = q;
            m.c[2][3] = -q * n;
            m.c[3][2] = 1.f;
            m.c[3][3] = 0;
            return m;
        }

        static Matrix4 PerspectiveOffCenterMatRH(real l, real r, real b, real t, real n, real f) {
            return LHToRH(PerspectiveOffCenterMatLH(l, r, b, t, n, f));
        }

        static Matrix4 OrthoMatLH(real w, real h, real n, real f) {
            return OrthoOffCenterMatLH(-w/2, w/2, -h/2, h/2, n, f);
        }

        static Matrix4 OrthoMatRH(real w, real h, real n, real f) {
            return LHToRH(OrthoMatLH(w, h, n, f));
        }

        static Matrix4 OrthoOffCenterMatLH(real l, real r, real b, real t, real n, real f ) {
            Matrix4 m;

            m.c[0][0] = 2 / (r - l);
            m.c[1][1] = 2 / (t - b);
            m.c[2][2] = 1 / (f - n);
            m.c[0][3] = -(r + l) / (r - l);
            m.c[1][3] = -(t + b) / (t - b);
            m.c[2][3] = -(n) / (f - n);

            return m;
        }

        static Matrix4 OrthoOffCenterMatRH(real l, real r, real b, real t, real n, real f) {
            return LHToRH(OrthoOffCenterMatLH(l , r, b, t, n, f));
        }

        static Matrix4 LookAtMatLH(const Vector3& eye, const Vector3& lookAt, const Vector3& up) {
            Vector3 z = (lookAt - eye).normalize();
            Vector3 x = up.cross(z).normalize();
            Vector3 y = z.cross(x);

            Matrix4 mat;
            mat.c[0][0] = x.x();
            mat.c[1][0] = y.x();
            mat.c[2][0] = z.x();
            mat.c[3][0] = 0;

            mat.c[0][1] = x.y();
            mat.c[1][1] = y.y();
            mat.c[2][1] = z.y();
            mat.c[3][1] = 0;

            mat.c[0][2] = x.z();
            mat.c[1][2] = y.z();
            mat.c[2][2] = z.z();
            mat.c[3][2] = 0;

            mat.c[0][3] = -x.dot(eye);
            mat.c[1][3] = -y.dot(eye);
            mat.c[2][3] = -z.dot(eye);
            mat.c[3][3] = 1;

            return mat;
        }

        static Matrix4 LookAtMatRH(const Vector3& eye, const Vector3& lookAt, const Vector3& up) {
            return LHToRH(LookAtMatLH(eye, lookAt, up));
        }

        static Matrix4 FastMult43( const Matrix4 &m1, const Matrix4 &m2 ) {
            // Note: dst may not be the same as m1 or m2
            Matrix4 dst;

            real *dstx = dst.x;
            const real *m1x = m1.x;
            const real *m2x = m2.x;

            dstx[0] = m1x[0] * m2x[0] + m1x[4] * m2x[1] + m1x[8] * m2x[2];
            dstx[1] = m1x[1] * m2x[0] + m1x[5] * m2x[1] + m1x[9] * m2x[2];
            dstx[2] = m1x[2] * m2x[0] + m1x[6] * m2x[1] + m1x[10] * m2x[2];
            dstx[3] = 0.0f;

            dstx[4] = m1x[0] * m2x[4] + m1x[4] * m2x[5] + m1x[8] * m2x[6];
            dstx[5] = m1x[1] * m2x[4] + m1x[5] * m2x[5] + m1x[9] * m2x[6];
            dstx[6] = m1x[2] * m2x[4] + m1x[6] * m2x[5] + m1x[10] * m2x[6];
            dstx[7] = 0.0f;

            dstx[8] = m1x[0] * m2x[8] + m1x[4] * m2x[9] + m1x[8] * m2x[10];
            dstx[9] = m1x[1] * m2x[8] + m1x[5] * m2x[9] + m1x[9] * m2x[10];
            dstx[10] = m1x[2] * m2x[8] + m1x[6] * m2x[9] + m1x[10] * m2x[10];
            dstx[11] = 0.0f;

            dstx[12] = m1x[0] * m2x[12] + m1x[4] * m2x[13] + m1x[8] * m2x[14] + m1x[12] * m2x[15];
            dstx[13] = m1x[1] * m2x[12] + m1x[5] * m2x[13] + m1x[9] * m2x[14] + m1x[13] * m2x[15];
            dstx[14] = m1x[2] * m2x[12] + m1x[6] * m2x[13] + m1x[10] * m2x[14] + m1x[14] * m2x[15];
            dstx[15] = 1.0f;

            return dst;
        }

        Matrix4() {
            c[0][0] = 1.f; c[1][0] = 0.f; c[2][0] = 0.f; c[3][0] = 0.f;
            c[0][1] = 0.f; c[1][1] = 1.f; c[2][1] = 0.f; c[3][1] = 0.f;
            c[0][2] = 0.f; c[1][2] = 0.f; c[2][2] = 1.f; c[3][2] = 0.f;
            c[0][3] = 0.f; c[1][3] = 0.f; c[2][3] = 0.f; c[3][3] = 1.f;
        }

        Matrix4( const real *realArray16 ) {
            for( unsigned int i = 0; i < 4; ++i ) {
                for( unsigned int j = 0; j < 4; ++j ) {
                    this->c[i][j] = realArray16[i * 4 + j];
                }
            }
        }

        Matrix4( const Quaternion &q ) {
            // Calculate coefficients
            real x2 = q.x + q.x, y2 = q.y + q.y, z2 = q.z + q.z;
            real xx = q.x * x2,  xy = q.x * y2,  xz = q.x * z2;
            real yy = q.y * y2,  yz = q.y * z2,  zz = q.z * z2;
            real wx = q.w * x2,  wy = q.w * y2,  wz = q.w * z2;

            this->c[0][0] = 1 - (yy + zz);  this->c[1][0] = xy - wz;	
            this->c[2][0] = xz + wy;        this->c[3][0] = 0;
            this->c[0][1] = xy + wz;        this->c[1][1] = 1 - (xx + zz);
            this->c[2][1] = yz - wx;        this->c[3][1] = 0;
            this->c[0][2] = xz - wy;        this->c[1][2] = yz + wx;
            this->c[2][2] = 1 - (xx + yy);  this->c[3][2] = 0;
            this->c[0][3] = 0;              this->c[1][3] = 0;
            this->c[2][3] = 0;              this->c[3][3] = 1;
        }

        Matrix4(real x11, real x12, real x13, real x14,
                real x21, real x22, real x23, real x24,
                real x31, real x32, real x33, real x34,
                real x41, real x42, real x43, real x44) {
            this->c[0][0] = x11; this->c[1][0] = x12; this->c[2][0] = x13; this->c[3][0] = x14;
            this->c[0][1] = x21; this->c[1][1] = x22; this->c[2][1] = x23; this->c[3][1] = x24;
            this->c[0][2] = x31; this->c[1][2] = x32; this->c[2][2] = x33; this->c[3][2] = x34;
            this->c[0][3] = x41; this->c[1][3] = x42; this->c[2][3] = x43; this->c[3][3] = x44;
        }

        Matrix4 operator+( const Matrix4 &m ) const {
            Matrix4 mf;

            mf.x[0] = x[0] + m.x[0];
            mf.x[1] = x[1] + m.x[1];
            mf.x[2] = x[2] + m.x[2];
            mf.x[3] = x[3] + m.x[3];
            mf.x[4] = x[4] + m.x[4];
            mf.x[5] = x[5] + m.x[5];
            mf.x[6] = x[6] + m.x[6];
            mf.x[7] = x[7] + m.x[7];
            mf.x[8] = x[8] + m.x[8];
            mf.x[9] = x[9] + m.x[9];
            mf.x[10] = x[10] + m.x[10];
            mf.x[11] = x[11] + m.x[11];
            mf.x[12] = x[12] + m.x[12];
            mf.x[13] = x[13] + m.x[13];
            mf.x[14] = x[14] + m.x[14];
            mf.x[15] = x[15] + m.x[15];

            return mf;
        }

        Matrix4 &operator+=( const Matrix4 &m ) {
            return *this = *this + m;
        }

        Matrix4 operator*( const Matrix4 &m ) const  {
            Matrix4 mf;

            mf.x[0] = x[0] * m.x[0] + x[4] * m.x[1] + x[8] * m.x[2] + x[12] * m.x[3];
            mf.x[1] = x[1] * m.x[0] + x[5] * m.x[1] + x[9] * m.x[2] + x[13] * m.x[3];
            mf.x[2] = x[2] * m.x[0] + x[6] * m.x[1] + x[10] * m.x[2] + x[14] * m.x[3];
            mf.x[3] = x[3] * m.x[0] + x[7] * m.x[1] + x[11] * m.x[2] + x[15] * m.x[3];

            mf.x[4] = x[0] * m.x[4] + x[4] * m.x[5] + x[8] * m.x[6] + x[12] * m.x[7];
            mf.x[5] = x[1] * m.x[4] + x[5] * m.x[5] + x[9] * m.x[6] + x[13] * m.x[7];
            mf.x[6] = x[2] * m.x[4] + x[6] * m.x[5] + x[10] * m.x[6] + x[14] * m.x[7];
            mf.x[7] = x[3] * m.x[4] + x[7] * m.x[5] + x[11] * m.x[6] + x[15] * m.x[7];

            mf.x[8] = x[0] * m.x[8] + x[4] * m.x[9] + x[8] * m.x[10] + x[12] * m.x[11];
            mf.x[9] = x[1] * m.x[8] + x[5] * m.x[9] + x[9] * m.x[10] + x[13] * m.x[11];
            mf.x[10] = x[2] * m.x[8] + x[6] * m.x[9] + x[10] * m.x[10] + x[14] * m.x[11];
            mf.x[11] = x[3] * m.x[8] + x[7] * m.x[9] + x[11] * m.x[10] + x[15] * m.x[11];

            mf.x[12] = x[0] * m.x[12] + x[4] * m.x[13] + x[8] * m.x[14] + x[12] * m.x[15];
            mf.x[13] = x[1] * m.x[12] + x[5] * m.x[13] + x[9] * m.x[14] + x[13] * m.x[15];
            mf.x[14] = x[2] * m.x[12] + x[6] * m.x[13] + x[10] * m.x[14] + x[14] * m.x[15];
            mf.x[15] = x[3] * m.x[12] + x[7] * m.x[13] + x[11] * m.x[14] + x[15] * m.x[15];

            return mf;
        }

        Matrix4& operator*=( const Matrix4& m) {
            *this = *this * m;
            return *this;
        }

        Matrix4 operator*( const real f ) const {
            Matrix4 m( *this );

            m.x[0]  *= f; m.x[1]  *= f; m.x[2]  *= f; m.x[3]  *= f;
            m.x[4]  *= f; m.x[5]  *= f; m.x[6]  *= f; m.x[7]  *= f;
            m.x[8]  *= f; m.x[9]  *= f; m.x[10] *= f; m.x[11] *= f;
            m.x[12] *= f; m.x[13] *= f; m.x[14] *= f; m.x[15] *= f;

            return m;
        }

        Vector3 operator*( const Vector3 &v ) const {
            return Vector3( v.x() * c[0][0] + v.y() * c[1][0] + v.z() * c[2][0] + c[3][0],
                v.x() * c[0][1] + v.y() * c[1][1] + v.z() * c[2][1] + c[3][1],
                v.x() * c[0][2] + v.y() * c[1][2] + v.z() * c[2][2] + c[3][2] );
        }

        Vector4 operator*( const Vector4 &v ) const {
            return Vector4( v.x() * c[0][0] + v.y() * c[1][0] + v.z() * c[2][0] + v.w() * c[3][0],
                v.x() * c[0][1] + v.y() * c[1][1] + v.z() * c[2][1] + v.w() * c[3][1],
                v.x() * c[0][2] + v.y() * c[1][2] + v.z() * c[2][2] + v.w() * c[3][2],
                v.x() * c[0][3] + v.y() * c[1][3] + v.z() * c[2][3] + v.w() * c[3][3] );
        }

        Vector3 mult33Vec( const Vector3 &v ) const {
            return Vector3( v.x() * c[0][0] + v.y() * c[1][0] + v.z() * c[2][0],
                v.x() * c[0][1] + v.y() * c[1][1] + v.z() * c[2][1],
                v.x() * c[0][2] + v.y() * c[1][2] + v.z() * c[2][2] );
        }

        Matrix4& translate( const real x, const real y, const real z ) {
            *this = TransMat( x, y, z ) * *this;
            return *this;
        }

        Matrix4& scale( const real x, const real y, const real z ) {
            *this = ScaleMat( x, y, z ) * *this;
            return *this;
        }

        Matrix4& rotate( const real x, const real y, const real z ) {
            *this = RotMat( x, y, z ) * *this;
            return *this;
        }

        Matrix4 transposed() const {
            Matrix4 m( *this );

            for( unsigned int y = 0; y < 4; ++y ) {
                for( unsigned int x = y + 1; x < 4; ++x ) {
                    real tmp = m.c[x][y];
                    m.c[x][y] = m.c[y][x];
                    m.c[y][x] = tmp;
                }
            }

            return m;
        }

        real determinant() const {
            return 
                c[0][3]*c[1][2]*c[2][1]*c[3][0] - c[0][2]*c[1][3]*c[2][1]*c[3][0] - c[0][3]*c[1][1]*c[2][2]*c[3][0] + c[0][1]*c[1][3]*c[2][2]*c[3][0] +
                c[0][2]*c[1][1]*c[2][3]*c[3][0] - c[0][1]*c[1][2]*c[2][3]*c[3][0] - c[0][3]*c[1][2]*c[2][0]*c[3][1] + c[0][2]*c[1][3]*c[2][0]*c[3][1] +
                c[0][3]*c[1][0]*c[2][2]*c[3][1] - c[0][0]*c[1][3]*c[2][2]*c[3][1] - c[0][2]*c[1][0]*c[2][3]*c[3][1] + c[0][0]*c[1][2]*c[2][3]*c[3][1] +
                c[0][3]*c[1][1]*c[2][0]*c[3][2] - c[0][1]*c[1][3]*c[2][0]*c[3][2] - c[0][3]*c[1][0]*c[2][1]*c[3][2] + c[0][0]*c[1][3]*c[2][1]*c[3][2] +
                c[0][1]*c[1][0]*c[2][3]*c[3][2] - c[0][0]*c[1][1]*c[2][3]*c[3][2] - c[0][2]*c[1][1]*c[2][0]*c[3][3] + c[0][1]*c[1][2]*c[2][0]*c[3][3] +
                c[0][2]*c[1][0]*c[2][1]*c[3][3] - c[0][0]*c[1][2]*c[2][1]*c[3][3] - c[0][1]*c[1][0]*c[2][2]*c[3][3] + c[0][0]*c[1][1]*c[2][2]*c[3][3];
        }

        Matrix4 inverted() const {
            Matrix4 m;

            real d = determinant();
            if( d == 0 ) return m;
            d = 1.0f / d;

            m.c[0][0] = d * (c[1][2]*c[2][3]*c[3][1] - c[1][3]*c[2][2]*c[3][1] + c[1][3]*c[2][1]*c[3][2] - c[1][1]*c[2][3]*c[3][2] - c[1][2]*c[2][1]*c[3][3] + c[1][1]*c[2][2]*c[3][3]);
            m.c[0][1] = d * (c[0][3]*c[2][2]*c[3][1] - c[0][2]*c[2][3]*c[3][1] - c[0][3]*c[2][1]*c[3][2] + c[0][1]*c[2][3]*c[3][2] + c[0][2]*c[2][1]*c[3][3] - c[0][1]*c[2][2]*c[3][3]);
            m.c[0][2] = d * (c[0][2]*c[1][3]*c[3][1] - c[0][3]*c[1][2]*c[3][1] + c[0][3]*c[1][1]*c[3][2] - c[0][1]*c[1][3]*c[3][2] - c[0][2]*c[1][1]*c[3][3] + c[0][1]*c[1][2]*c[3][3]);
            m.c[0][3] = d * (c[0][3]*c[1][2]*c[2][1] - c[0][2]*c[1][3]*c[2][1] - c[0][3]*c[1][1]*c[2][2] + c[0][1]*c[1][3]*c[2][2] + c[0][2]*c[1][1]*c[2][3] - c[0][1]*c[1][2]*c[2][3]);
            m.c[1][0] = d * (c[1][3]*c[2][2]*c[3][0] - c[1][2]*c[2][3]*c[3][0] - c[1][3]*c[2][0]*c[3][2] + c[1][0]*c[2][3]*c[3][2] + c[1][2]*c[2][0]*c[3][3] - c[1][0]*c[2][2]*c[3][3]);
            m.c[1][1] = d * (c[0][2]*c[2][3]*c[3][0] - c[0][3]*c[2][2]*c[3][0] + c[0][3]*c[2][0]*c[3][2] - c[0][0]*c[2][3]*c[3][2] - c[0][2]*c[2][0]*c[3][3] + c[0][0]*c[2][2]*c[3][3]);
            m.c[1][2] = d * (c[0][3]*c[1][2]*c[3][0] - c[0][2]*c[1][3]*c[3][0] - c[0][3]*c[1][0]*c[3][2] + c[0][0]*c[1][3]*c[3][2] + c[0][2]*c[1][0]*c[3][3] - c[0][0]*c[1][2]*c[3][3]);
            m.c[1][3] = d * (c[0][2]*c[1][3]*c[2][0] - c[0][3]*c[1][2]*c[2][0] + c[0][3]*c[1][0]*c[2][2] - c[0][0]*c[1][3]*c[2][2] - c[0][2]*c[1][0]*c[2][3] + c[0][0]*c[1][2]*c[2][3]);
            m.c[2][0] = d * (c[1][1]*c[2][3]*c[3][0] - c[1][3]*c[2][1]*c[3][0] + c[1][3]*c[2][0]*c[3][1] - c[1][0]*c[2][3]*c[3][1] - c[1][1]*c[2][0]*c[3][3] + c[1][0]*c[2][1]*c[3][3]);
            m.c[2][1] = d * (c[0][3]*c[2][1]*c[3][0] - c[0][1]*c[2][3]*c[3][0] - c[0][3]*c[2][0]*c[3][1] + c[0][0]*c[2][3]*c[3][1] + c[0][1]*c[2][0]*c[3][3] - c[0][0]*c[2][1]*c[3][3]);
            m.c[2][2] = d * (c[0][1]*c[1][3]*c[3][0] - c[0][3]*c[1][1]*c[3][0] + c[0][3]*c[1][0]*c[3][1] - c[0][0]*c[1][3]*c[3][1] - c[0][1]*c[1][0]*c[3][3] + c[0][0]*c[1][1]*c[3][3]);
            m.c[2][3] = d * (c[0][3]*c[1][1]*c[2][0] - c[0][1]*c[1][3]*c[2][0] - c[0][3]*c[1][0]*c[2][1] + c[0][0]*c[1][3]*c[2][1] + c[0][1]*c[1][0]*c[2][3] - c[0][0]*c[1][1]*c[2][3]);
            m.c[3][0] = d * (c[1][2]*c[2][1]*c[3][0] - c[1][1]*c[2][2]*c[3][0] - c[1][2]*c[2][0]*c[3][1] + c[1][0]*c[2][2]*c[3][1] + c[1][1]*c[2][0]*c[3][2] - c[1][0]*c[2][1]*c[3][2]);
            m.c[3][1] = d * (c[0][1]*c[2][2]*c[3][0] - c[0][2]*c[2][1]*c[3][0] + c[0][2]*c[2][0]*c[3][1] - c[0][0]*c[2][2]*c[3][1] - c[0][1]*c[2][0]*c[3][2] + c[0][0]*c[2][1]*c[3][2]);
            m.c[3][2] = d * (c[0][2]*c[1][1]*c[3][0] - c[0][1]*c[1][2]*c[3][0] - c[0][2]*c[1][0]*c[3][1] + c[0][0]*c[1][2]*c[3][1] + c[0][1]*c[1][0]*c[3][2] - c[0][0]*c[1][1]*c[3][2]);
            m.c[3][3] = d * (c[0][1]*c[1][2]*c[2][0] - c[0][2]*c[1][1]*c[2][0] + c[0][2]*c[1][0]*c[2][1] - c[0][0]*c[1][2]*c[2][1] - c[0][1]*c[1][0]*c[2][2] + c[0][0]*c[1][1]*c[2][2]);

            return m;
        }

        void decompose( Vector3 &trans, Vector3 &rot, Vector3 &scale ) const {
            // Getting translation is trivial
            trans = Vector3( c[3][0], c[3][1], c[3][2] );

            // Scale is length of columns
            scale.x() = sqrtf( c[0][0] * c[0][0] + c[0][1] * c[0][1] + c[0][2] * c[0][2] );
            scale.y() = sqrtf( c[1][0] * c[1][0] + c[1][1] * c[1][1] + c[1][2] * c[1][2] );
            scale.z() = sqrtf( c[2][0] * c[2][0] + c[2][1] * c[2][1] + c[2][2] * c[2][2] );

            if( scale.x() == 0 || scale.y() == 0 || scale.z() == 0 ) return;

            // Detect negative scale with determinant and flip one arbitrary axis
            if( determinant() < 0 ) scale.x() = -scale.x();

            // Combined rotation matrix YXZ
            //
            // Cos[y]*Cos[z]+Sin[x]*Sin[y]*Sin[z]   Cos[z]*Sin[x]*Sin[y]-Cos[y]*Sin[z]  Cos[x]*Sin[y]	
            // Cos[x]*Sin[z]                        Cos[x]*Cos[z]                       -Sin[x]
            // -Cos[z]*Sin[y]+Cos[y]*Sin[x]*Sin[z]  Cos[y]*Cos[z]*Sin[x]+Sin[y]*Sin[z]  Cos[x]*Cos[y]

            rot.x() = asinf( -c[2][1] / scale.z() );

            // Special case: Cos[x] == 0 (when Sin[x] is +/-1)
            real f = fabsf( c[2][1] / scale.z() );
            if( f > 0.999f && f < 1.001f ) {
                // Pin arbitrarily one of y or z to zero
                // Mathematical equivalent of gimbal lock
                rot.y() = 0;

                // Now: Cos[x] = 0, Sin[x] = +/-1, Cos[y] = 1, Sin[y] = 0
                // => m[0][0] = Cos[z] and m[1][0] = Sin[z]
                rot.z() = atan2f( -c[1][0] / scale.y(), c[0][0] / scale.x() );
            }
            // Standard case
            else {
                rot.y() = atan2f( c[2][0] / scale.z(), c[2][2] / scale.z() );
                rot.z() = atan2f( c[0][1] / scale.x(), c[1][1] / scale.y() );
            }
        }

        Vector4 getCol( unsigned int col ) const {
            return Vector4( x[col * 4 + 0], x[col * 4 + 1], x[col * 4 + 2], x[col * 4 + 3] );
        }

        Vector4 getRow( unsigned int row ) const {
            return Vector4( x[row + 0], x[row + 4], x[row + 8], x[row + 12] );
        }

        Vector3 getTrans() const {
            return Vector3( c[3][0], c[3][1], c[3][2] );
        }

        Vector3 getScale() const {
            Vector3 scale;
            // Scale is length of columns
            scale.x() = sqrtf( c[0][0] * c[0][0] + c[0][1] * c[0][1] + c[0][2] * c[0][2] );
            scale.y() = sqrtf( c[1][0] * c[1][0] + c[1][1] * c[1][1] + c[1][2] * c[1][2] );
            scale.z() = sqrtf( c[2][0] * c[2][0] + c[2][1] * c[2][1] + c[2][2] * c[2][2] );
            return scale;
        }
    };

#undef min
#undef max

    class AABB3 {
    public:
        Vector3 lower_left;
        Vector3 upper_right;
        real boundingRadius;

        AABB3(): 
            lower_left(Vector3()), 
            upper_right(Vector3()), 
            boundingRadius(0.f) { 
        }

        AABB3(const Vector3& u, const Vector3& l): 
            lower_left(u), 
            upper_right(l), 
            boundingRadius((upper_right - lower_left).length() / 2.0f) { 
        }

        AABB3(const AABB3& rhs): 
            lower_left(rhs.lower_left), 
            upper_right(rhs.upper_right), 
            boundingRadius(rhs.boundingRadius) {
        }

        AABB3& operator=(const AABB3& rhs) {
            if(this != &rhs) {
                this->lower_left = rhs.lower_left;
                this->upper_right = rhs.upper_right;
                this->boundingRadius = rhs.boundingRadius;
            }
            return *this;
        }

        void set(const Vector3& min, const Vector3& max) {
            this->lower_left = min;
            this->upper_right = max;
            this->boundingRadius = (this->upper_right - this->lower_left).length() / 2.0f;
        }
        void set(const Vector3& min, const Vector3& max, real boundingRadius) {
            this->lower_left = min;
            this->upper_right = max;
            this->boundingRadius = boundingRadius;
        }

        real getBoudingRadius() const {
            return this->boundingRadius;
        }
        Vector3 getCenter() const {
            return (this->upper_right - this->lower_left) / 2.0f;
        }

        void transform(const Matrix4& mat) {
            Vector3 box[8];
            Vector3 newMin, newMax;

            box[0].x() = this->lower_left.x();
            box[0].y() = this->lower_left.y();
            box[0].z() = this->lower_left.z();

            box[1].x() = this->lower_left.x();
            box[1].y() = this->lower_left.y();
            box[1].z() = this->upper_right.z();

            box[2].x() = this->lower_left.x();
            box[2].y() = this->upper_right.y();
            box[2].z() = this->lower_left.z();

            box[3].x() = this->lower_left.x();
            box[3].y() = this->upper_right.y();
            box[3].z() = this->upper_right.z();

            box[4].x() = this->upper_right.x();
            box[4].y() = this->lower_left.y();
            box[4].z() = this->lower_left.z();

            box[5].x() = this->upper_right.x();
            box[5].y() = this->lower_left.y();
            box[5].z() = this->upper_right.z();

            box[6].x() = this->upper_right.x();
            box[6].y() = this->upper_right.y();
            box[6].z() = this->lower_left.z();

            box[7].x() = this->upper_right.x();
            box[7].y() = this->upper_right.y();
            box[7].z() = this->upper_right.z();

            newMin = mat * box[0];
            newMax = newMin;

            for(int i=1; i<8; ++i) {
                box[i] = mat * box[i];

                newMin.x() = MIST_MIN(box[i].x(), newMin.x());
                newMin.y() = MIST_MIN(box[i].x(), newMin.y());
                newMin.z() = MIST_MIN(box[i].x(), newMin.z());

                newMax.x() = MIST_MAX(box[i].x(), newMax.x());
                newMax.y() = MIST_MAX(box[i].x(), newMax.y());
                newMax.z() = MIST_MAX(box[i].x(), newMax.z());
            }

            set(Vector3(newMin),
                Vector3(newMax));
        }

        const Vector3& getMin() const {
            return this->lower_left;
        }
        const Vector3& getMax() const {
            return this->upper_right;
        }

        Vector3& getMin() {
            return this->lower_left;
        }
        Vector3& getMax() {
            return this->upper_right; 
        }

        real getWidth() const {
            return abs(this->upper_right.x() - this->lower_left.x());
        }
        real getHeight() const {
            return abs(this->upper_right.y() - this->lower_left.y());
        }
        real getDepth() const {
            return abs(this->upper_right.z() - this->lower_left.z());
        }

        void scale(float scale) {
            Vector3 center = this->getCenter();
            Vector3 dist = (this->upper_right - center) / 2.0 * scale;

            set(center - dist, center + dist);
        }

        operator AABB2() {
            return AABB2(lower_left.x(), lower_left.y(), upper_right.x(), upper_right.y());
        }
    };

    typedef AABB3 Box;

    class Plane {
    public:
        Plane() { }

        Plane(const Plane& rhs):
            mPlane(rhs.mPlane) { }

        explicit Plane(const Vector4& vec):
        mPlane(vec) { }

        Plane(real x, real y, real z, real w) {
            mPlane.set(x, y, z, w);
        }

        Plane& operator=(const Plane& rhs) {
            if(this != &rhs) {
                this->mPlane = rhs.mPlane;
            }
            return *this;
        }

        Plane& operator=(const Vector4& vec) {
            mPlane = vec;
            return *this;
        }

        bool operator==(const Plane& rhs) const {
            return this->mPlane.equals(rhs.mPlane);
        }

        void set(real x, real y, real z, real w) {
            this->mPlane.set(x, y, z, w);
        }

        Plane const operator-() const {
            return Plane(-this->mPlane);
        }

        Vector3 normal() const {
            return Vector3(this->mPlane.x(), this->mPlane.y(), this->mPlane.z());
        }

        real getDistance(const Vector3 point) {
            real num = this->mPlane.dot(Vector4(point));
            real dom = mist::math::fast_inv_sqrt(this->mPlane.x() * this->mPlane.x() + 
                this->mPlane.y() * this->mPlane.y() + 
                this->mPlane.z() * this->mPlane.z());
            return num/dom;
        }

        operator Vector4() const {
            return mPlane;
        }

        float a() const {
            return mPlane.x();
        }

        float b() const {
            return mPlane.y();
        }

        float c() const {
            return mPlane.z();
        }

        float d() const {
            return mPlane.w();
        }

    private:
        Vector4 mPlane;
    };

    class Sphere {
    public:
        Sphere():
            mRadius(0.f),
            mCenter(Vector3()) { }

        Sphere(const Vector3& pos, real radius):
            mRadius(radius),
            mCenter(pos) { }

        explicit Sphere(const Vector4& vec) {
            this->mCenter.set(vec.x(), vec.y(), vec.z());
            this->mRadius = vec.w();
        }

        const real& radius() const {
            return this->mRadius;
        }

        real& radius() {
            return this->mRadius;
        }

        const Vector3& center() const {
            return this->mCenter;
        }

        Vector3& center() {
            return this->mCenter;
        }

        bool isEmpty() {
            return math::float_equal(this->mRadius, 0.f);
        }

        Sphere& operator+=(const Vector3& trans) {
            this->mCenter += trans;
            return *this;
        }

        Sphere& operator-=(const Vector3& trans) {
            this->mCenter -= trans;
            return *this;
        }

        Sphere& operator*=(real s) {
            this->mRadius *= s;
            return *this;
        }

        Sphere& operator/=(real s) {
            mist_assert(s != 0.0f);

            this->mRadius /= s;
            return *this;
        }

        Sphere& operator=(const Sphere& rhs) {
            if(this != &rhs) {
                this->mRadius = rhs.mRadius;
                this->mCenter = rhs.mCenter;
            }
            return *this;
        }

        Vector4 asVector4() const {
            return Vector4(mCenter.x(), mCenter.y(), mCenter.z(), mRadius);
        }

        bool operator==(const Sphere& rhs) const {
            return this->mCenter.equals(rhs.mCenter) && 
                this->mRadius == rhs.mRadius;
        }

        bool vecInBound(const Vector3& point) {
            real distantce = (point - this->mCenter).sqrLength();

            return distantce <= mRadius * mRadius;
        }

        bool contains(const Vector3& point) {
            return vecInBound(point);
        }

    private:
        real mRadius;
        Vector3 mCenter;
    };

    class MIST_API PerlinNoise {
    public:
        static float Gen(float x, float y, float z);

    private:
        static float fade(float t);
        static float lerp(float t, float a, float b);
        static float grad(int hash, float x, float y, float z);
    };

    namespace traits {
        template<> struct is_pod<Vector2>   { enum { value = true }; };
        template<> struct is_pod<Vector3>   { enum { value = true }; };
        template<> struct is_pod<AABB2>     { enum { value = true }; };
        template<> struct is_pod<AABB3>     { enum { value = true }; };
        template<> struct is_pod<Matrix4>   { enum { value = true }; };
        template<> struct is_pod<Plane>     { enum { value = true }; };
        template<> struct is_pod<Sphere>    { enum { value = true }; };
        template<> struct is_pod<Quaternion>{ enum { value = true }; };
    }

    namespace reflection {
        template<> struct TypeNameRetriever<Vector2>    { static const char* Name() { return "vector2"; } };
        template<> struct TypeNameRetriever<Vector3>    { static const char* Name() { return "vector3"; } };
        template<> struct TypeNameRetriever<AABB2>      { static const char* Name() { return "rectangle"; } };
        template<> struct TypeNameRetriever<AABB3>      { static const char* Name() { return "box"; } };
        template<> struct TypeNameRetriever<Matrix4>    { static const char* Name() { return "matrix4"; } };
        template<> struct TypeNameRetriever<Plane>      { static const char* Name() { return "plane"; } };
        template<> struct TypeNameRetriever<Sphere>     { static const char* Name() { return "sphere"; } };
        template<> struct TypeNameRetriever<Quaternion> { static const char* Name() { return "quaternion"; } };
    }

} // namespace mist

#include "PreDeclare.h"
#include "Util.h"
namespace mist {

    namespace math {

        MIST_API Vector4 get_matrix_col(const Matrix4& mat, uint32 index);

        MIST_API Vector3 transform_quaternion(const Vector3& v, const Quaternion& q);
        MIST_API Quaternion inverse_quaternion(const Quaternion& q);
    }
    
} // namespace mist

#endif
