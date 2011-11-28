//
//  Math.h
//  Project Unknown
//
//  Created by Robert Bu on 11/22/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_Math_h
#define Project_Unknown_Math_h

#include "Platform.h"
#include <cmath>

namespace ukn {
    
    const float pi    = 3.14159265358f;
	const float pi_2  = 1.57079632679f;
	const float pi_4  = 0.78539826339f;
	const float pi_6  = 0.5233333f;
	const float pi_12 = 0.2616667f;
    
	const double d_pi	  =	3.1415926535897932384626433832795;
	const double d_pi_2   = 1.5707963267948966192313216916398;
	const double d_pi_4   = 0.7853981633974483096156608458198;
	const double d_pi_8   = 0.3926990816987241548078304229099;
    
    inline real degree_to_radius(real dgr) {
        return (d_pi * dgr) / 180.f;
    }
    
    inline real radius_to_degree(real rad) {
        return (180.f * rad) / d_pi;
    }
    
#ifndef UKN_TINY
#define UKN_TINY 0.00000001f
#endif
    
    inline bool float_equal(real f1, real f2) {
        return fabs(f1 - f2) < UKN_TINY;
    }
    
    inline real fast_inv_sqrt(real x) {
        {
            union
            {
                int intPart;
                real realPart;
            } convertor;
            
            convertor.realPart = x;
            convertor.intPart = 0x5f3759df - (convertor.intPart >> 1);
            return convertor.realPart*(1.5f - 0.4999f*x*convertor.realPart*convertor.realPart);
        }
    }
    
    // compress a unit float to nBits integer
	// there would be some data loss but good for some condition
	// it is safe to cast the result to nBits integer, such as uint16 if you pass nBits as 16
	inline uint32 compress_unit_float(real unitFloat, uint32 nBits) {
		// determine the number of intervals based on the number of output
		uint32 nInterval = 1u << nBits;
		
		// scale the input value from the range [0, 1] into the range [0, nIntervals - 1]
		// substract 1 because we want the largest output value to fit into nBits bits
		real scaled = unitFloat * (real)(nInterval - 1u);
		
		// finally round to the nearest interval center
		// by adding 0.5f, then truncating to the next-lowest interval index
		uint32 rounded = (uint32)(scaled + 0.5f);
		
		if(rounded > nInterval-1u)
			rounded = nInterval-1u;
		
		return rounded;
	}
	
	// decompress a unit float from a quantized uint
	inline real decompress_unit_float(uint32 quantized, uint32 nBits) {
		// determine the number of intervals based on the number of bits
		uint32 nIntervals = 1u << nBits;
		
		// decode by simply converting the uint32 to an float, then scaling by the interval size
		real intervalSize = 1.f / (real)(nIntervals - 1u);
		
		real approxUnitFloat = (real)quantized * intervalSize;
		return approxUnitFloat;
	}
	
	// compress a float within a specific range
	inline uint32 compress_float(real value, real min, real max, uint32 nBits) {
		float unitFloat = (value - min) / (max - min);
		uint32 quantized = compress_unit_float(unitFloat, nBits);
		return quantized;
	}
	
	inline real decompress_float(uint32 quantized, real min, real max, uint32 nBits) {
		real unitFloat = decompress_unit_float(quantized, nBits);
		real value = min + (unitFloat * (max - min));
		return value;
	}
    
    inline uint16 flip_bytes(uint16 value) {
        return ((value >> 8) & 0x00FF) | ((value << 8) & 0xFF00);
    }
    
    inline uint32 flip_bytes(uint32 value) {
        return ((value >> 24) & 0x000000FF) | ((value >> 8) & 0x0000FF00) |
        ((value << 8) & 0x00FF0000) | ((value << 24) & 0xFF000000);
    }
    
    inline int16 flip_bytes(int16 value) {
        return int16(flip_bytes(uint16(value)));
    }
    
    inline int32 flip_bytes(int32 value) {
        return int32(flip_bytes(uint32(value)));
    }
    
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
    
    inline float clamp(float t1, float t2, real v) {
        if(v > t2) return t2;
        else if(v < t1) return t1;
        return v;
    }
    
#define ukn_min(a, b) a < b ? a : b
#define ukn_max(a, b) a > b ? a : b
#define ukn_abs(x) x < 0 ? -x : x
    
    class Vector2 {
    public:
        real x, y;
        
        Vector2(real _x, real _y):
        x(_x),
        y(_y) { }
        
        Vector2():
        x(0.f),
        y(0.f) {
            
        }
        
        void set(real _x, real _y) {
            this->x = _x;
            this->y = _y;
        }
        
        Vector2 operator -() const { 
            return Vector2(-this->x, -this->y);
        }
        
        Vector2 operator -(const Vector2& rhs) const { 
            return Vector2(this->x-rhs.x, this->y-rhs.y);
        }
        
        Vector2 operator +(const Vector2& rhs) const { 
            return Vector2(this->x+rhs.x, this->y+rhs.y);
        }
        
        Vector2& operator+=(const Vector2& rhs) { 
            this->x += rhs.x; this->y += rhs.y; return *this;
        }
        
        Vector2& operator-=(const Vector2& rhs) { 
            this->x -= rhs.x; this->y -= rhs.y; return *this;
        }
        
        bool operator ==(const Vector2& rhs) const { 
            return float_equal(this->x, rhs.x) && float_equal(this->y, rhs.y); 
        }
        
        bool operator !=(const Vector2& rhs) const { 
            return this->x!=rhs.x && this->y!=rhs.y; 
        }
        
        Vector2 operator/(real s) const { 
            return Vector2(this->x/s, this->y/s);
        }
        
        Vector2 operator*(real s) const { 
            return Vector2(this->x*s, this->y*s);
        }
        
        Vector2& operator*=(real s) { 
            this->x *= s; this->y *= s; return *this;
        }
        
        real dot(const Vector2& rhs) const { 
            return this->x*rhs.x + this->y*rhs.y;
        }
        
        real length() const { 
            return sqrtf(dot(*this));
        }
        
        real lengthsq() const { 
            return dot(*this);
        }
        
        real angle(const Vector2* rhs = 0) const {
            if(rhs) {
                Vector2 s = *this, t = *rhs;
                s.normalize();
                t.normalize();
                return acosf(s.dot(t));
            } else
                return atan2f(this->y, this->x);
        }
        
        void clamp(const real max) {
            if(length() > max) {
                normalize();
                this->x *= max;
                this->y *= max;
            }
        }
        
        Vector2& normalize() {
            real rc = fast_inv_sqrt(dot(*this));
            this->x *= rc;
            this->y *= rc;
            return *this;
        }
        
        Vector2& rotate(real a) {
            Vector2 v;
            v.x = this->x * cosf(a) - this->y * sinf(a);
            v.y = this->x * sinf(a) + this->y * cosf(a);
            this->x = v.x; this->y = v.y;
            return *this;
        }
        
        Vector2 normal(const Vector2& rhs){
            Vector2 normal;
            
            normal.x = this->y - rhs.y;
            normal.y = rhs.x - this->x;
            real len = sqrt(normal.x * normal.x + normal.y * normal.y);
            if(len == 0.f) {
                return Vector2();
            }
            
            normal.x /= len;
            normal.y /= len;
            
            return normal;
        }
    };
    
    class Vector3 {
	public:		
		Vector3(): x(0.f), y(0.f), z(0.f) {
        }
        
		Vector3(real _x, real _y, real _z): x(_x), y(_y), z(_z) {
        }
        
		Vector3(const Vector3& rhs): x(rhs.x), y(rhs.y), z(rhs.z) {
        }
		
		void set(real _x, real _y, real _z) {
            this->x = _x; this->y = _y; this->z = _z;
		}
        
		Vector3& operator=(const Vector3& rhs)  {
			this->x = rhs.x; this->y = rhs.y; this->z = rhs.z;
			return *this;
		}
        
		Vector3 operator-(const Vector3& rhs) const {
			return Vector3(this->x-rhs.x, this->y-rhs.y, this->z-rhs.z);
		}
        
		Vector3 operator+(const Vector3& rhs) const {
			return Vector3(this->x+rhs.x, this->y+rhs.y, this->z+rhs.z);
		}
        
		Vector3 operator*(real f) const {
			return Vector3(this->x*f, this->y*f, this->z*f);
		}
        
		Vector3 operator/(real f) const {
			assert(f != 0);
			return Vector3(this->x/f, this->y/f, this->z/f);
		}
        
		Vector3& operator+=(const Vector3& rhs) {
			this->x += rhs.x; this->y += rhs.y; this->z += rhs.z;
			return *this;
		}
        
		Vector3& operator-=(const Vector3& rhs) {
			this->x -= rhs.x; this->y -= rhs.y; this->z -= rhs.z;
			return *this;
		}
        
        Vector3 operator-() const {
            return Vector3(-this->x, -this->y, -this->z);
        }
        
        real operator*(const Vector3& rhs) {
            return this->dot(rhs);
        }
        
		real dot(const Vector3& rhs) const {
			return this->x*rhs.x+this->y*rhs.y+this->z*rhs.z;
		}
        
		Vector3 cross(const Vector3& rhs) const {
			return Vector3(-this->z*rhs.y+this->y*rhs.z, this->z*rhs.x-this->x*rhs.z, -this->y*rhs.x+this->x*rhs.y);
		}
        
		real length() const {
			return (real)sqrtf(this->x*this->x + this->y*this->y + this->z*this->z);
		}
        
		real sqrLength() const {
			return this->x*this->x + this->y*this->y + this->z*this->z;
		}
        
		Vector3 normalize() const {
			real inv = 1/length();
			return Vector3(this->x*inv, this->y*inv, this->z*inv);
		}
        
		Vector3 negate() const {
			return Vector3(-this->x, -this->y, -this->z);
		}
        
        bool operator==(const Vector3& rhs) const {
            return this->x == rhs.x && this->y == rhs.y && this->z == rhs.z;
        }
        
		real x, y, z;
	};
    
    class Vector4 {
    public:
        real x, y, z, w;
        
        Vector4():
        x( 0 ), 
        y( 0 ), 
        z( 0 ), 
        w( 0 ) { }
        
        Vector4(const real x, const real y, const real z, const real w):
		x( x ), 
        y( y ), 
        z( z ), 
        w( w ) { }
        
        Vector4(const Vector3& v):
        x( v.x ), 
        y( v.y ), 
        z( v.z ),
        w( 1.0f ) { }
        
        void set(real _x, real _y, real _z, real _w) {
            this->x = _x;
            this->y = _y;
            this->z = _z;
            this->w = _w;
        }
        
        Vector4 operator-() const {
            return Vector4(-x, -y, -z, -w);
        }
        
        Vector4 operator+(const Vector4 &v) const {
            return Vector4(this->x + v.x, 
                               this->y + v.y, 
                               this->z + v.z, 
                               this->w + v.w);
        }
        
        Vector4 operator-(const Vector4 &v) const {
            return Vector4(this->x - v.x, 
                           this->y - v.y, 
                           this->z - v.z, 
                           this->w - v.w);
        }
        
        Vector4 operator*(const real f) const {
            return Vector4(this->x * f, 
                               this->y * f, 
                               this->z * f, 
                               this->w * f);
        }
        
        real dot(const Vector4& vec) const {
            return this->x * vec.x + this->y * vec.y + this->z * vec.z + this->w * vec.w;
        }
        
        real sqrLength() const {
            return this->dot(*this);
        }
        
        real length() const {
            return sqrtf(sqrLength());
        }
        
        Vector4 normalize() const {
            float invLength = 1 / length();
            return Vector4(x * invLength,
                           y * invLength,
                           z * invLength,
                           w * invLength);
        }
        
        operator Vector3() {
            return Vector3(x, y, z);
        }
        
        real operator*(const Vector4& rhs) const {
            return this->x * rhs.x + this->y * rhs.y + this->z * rhs.z + this->w * rhs.w;
        }
        
        bool operator==(const Vector4& rhs) const {
            return this->x == rhs.x && this->y == rhs.y && this->z == rhs.z && this->w == rhs.w;
        }
    };
    
    class AABB2 {
    public:
        real x1, y1;
        real x2, y2;
        
        AABB2(real _x1, real _y1, real _x2, real _y2, bool asWH=false):
        x1(_x1), y1(_y1),
        x2(_x2), y2(_y2) {
            
        }
        
        AABB2() {   
        }
        
        AABB2(const Vector2& upper, const Vector2& lower):
        x1(upper.x), y1(upper.y),
        x2(lower.x), y2(lower.y) {
            
        }
        
        AABB2(const Vector2& pos, real r) {
            setAsBox(pos.x, pos.y, r);
        }
        
        void set(const Vector2& upper, const Vector2& lower) {
            this->x1 = upper.x;
            this->y1 = upper.y;
            this->x2 = upper.x;
            this->y2 = upper.y;
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
        
        real getWidth() const {
            return this->x2 - this->x1;
        }
        
        real getHeight() const {
            return this->y2 - this->y1;
        }
        
        Vector2 getUpper() const {
            return Vector2(this->x1, this->y1);
        }
        
        Vector2 getLower() const {
            return Vector2(this->x2, this->y2);
        }
        
        void encapsulate(real x, real y) {
            if(x<this->x1) this->x1 = x;
            if(x>this->x2) this->x2 = x;
            if(y<this->y1) this->y1 = y;
            if(y>this->y2) this->y2 = y;
        }
        
        bool testPoint(real x, real y) const {
            return x>=x1 && x<=x2 && y>=y1 && y<=y2;
        }
        
        bool testPoint(const Vector2& pos) const {
            return testPoint(pos.x, pos.y);
        }
        
        bool intersect(const AABB2& rhs) const {
            if(fabs(this->x1 + this->x2 - rhs.x1 - rhs.x2) < (this->x2 - this->x1 + rhs.x2 - rhs.x1)) {
                if(fabs(this->y1 + this->y2 - rhs.y1 - rhs.y2) < (this->y2 - this->y1 + rhs.y2 - rhs.y1)) {
                    return true;
                }
            }
            return false;
        }
        
        Vector2 getPosition() const {
            return Vector2((this->x1 + this->x2)/2,
                              (this->y1 + this->y2)/2);
        }
    };
  
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
			makeRotate(angle, vec.x, vec.y, vec.z);
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
			real ux = this->w*v.x + this->y*v.z - this->z*v.y;
			real uy = this->w*v.y + this->z*v.x - this->x*v.z;
			real uz = this->w*v.z + this->x*v.y - this->y*v.x;
			real uw = -this->x*v.x - this->y*v.y - this->z*v.z;
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
			getRotate(angle, vec.x, vec.y, vec.z);
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
            return Matrix4( Quaternion( axis.x, axis.y, axis.z, cosf( angle * 0.5f ) ) );
        }
        
        static Matrix4 FrustumMat( real l, real r, real b, real t, real n, real f ) {
            Matrix4 m;
            
            m.x[0] = 2 * n / (r - l);
            m.x[5] = 2 * n / (t - b);
            m.x[8] = (r + l) / (r - l);
            m.x[9] = (t + b) / (t - b);
            m.x[10] = -(f + n) / (f - n);
            m.x[11] = -1;
            m.x[14] = -2 * f * n / (f - n);
            
            return m;
        }
        
        static Matrix4 PerspectiveMat(real field_of_view, real aspect, real n, real f) {
            Matrix4 m;
            real ff = 1.0f / tan(degree_to_radius(field_of_view) / 2.0f);
            m.x[0] = ff / aspect;
            m.x[5] = ff;
            m.x[10] = (n + f) / (n - f);
            m.x[14] = (2.0f * f * n) / (n - f);
            m.x[11] = -1.f;
            return m;
        }
        
        static Matrix4 OrthoMat( real l, real r, real b, real t, real n, real f ) {
            Matrix4 m;
            
            m.x[0] = 2 / (r - l);
            m.x[5] = 2 / (t - b);
            m.x[10] = -2 / (f - n);
            m.x[12] = -(r + l) / (r - l);
            m.x[13] = -(t + b) / (t - b);
            m.x[14] = -(f + n) / (f - n);
            
            return m;
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
                    c[i][j] = realArray16[i * 4 + j];
                }
            }
        }
        
        Matrix4( const Quaternion &q ) {
            // Calculate coefficients
            real x2 = q.x + q.x, y2 = q.y + q.y, z2 = q.z + q.z;
            real xx = q.x * x2,  xy = q.x * y2,  xz = q.x * z2;
            real yy = q.y * y2,  yz = q.y * z2,  zz = q.z * z2;
            real wx = q.w * x2,  wy = q.w * y2,  wz = q.w * z2;
            
            c[0][0] = 1 - (yy + zz);  c[1][0] = xy - wz;	
            c[2][0] = xz + wy;        c[3][0] = 0;
            c[0][1] = xy + wz;        c[1][1] = 1 - (xx + zz);
            c[2][1] = yz - wx;        c[3][1] = 0;
            c[0][2] = xz - wy;        c[1][2] = yz + wx;
            c[2][2] = 1 - (xx + yy);  c[3][2] = 0;
            c[0][3] = 0;              c[1][3] = 0;
            c[2][3] = 0;              c[3][3] = 1;
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
        
        Matrix4 operator*( const real f ) const {
            Matrix4 m( *this );
            
            m.x[0]  *= f; m.x[1]  *= f; m.x[2]  *= f; m.x[3]  *= f;
            m.x[4]  *= f; m.x[5]  *= f; m.x[6]  *= f; m.x[7]  *= f;
            m.x[8]  *= f; m.x[9]  *= f; m.x[10] *= f; m.x[11] *= f;
            m.x[12] *= f; m.x[13] *= f; m.x[14] *= f; m.x[15] *= f;
            
            return m;
        }
       
        Vector3 operator*( const Vector3 &v ) const {
            return Vector3( v.x * c[0][0] + v.y * c[1][0] + v.z * c[2][0] + c[3][0],
                               v.x * c[0][1] + v.y * c[1][1] + v.z * c[2][1] + c[3][1],
                               v.x * c[0][2] + v.y * c[1][2] + v.z * c[2][2] + c[3][2] );
        }
        
        Vector4 operator*( const Vector4 &v ) const {
            return Vector4( v.x * c[0][0] + v.y * c[1][0] + v.z * c[2][0] + v.w * c[3][0],
                               v.x * c[0][1] + v.y * c[1][1] + v.z * c[2][1] + v.w * c[3][1],
                               v.x * c[0][2] + v.y * c[1][2] + v.z * c[2][2] + v.w * c[3][2],
                               v.x * c[0][3] + v.y * c[1][3] + v.z * c[2][3] + v.w * c[3][3] );
        }
        
        Vector3 mult33Vec( const Vector3 &v ) const {
            return Vector3( v.x * c[0][0] + v.y * c[1][0] + v.z * c[2][0],
                               v.x * c[0][1] + v.y * c[1][1] + v.z * c[2][1],
                               v.x * c[0][2] + v.y * c[1][2] + v.z * c[2][2] );
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
            scale.x = sqrtf( c[0][0] * c[0][0] + c[0][1] * c[0][1] + c[0][2] * c[0][2] );
            scale.y = sqrtf( c[1][0] * c[1][0] + c[1][1] * c[1][1] + c[1][2] * c[1][2] );
            scale.z = sqrtf( c[2][0] * c[2][0] + c[2][1] * c[2][1] + c[2][2] * c[2][2] );
            
            if( scale.x == 0 || scale.y == 0 || scale.z == 0 ) return;
            
            // Detect negative scale with determinant and flip one arbitrary axis
            if( determinant() < 0 ) scale.x = -scale.x;
            
            // Combined rotation matrix YXZ
            //
            // Cos[y]*Cos[z]+Sin[x]*Sin[y]*Sin[z]   Cos[z]*Sin[x]*Sin[y]-Cos[y]*Sin[z]  Cos[x]*Sin[y]	
            // Cos[x]*Sin[z]                        Cos[x]*Cos[z]                       -Sin[x]
            // -Cos[z]*Sin[y]+Cos[y]*Sin[x]*Sin[z]  Cos[y]*Cos[z]*Sin[x]+Sin[y]*Sin[z]  Cos[x]*Cos[y]
            
            rot.x = asinf( -c[2][1] / scale.z );
            
            // Special case: Cos[x] == 0 (when Sin[x] is +/-1)
            real f = fabsf( c[2][1] / scale.z );
            if( f > 0.999f && f < 1.001f ) {
                // Pin arbitrarily one of y or z to zero
                // Mathematical equivalent of gimbal lock
                rot.y = 0;
                
                // Now: Cos[x] = 0, Sin[x] = +/-1, Cos[y] = 1, Sin[y] = 0
                // => m[0][0] = Cos[z] and m[1][0] = Sin[z]
                rot.z = atan2f( -c[1][0] / scale.y, c[0][0] / scale.x );
            }
            // Standard case
            else {
                rot.y = atan2f( c[2][0] / scale.z, c[2][2] / scale.z );
                rot.z = atan2f( c[0][1] / scale.x, c[1][1] / scale.y );
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
            scale.x = sqrtf( c[0][0] * c[0][0] + c[0][1] * c[0][1] + c[0][2] * c[0][2] );
            scale.y = sqrtf( c[1][0] * c[1][0] + c[1][1] * c[1][1] + c[1][2] * c[1][2] );
            scale.z = sqrtf( c[2][0] * c[2][0] + c[2][1] * c[2][1] + c[2][2] * c[2][2] );
            return scale;
        }
    };
    
    
    class AABB3 {
    public:
        Vector3 min;
        Vector3 max;
        real boundingRadius;
        
        AABB3(): 
        min(Vector3()), 
        max(Vector3()), 
        boundingRadius(0.f) { 
        }
        
        AABB3(const Vector3& u, const Vector3& l): 
        min(u), 
        max(l), 
        boundingRadius((max - min).length() / 2.0) { 
        }
        
        AABB3(const AABB3& rhs): 
        min(rhs.min), 
        max(rhs.max), 
        boundingRadius(rhs.boundingRadius) {
        }
        
        AABB3& operator=(const AABB3& rhs) {
            if(this != &rhs) {
                this->min = rhs.min;
                this->max = rhs.max;
                this->boundingRadius = rhs.boundingRadius;
            }
            return *this;
        }
        
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
    
    inline void AABB3::set(const Vector3& min, const Vector3& max) {
        this->min = min;
        this->max = max;
        this->boundingRadius = (this->max - this->min).length() / 2.0f;
    }
    
    inline void AABB3::set(const Vector3& min, const Vector3& max, real boundingR) {
        this->min = min;
        this->max = max;
        this->boundingRadius = boundingR;
    }
    
    inline real AABB3::getBoudingRadius() const {
        return this->boundingRadius;
    }
    
    inline Vector3 AABB3::getCenter() const {
        return (this->max - this->min) / 2.0f;
    }
    
    inline void AABB3::transform(const Matrix4& mat) {
        Vector4 box[8];
        Vector4 newMin, newMax;
        
        box[0].x = this->min.x;
        box[0].y = this->min.y;
        box[0].z = this->min.z;
        
        box[1].x = this->min.x;
        box[1].y = this->min.y;
        box[1].z = this->max.z;
        
        box[2].x = this->min.x;
        box[2].y = this->max.y;
        box[2].z = this->min.z;
        
        box[3].x = this->min.x;
        box[3].y = this->max.y;
        box[3].z = this->max.z;
        
        box[4].x = this->max.x;
        box[4].y = this->min.y;
        box[4].z = this->min.z;
        
        box[5].x = this->max.x;
        box[5].y = this->min.y;
        box[5].z = this->max.z;
        
        box[6].x = this->max.x;
        box[6].y = this->max.y;
        box[6].z = this->min.z;
        
        box[7].x = this->max.x;
        box[7].y = this->max.y;
        box[7].z = this->max.z;
        
        newMin = mat * box[0];
        newMax = newMin;
        
        for(int i=1; i<8; ++i) {
            box[i] = mat * box[i];
            
            newMin.x = ukn_min(box[i].x, newMin.x);
            newMin.y = ukn_min(box[i].x, newMin.y);
            newMin.z = ukn_min(box[i].x, newMin.z);
            
            newMax.x = ukn_max(box[i].x, newMax.x);
            newMax.y = ukn_max(box[i].x, newMax.y);
            newMax.z = ukn_max(box[i].x, newMax.z);
        }
        
        set(static_cast<Vector3>(newMin),
            static_cast<Vector3>(newMax));
    }
    
    inline const Vector3& AABB3::getMin() const {
        return this->min;
    }
    
    inline const Vector3& AABB3::getMax() const {
        return this->max;
    }
    
    inline Vector3& AABB3::getMin() {
        return this->min;
    }
    
    inline Vector3& AABB3::getMax() {
        return this->max; 
    }
    
    inline real AABB3::getWidth() const {
        return abs(this->max.x - this->min.x);
    }
    
    inline real AABB3::getHeight() const {
        return abs(this->max.y - this->min.y);
    }
    
    inline real AABB3::getDepth() const {
        return abs(this->max.z - this->min.z);
    }
    
    inline void AABB3::scale(float scale) {
        Vector3 center = this->getCenter();
        Vector3 dist = (this->max - center) / 2.0 * scale;
        
        set(center - dist, center + dist);
    }
    
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
            return this->mPlane == rhs.mPlane;
        }
        
        void set(real x, real y, real z, real w) {
            this->mPlane.set(x, y, z, w);
        }
        
        Plane const operator-() const {
            return Plane(-this->mPlane);
        }
        
        Vector3 normal() const {
            return Vector3(this->mPlane.x, this->mPlane.y, this->mPlane.z);
        }
        
        real getDistance(const Vector3 point) {
            real num = this->mPlane * Vector4(point);
            real dom = fast_inv_sqrt(this->mPlane.x * this->mPlane.x + 
                                     this->mPlane.y * this->mPlane.y + 
                                     this->mPlane.z * this->mPlane.z);
            return num/dom;
        }
        
        operator Vector4() const {
            return mPlane;
        }
        
        float a() const {
            return mPlane.x;
        }
        
        float b() const {
            return mPlane.y;
        }
        
        float c() const {
            return mPlane.z;
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
            this->mCenter.set(vec.x, vec.y, vec.z);
            this->mRadius = vec.w;
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
            return float_equal(this->mRadius, 0.f);
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
            ukn_assert(s != 0.0f);
            
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
            return Vector4(mCenter.x, mCenter.y, mCenter.z, mRadius);
        }
        
        bool operator==(const Sphere& rhs) const {
            return this->mCenter == rhs.mCenter && this->mRadius == rhs.mRadius;
        }
        
        bool vecInBound(const Vector3& point) {
            real distantce = (point - this->mCenter).sqrLength();
            
            return distantce <= mRadius * mRadius;
        }
        
        bool testPoint(const Vector3& point) {
            return vecInBound(point);
        }
        
    private:
        real mRadius;
        Vector3 mCenter;
    };
    
    
} // namespace ukn

#include "PreDeclare.h"

namespace ukn {
    
    inline Vector3 float3_to_vector3(const float3& f3) {
        return Vector3(f3[0],
                       f3[1],
                       f3[2]);
    }
    
    inline Vector2 float2_to_vector2(const float2& f2) {
        return Vector2(f2[0],
                       f2[1]);
    }
    
    inline Vector4 float4_to_vector4(const float4& f4) {
        return Vector4(f4[0],
                       f4[1],
                       f4[2],
                       f4[3]);
    }
    
    inline Vector4 get_matrix_col(const Matrix4& mat, uint32 index) {
        return Vector4(mat.c[index][0],
                       mat.c[index][1],
                       mat.c[index][2],
                       mat.c[index][3]);
    }
    
    static Matrix4 look_at_mat_lh(const Vector3& eye, const Vector3& lookAt, const Vector3& up) {
        Vector3 z = (lookAt - eye).normalize();
        Vector3 x = up.cross(z).normalize();
        Vector3 y = z.cross(x);
            
        Matrix4 mat;
        mat.x[0] = x.x;
        mat.x[1] = y.x;
        mat.x[2] = z.x;
        mat.x[3] = 0;
        
        mat.x[4] = x.y;
        mat.x[5] = y.y;
        mat.x[6] = z.y;
        mat.x[7] = 0;
        
        mat.x[8] = x.z;
        mat.x[9] = y.z;
        mat.x[10] = z.z;
        mat.x[11] = 0;
        
        mat.x[12] = x.dot(eye);
        mat.x[13] = y.dot(eye);
        mat.x[14] = z.dot(eye);
        mat.x[15] = 1;
        
        return mat;
    }
    
} // namespace ukn

#endif
