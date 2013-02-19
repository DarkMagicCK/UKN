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
    SelfType operator+(real r) const {\
        SelfType tmp = *this;\
        for(int i=0; i<Size; ++i) {\
            tmp.value[i] = Type(tmp.value[i] + r);\
        }\
        return tmp;\
    }\
    SelfType operator-(real r) const {\
        SelfType tmp = *this;\
        for(int i=0; i<Size; ++i) {\
            tmp.value[i] = Type(tmp.value[i] - r);\
        }\
        return tmp;\
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

    class MIST_API AABB2 {
    public:
        real x1, y1;
        real x2, y2;

        AABB2(real _x1, real _y1, real _x2, real _y2, bool asWH=false);
        AABB2();
        AABB2(const Vector2& upper, const Vector2& lower);
        AABB2(const Vector2& pos, real r);
        AABB2(const AABB2& rhs);

        void set(const Vector2& upper, const Vector2& lower);
        void setXY(real _x1, real _y1);
        void set(real _x1, real _y1, real _x2, real _y2, bool asWH=false);
        void setAsBox(real x, real y, real r);
        AABB2& operator=(const AABB2& rhs);

        real width() const { return this->x2 - this->x1;}
        real height() const { return this->y2 - this->y1;}
        Vector2 upper() const {return Vector2(this->x1, this->y1);}
        Vector2 lower() const {return Vector2(this->x2, this->y2);}
        real right() const {return this->x2;}
        real bottom() const {return this->y2;}
        real left() const {return this->x1;}
        real top() const {return this->y1;}
        real x() const {return this->x1;}
        real y() const {return this->y1;}

        void encapsulate(real x, real y);
        bool contains(real x, real y) const;
        bool contains(const Vector2& pos) const;
        bool contains(const AABB2& rhs, real margin = 0) const;
        bool intersect(const AABB2& rhs) const;
        bool isEmpty() const;
        Vector2 getPosition() const;
    };

    typedef AABB2 Rectangle;

    class MIST_API Quaternion {
    public:
        Quaternion(real _x, real _y, real _z, real _w);
        Quaternion(const Quaternion& quad);
        Quaternion();
        Quaternion(real eulerX, real eulerY, real eulerZ);

        void set(real _x, real _y, real _z, real _w);
        void set(const Quaternion& quad);
        float length() const;

        Quaternion& operator=(const Quaternion& rhs);
        Quaternion operator*(const Quaternion& rhs) const;
        Quaternion& operator*=(const Quaternion& rhs);

        void makeRotate(real angle, real _x, real _y, real _z);
        void makeRotate(real angle, const Vector3& vec);
        void makeRotate(real roll, real pitch, real yaw);

        Vector3 rotate(const Vector3& v) const;

        void getRotate(real& angle, real& _x, real& _y, real& _z) const;
        void getRotate(real& angle, Vector3& vec) const;
        void slerp(real t, const Quaternion& from, const Quaternion& to);

        float x() const { return vec.x(); }
        float y() const { return vec.y(); }
        float z() const { return vec.z(); }
        float w() const { return vec.w(); }

        Vector4 vec;
    };

    class MIST_API Matrix4 {
    public:
        union {
            real c[4][4];	// Column major order for OpenGL: c[column][row]
            real x[16];
        };

        static Matrix4 TransMat( real x, real y, real z );
        static Matrix4 ScaleMat( real x, real y, real z );
        static Matrix4 RotMat( real x, real y, real z );
        static Matrix4 RotMat( Vector3 axis, real angle );
        static Matrix4 RotX(real x);
        static Matrix4 RotY(real y);
        static Matrix4 RotZ(real z);
        static Matrix4 LHToRH(const Matrix4& mat);
        static Matrix4 PerspectiveFovMatLH(real field_of_view, real aspect, real n, real f);
        static Matrix4 PerspectiveFovMatRH(real field_of_view, real aspect, real n, real f);
        static Matrix4 PerspectiveMatLH(real width, real height, real n, real f);
        static Matrix4 PerspectiveMatRH(real width, real height, real n, real f);
        static Matrix4 PerspectiveOffCenterMatLH(real l, real r, real b, real t, real n, real f);
        static Matrix4 PerspectiveOffCenterMatRH(real l, real r, real b, real t, real n, real f);
        static Matrix4 OrthoMatLH(real w, real h, real n, real f);
        static Matrix4 OrthoMatRH(real w, real h, real n, real f);
        static Matrix4 OrthoOffCenterMatLH(real l, real r, real b, real t, real n, real f );
        static Matrix4 OrthoOffCenterMatRH(real l, real r, real b, real t, real n, real f);
        static Matrix4 LookAtMatLH(const Vector3& eye, const Vector3& lookAt, const Vector3& up);
        static Matrix4 LookAtMatRH(const Vector3& eye, const Vector3& lookAt, const Vector3& up);
        static Matrix4 FastMult43( const Matrix4 &m1, const Matrix4 &m2 );

        Matrix4();
        Matrix4( const real *realArray16 );
        Matrix4( const Quaternion &q );
        Matrix4(real x11, real x12, real x13, real x14,
                real x21, real x22, real x23, real x24,
                real x31, real x32, real x33, real x34,
                real x41, real x42, real x43, real x44);
        Matrix4 operator+( const Matrix4 &m ) const;
        Matrix4 &operator+=( const Matrix4 &m );
        Matrix4 operator*( const Matrix4 &m ) const;
        Matrix4& operator*=( const Matrix4& m);
        Matrix4 operator*( const real f ) const;

        Vector3 operator*( const Vector3 &v ) const;
        Vector4 operator*( const Vector4 &v ) const;
        Vector3 mult33Vec( const Vector3 &v ) const;

        Matrix4& translate( const real x, const real y, const real z );
        Matrix4& scale( const real x, const real y, const real z );
        Matrix4& rotate( const real x, const real y, const real z );
        Matrix4 transposed() const;

        real determinant() const;
        Matrix4 inverted() const;
        void decompose( Vector3 &trans, Vector3 &rot, Vector3 &scale ) const;
        Vector4 getCol( unsigned int col ) const;
        Vector4 getRow( unsigned int row ) const;
        Vector3 getTrans() const;
        Vector3 getScale() const;
    };

    class MIST_API AABB3 {
    public:
        Vector3 lower_left;
        Vector3 upper_right;
        real boundingRadius;

        AABB3();
        AABB3(const Vector3& u, const Vector3& l);
        AABB3(const AABB3& rhs);
        AABB3& operator=(const AABB3& rhs);

        void set(const Vector3& min, const Vector3& max);
        void set(const Vector3& min, const Vector3& max, real boundingRadius);

        real getBoudingRadius() const;
        Vector3 getCenter() const;

        void transform(const Matrix4& mat);
        const Vector3& getMin() const;
        const Vector3& getMax() const;
        Vector3& getMin();
        Vector3& getMax();
        real getWidth() const;
        real getHeight() const;
        real getDepth() const;
        void scale(float scale);
    };

    typedef AABB3 Box;

    class MIST_API Plane {
    public:
        Plane();
        Plane(const Plane& rhs);
        explicit Plane(const Vector4& vec);

        Plane(real x, real y, real z, real w);
        Plane& operator=(const Plane& rhs);
        Plane& operator=(const Vector4& vec);

        bool operator==(const Plane& rhs) const;
        void set(const Vector4& vec);

        Plane operator-() const;
        Vector4 get() const;
        real dot(const Vector3& rhs) const;
        real getDistance(const Vector3 point);

        float a() const { return mPlane.x();}
        float b() const { return mPlane.y();}
        float c() const { return mPlane.z();}
        float d() const { return mPlane.w();}

    private:
        Vector4 mPlane;
    };

    class MIST_API Sphere {
    public:
        Sphere();
        Sphere(const Vector3& pos, real radius);
        explicit Sphere(const Vector4& vec);

        const real& radius() const;
        real& radius();

        const Vector3& center() const;
        Vector3& center();

        bool isEmpty() const;

        Sphere& operator+=(const Vector3& trans);
        Sphere& operator-=(const Vector3& trans);

        Sphere& operator*=(real s);
        Sphere& operator/=(real s);
        Sphere& operator=(const Sphere& rhs);

        Vector4 asVector4() const;

        bool operator==(const Sphere& rhs) const;
        bool vecInBound(const Vector3& point);
        bool contains(const Vector3& point);

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
