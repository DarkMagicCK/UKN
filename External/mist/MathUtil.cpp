#include "MathUtil.h"

namespace mist {


    static int _ukn_perlin_noise_perm[512] = {60,137,91,90,15,
        131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
        190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
        88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
        77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
        102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
        135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
        5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
        223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
        129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
        251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
        49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
        138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
    };

    float PerlinNoise::fade(float t) {
        return t*t*t*(t*(t*6.0f-15.0f)+10.f);
    }

    float PerlinNoise::lerp(float t, float a, float b) {
        return a + t * (b-a);
    }

    float PerlinNoise::grad(int hash, float x, float y, float z) {
        int h = hash & 15;
        float u = h < 8 ? x : y;
        float v = h < 4 ? y : ((h == 12) || (h == 14)) ? x : z;
        return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
    }

    float PerlinNoise::Gen(float x, float y, float z) {
        float floorX = floorf(x);
        float floorY = floorf(y);
        float floorZ = floorf(z);

        int X = int(floorX) & 255;
        int Y = int(floorY) & 255;
        int Z = int(floorZ) & 255;

        x -= floorX;
        y -= floorY;
        z -= floorZ;

        float u = fade(x);
        float v = fade(y);
        float w = fade(z);

        int A = _ukn_perlin_noise_perm[X] + Y;
        int AA = _ukn_perlin_noise_perm[A] + Z;
        int AB = _ukn_perlin_noise_perm[A+1] + Z;
        int B = _ukn_perlin_noise_perm[X+1] + Y;
        int BA = _ukn_perlin_noise_perm[B] + Z;
        int BB = _ukn_perlin_noise_perm[B+1] + Z;

        return lerp(w,  lerp(v, lerp(u, grad(_ukn_perlin_noise_perm[AA], x, y, z),
                                        grad(_ukn_perlin_noise_perm[BA], x-1, y, z)),
                                lerp(u, grad(_ukn_perlin_noise_perm[AB], x, y-1, z),
                                        grad(_ukn_perlin_noise_perm[BB], x-1, y-1, z))),
                        lerp(v, lerp(u, grad(_ukn_perlin_noise_perm[AA+1], x, y, z-1),
                                        grad(_ukn_perlin_noise_perm[BA+1], x-1, y, z-1)),
                                lerp(u, grad(_ukn_perlin_noise_perm[AB+1], x, y-1, z-1),
                                        grad(_ukn_perlin_noise_perm[BB+1], x-1, y-1, z-1))));    
    }


    namespace math {

        real degree_to_radius(real dgr) {
            return (real)((d_pi * dgr) / 180.0);
        }

        real radius_to_degree(real rad) {
            return (real)((180.0 * rad) / d_pi);
        }

        real sin_dgr(real dgr) {
            return (real)(sin(degree_to_radius(dgr)));
        }

        real cos_dgr(real dgr) {
            return (real)(cos(degree_to_radius(dgr)));
        }

        real tan_dgr(real dgr) {
            return (real)(tan(degree_to_radius(dgr)));
        }

        bool float_equal(real f1, real f2) {
            return fabs(f1 - f2) < MIST_TINY;
        }

        // fast inv sqrt by Join Carmack
        real fast_inv_sqrt(real x) {
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

        int is_pow_of_2(uint32 x) {
            return !(x & (x-1));
        }

        uint32 next_pow_of_2(uint32 x) {
            if ( is_pow_of_2(x) )
                return x;
            x |= x>>1;
            x |= x>>2;
            x |= x>>4;
            x |= x>>8;
            x |= x>>16;
            return x+1;
        }

        // compress a unit float to nBits integer
        // there would be some data loss but good for some condition
        // it is safe to cast the result to nBits integer, such as uint16 if you pass nBits as 16
        uint32 compress_unit_float(real unitFloat, uint32 nBits) {
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
        real decompress_unit_float(uint32 quantized, uint32 nBits) {
            // determine the number of intervals based on the number of bits
            uint32 nIntervals = 1u << nBits;

            // decode by simply converting the uint32 to an float, then scaling by the interval size
            real intervalSize = 1.f / (real)(nIntervals - 1u);

            real approxUnitFloat = (real)quantized * intervalSize;
            return approxUnitFloat;
        }

        // compress a float within a specific range
        uint32 compress_float(real value, real min, real max, uint32 nBits) {
            float unitFloat = (value - min) / (max - min);
            uint32 quantized = compress_unit_float(unitFloat, nBits);
            return quantized;
        }

        real decompress_float(uint32 quantized, real min, real max, uint32 nBits) {
            real unitFloat = decompress_unit_float(quantized, nBits);
            real value = min + (unitFloat * (max - min));
            return value;
        }

        uint16 flip_bytes(uint16 value) {
            return ((value >> 8) & 0x00FF) | ((value << 8) & 0xFF00);
        }

        uint32 flip_bytes(uint32 value) {
            return ((value >> 24) & 0x000000FF) | ((value >> 8) & 0x0000FF00) |
                ((value << 8) & 0x00FF0000) | ((value << 24) & 0xFF000000);
        }

        int16 flip_bytes(int16 value) {
            return int16(flip_bytes(uint16(value)));
        }

        int32 flip_bytes(int32 value) {
            return int32(flip_bytes(uint32(value)));
        }

        Vector4 get_matrix_col(const Matrix4& mat, uint32 index) {
            return Vector4(mat.c[index][0],
                           mat.c[index][1],
                           mat.c[index][2],
                           mat.c[index][3]);
        }

        Vector3 transform_quaternion(const Vector3& v, const Quaternion& q) {
            
            Vector3 qv = Vector3(q.vec.x(), q.vec.y(), q.vec.z());
            float a(q.w() * q.w() - qv.sqrLength());
            float b(2 * qv.dot(v));
            float c(q.w() + q.w());

            Vector3 cross_v(qv.cross(v));
            return Vector3(a * v.x() + b * q.x() + c * cross_v.x(),
                a * v.y() + b * q.y() + c * cross_v.y(),
                a * v.z() + b * q.z() + c * cross_v.z());
        }

        Quaternion inverse_quaternion(const Quaternion& q) {
            float inv(1.f / q.length());
            return Quaternion(-q.x() * inv, -q.y() * inv, -q.z ()* inv, q.w() * inv);
        }


        float3 cross(const float3& lhs, const float3& rhs) {
            return float3(-lhs[2]*rhs[1] + lhs[1]*rhs[2], 
                          lhs[2]*rhs[0] - lhs[0]*rhs[2], 
                          -lhs[1]*rhs[0] + lhs[0]*rhs[1]);
        }

        void translate(Matrix4& lhs, const float3& rhs) {
            lhs.translate(rhs[0], rhs[1], rhs[2]);
        }
    }

    AABB2::AABB2(real _x1, real _y1, real _x2, real _y2, bool asWH):
        x1(_x1), y1(_y1),
        x2(asWH ? _x1 + _x2: _x2), y2(asWH ? _y1 + _y2 : _y2) {

    }

    AABB2::AABB2():
        x1(0), y1(0),
        x2(0), y2(0) {   
    }

    AABB2::AABB2(const Vector2& upper, const Vector2& lower):
        x1(upper.x()), y1(upper.y()),
        x2(lower.x()), y2(lower.y()) {

    }

    AABB2::AABB2(const Vector2& pos, real r) {
        setAsBox(pos.x(), pos.y(), r);
    }

    AABB2::AABB2(const AABB2& rhs) {
        this->x1 = rhs.x1;
        this->y1 = rhs.y1;
        this->x2 = rhs.x2;
        this->y2 = rhs.y2;
    }

    void AABB2::set(const Vector2& upper, const Vector2& lower) {
        this->x1 = upper.x();
        this->y1 = upper.y();
        this->x2 = lower.x();
        this->y2 = lower.y();
    }

    void AABB2::setXY(real _x1, real _y1) {
        this->x2 = _x1 + this->width();
        this->y2 = _y1 + this->height();
        this->x1 = _x1;
        this->y1 = _y1;
    }

    void AABB2::set(real _x1, real _y1, real _x2, real _y2, bool asWH) {
        this->x1 = _x1; 
        this->y1 = _y1;
        this->x2 = asWH ? _x1 + _x2 : _x2;
        this->y2 = asWH ? _y1 + _y2 : _y2;
    }

    void AABB2::setAsBox(real x, real y, real r) {
        this->x1 = x - r;
        this->y1 = y - r;
        this->x2 = x + r;
        this->y2 = y + r;
    }

    AABB2& AABB2::operator=(const AABB2& rhs) {
        if(this != &rhs) {
            this->x1 = rhs.x1;
            this->y1 = rhs.y1;
            this->x2 = rhs.x2;
            this->y2 = rhs.y2;
        }
        return *this;
    }

    void AABB2::encapsulate(real x, real y) {
        if(x<this->x1) this->x1 = x;
        if(x>this->x2) this->x2 = x;
        if(y<this->y1) this->y1 = y;
        if(y>this->y2) this->y2 = y;
    }

    bool AABB2::contains(real x, real y) const {
        return x>=x1 && x<=x2 && y>=y1 && y<=y2;
    }

    bool AABB2::contains(const Vector2& pos) const {
        return contains(pos.x(), pos.y());
    }
    
    bool AABB2::contains(const AABB2& r, real margin) const {
        return (r.x() - margin >= this->x() && r.y() - margin >= this->y() &&
                (r.x() + r.width() + margin) <= (this->x() + this->width()) &&
                (r.y() + r.height() + margin) <= (this->y() + this->height()));
    }

    bool AABB2::intersect(const AABB2& rhs) const {
        return ((rhs.x() + rhs.width()) > this->x() && (rhs.x() < (this->x() + this->width())) &&
                (rhs.y() + rhs.height()) > this->y() && (rhs.y() < (this->y() + this->height())));
    }

    bool AABB2::isEmpty() const {
        return (x1 == x2) || (y1 == y2);
    }

    void AABB2::extend(const AABB2& rhs) {
        if(this->x1 > rhs.x1)
            this->x1 = rhs.x1;
        if(this->y1 > rhs.y1)
            this->y1 = rhs.y1;
        if(this->x2 < rhs.x2)
            this->x2 = rhs.x2;
        if(this->y2 < rhs.y2)
            this->y2 = rhs.y2;
    }

    Vector2 AABB2::getPosition() const {
        return Vector2(this->x1, this->y1);
    }

    Quaternion::Quaternion(real _x, real _y, real _z, real _w):
        vec(Vector4(_x, _y, _z, _w)) {
    }

    Quaternion::Quaternion(const Quaternion& quad):
        vec(quad.vec) {
    }

    Quaternion::Quaternion() {
    }

    Quaternion::Quaternion(real eulerX, real eulerY, real eulerZ) {
        Quaternion roll( sinf( eulerX * 0.5f ), 0, 0, cosf( eulerX * 0.5f ) );
        Quaternion pitch( 0, sinf( eulerY * 0.5f ), 0, cosf( eulerY * 0.5f ) );
        Quaternion yaw( 0, 0, sinf( eulerZ * 0.5f ), cosf( eulerZ * 0.5f ) );

        // Order: y * x * z
        *this = pitch * roll * yaw;
    }

    void Quaternion::set(real _x, real _y, real _z, real _w) {
        vec.set(_x, _y, _z, _w);
    }

    void Quaternion::set(const Quaternion& quad) {
        vec = quad.vec;
    }

    float Quaternion::length() const {
        return vec.length();
    }

    Quaternion& Quaternion::operator=(const Quaternion& rhs) {
        this->set(rhs);
        return *this;
    }

    Quaternion Quaternion::operator*(const Quaternion& rhs) const {
        return Quaternion(
            this->x() * rhs.w() - this->y() * rhs.z() + this->z() * rhs.y() + this->w() * rhs.x(),
            this->x() * rhs.z() + this->y() * rhs.w() - this->z() * rhs.x() + this->w() * rhs.y(),
            this->y() * rhs.x() - this->x() * rhs.y() + this->z() * rhs.w() + this->w() * rhs.z(),
            this->w() * rhs.w() - this->x() * rhs.x() - this->y() * rhs.y() - this->z() * rhs.z());
    }

    Quaternion& Quaternion::operator*=(const Quaternion& rhs) {
        *this = *this * rhs;
        return *this;
    }

    void Quaternion::makeRotate(real angle, real _x, real _y, real _z) {
        real inversenorm = (real)1.0f/sqrtf(x()*x() + y()*y() + z()*z());
        real coshalfangle = cosf(0.5f*angle);
        real sinhalfangle = sinf(0.5f*angle);

        this->vec.set(_x * sinhalfangle * inversenorm,
            _y * sinhalfangle * inversenorm,
            _z * sinhalfangle * inversenorm,
            coshalfangle);
    }

    void Quaternion::makeRotate(real angle, const Vector3& vec) {
        makeRotate(angle, vec.x(), vec.y(), vec.z());
    }

    void Quaternion::makeRotate(real roll, real pitch, real yaw) {
        real sh0 = sinf(0.5f*roll);
        real ch0 = cosf(0.5f*roll);
        real sh1 = sinf(0.5f*pitch);
        real ch1 = cosf(0.5f*pitch);
        real sh2 = sinf(0.5f*yaw);
        real ch2 = cosf(0.5f*yaw);

        this->vec.set(sh0*ch1*ch2 + ch0*sh1*sh2,
            ch0*sh2*ch1 + sh0*ch2*sh1,
            ch0*ch2*sh1 - sh0*sh2*ch1,
            ch0*ch1*ch2 - sh0*sh1*sh2);
    }

    Vector3 Quaternion::rotate(const Vector3& v) const {
        real ux = this->w()*v.x() + this->y()*v.z() - this->z()*v.y();
        real uy = this->w()*v.y() + this->z()*v.x() - this->x()*v.z();
        real uz = this->w()*v.z() + this->x()*v.y() - this->y()*v.x();
        real uw = -this->x()*v.x() - this->y()*v.y() - this->z()*v.z();
        real vx = -uw*this->x() + ux*this->w() - uy*this->z() + uz*this->y();
        real vy = -uw*this->y() + uy*this->w() - uz*this->x() + ux*this->z();
        real vz = -uw*this->z() + uz*this->w() - ux*this->y() + uz*this->x();
        return Vector3(vx, vy, vz);
    }

    void Quaternion::getRotate(real& angle, real& _x, real& _y, real& _z) const {
        real sinhalfangle = sqrtf(this->x()*this->x() + 
            this->y()*this->y() + 
            this->z()*this->z());
        angle = 2.f * atan2f(sinhalfangle, this->w());
        if(sinhalfangle) {
            _x = x() / sinhalfangle;
            _y = y() / sinhalfangle;
            _z = z() / sinhalfangle;
        } else {
            _x = 0.0f;
            _y = 0.0f;
            _z = 0.0f;
        }
    }

    void Quaternion::getRotate(real& angle, Vector3& vec) const {
        getRotate(angle, vec.x(), vec.y(), vec.z());
    }

    void Quaternion::slerp(real t, const Quaternion& from, const Quaternion& to) {
        const real epsilon = 0.00001f;
        real cosomega, scale_from, scale_to;

        Quaternion quatTo(to);

        cosomega = from.x()*to.x() + 
            from.y()*to.y() + 
            from.z()*to.z() + 
            from.w()*to.w();
        if(cosomega < 0.0f) {
            cosomega = -cosomega;
            quatTo.set(-to.x(), -to.y(), -to.z(), -to.w());
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

        this->vec.set(from.x()*scale_from + quatTo.x()*scale_to,
            from.y()*scale_from + quatTo.y()*scale_to,
            from.z()*scale_from + quatTo.z()*scale_to,
            from.w()*scale_from + quatTo.w()*scale_to);
    }

    Matrix4 Matrix4::TransMat( real x, real y, real z ) {
        Matrix4 m;

        m.c[0][3] = x;
        m.c[1][3] = y;
        m.c[2][3] = z;

        return m;
    }

    Matrix4 Matrix4::ScaleMat( real x, real y, real z ) {
        Matrix4 m;

        m.c[0][0] = x;
        m.c[1][1] = y;
        m.c[2][2] = z;

        return m;
    }

    Matrix4 Matrix4::RotMat( real x, real y, real z ) {
        // Rotation order: YXZ [* Vector]
        return Matrix4( Quaternion( x, y, z ) );
    }

    Matrix4 Matrix4::RotMat( Vector3 axis, real angle ) {
        axis = axis * sinf( angle * 0.5f );
        return Matrix4( Quaternion( axis.x(), axis.y(), axis.z(), cosf( angle * 0.5f ) ) );
    }

    Matrix4 Matrix4::RotX(real x) {
        real sx = sin(x), cx = cos(x);

        Matrix4 m;
        m.c[1][1] = cx;
        m.c[1][2] = -sx;
        m.c[2][1] = sx;
        m.c[2][2] = cx;
        return m;
    }

    Matrix4 Matrix4::RotY(real y) {
        real sy = sin(y), cy = cos(y);

        Matrix4 m;
        m.c[0][0] = cy;
        m.c[0][2] = sy;
        m.c[3][0] = -sy;
        m.c[3][2] = cy;
        return m;
    }

    Matrix4 Matrix4::RotZ(real z) {
        real sz = sin(z), cz = cos(z);

        Matrix4 m;
        m.c[0][0] = cz;
        m.c[0][1] = -sz;
        m.c[1][0] = sz;
        m.c[1][1] = cz;
        return m;
    }

    Matrix4 Matrix4::LHToRH(const Matrix4& mat) {
        Matrix4 result = mat;
        result.c[0][2] = -result.c[0][2];
        result.c[1][2] = -result.c[1][2];
        result.c[2][2] = -result.c[2][2];
        result.c[3][2] = -result.c[3][2];
        return result;
    }

    Matrix4 Matrix4::PerspectiveFovMatLH(real field_of_view, real aspect, real n, real f) {
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

    Matrix4 Matrix4::PerspectiveFovMatRH(real field_of_view, real aspect, real n, real f) {
        return LHToRH(PerspectiveFovMatLH(field_of_view, aspect, n, f));
    }

    Matrix4 Matrix4::PerspectiveMatLH(real width, real height, real n, real f) {
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

    Matrix4 Matrix4::PerspectiveMatRH(real width, real height, real n, real f) {
        return LHToRH(PerspectiveMatLH(width, height, n, f ));
    }

    Matrix4 Matrix4::PerspectiveOffCenterMatLH(real l, real r, real b, real t, real n, real f) {
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

    Matrix4 Matrix4::PerspectiveOffCenterMatRH(real l, real r, real b, real t, real n, real f) {
        return LHToRH(PerspectiveOffCenterMatLH(l, r, b, t, n, f));
    }

    Matrix4 Matrix4::OrthoMatLH(real w, real h, real n, real f) {
        return OrthoOffCenterMatLH(-w/2, w/2, -h/2, h/2, n, f);
    }

    Matrix4 Matrix4::OrthoMatRH(real w, real h, real n, real f) {
        return LHToRH(OrthoMatLH(w, h, n, f));
    }

    Matrix4 Matrix4::OrthoOffCenterMatLH(real l, real r, real b, real t, real n, real f ) {
        Matrix4 m;

        m.c[0][0] = 2 / (r - l);
        m.c[1][1] = 2 / (t - b);
        m.c[2][2] = 1 / (f - n);
        m.c[0][3] = -(r + l) / (r - l);
        m.c[1][3] = -(t + b) / (t - b);
        m.c[2][3] = -(n) / (f - n);

        return m;
    }

    Matrix4 Matrix4::OrthoOffCenterMatRH(real l, real r, real b, real t, real n, real f) {
        return LHToRH(OrthoOffCenterMatLH(l , r, b, t, n, f));
    }

    Matrix4 Matrix4::LookAtMatLH(const Vector3& eye, const Vector3& lookAt, const Vector3& up) {
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

    Matrix4 Matrix4::LookAtMatRH(const Vector3& eye, const Vector3& lookAt, const Vector3& up) {
        return LHToRH(LookAtMatLH(eye, lookAt, up));
    }

    Matrix4 Matrix4::FastMult43( const Matrix4 &m1, const Matrix4 &m2 ) {
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

    Matrix4::Matrix4() {
        c[0][0] = 1.f; c[1][0] = 0.f; c[2][0] = 0.f; c[3][0] = 0.f;
        c[0][1] = 0.f; c[1][1] = 1.f; c[2][1] = 0.f; c[3][1] = 0.f;
        c[0][2] = 0.f; c[1][2] = 0.f; c[2][2] = 1.f; c[3][2] = 0.f;
        c[0][3] = 0.f; c[1][3] = 0.f; c[2][3] = 0.f; c[3][3] = 1.f;
    }

    Matrix4::Matrix4( const real *realArray16 ) {
        for( unsigned int i = 0; i < 4; ++i ) {
            for( unsigned int j = 0; j < 4; ++j ) {
                this->c[i][j] = realArray16[i * 4 + j];
            }
        }
    }

    Matrix4::Matrix4( const Quaternion &q ) {
        // Calculate coefficients
        real x2 = q.x() + q.x(), y2 = q.y() + q.y(), z2 = q.z() + q.z();
        real xx = q.x() * x2,  xy = q.x() * y2,  xz = q.x() * z2;
        real yy = q.y() * y2,  yz = q.y() * z2,  zz = q.z() * z2;
        real wx = q.w() * x2,  wy = q.w() * y2,  wz = q.w() * z2;

        this->c[0][0] = 1 - (yy + zz);  this->c[1][0] = xy - wz;	
        this->c[2][0] = xz + wy;        this->c[3][0] = 0;
        this->c[0][1] = xy + wz;        this->c[1][1] = 1 - (xx + zz);
        this->c[2][1] = yz - wx;        this->c[3][1] = 0;
        this->c[0][2] = xz - wy;        this->c[1][2] = yz + wx;
        this->c[2][2] = 1 - (xx + yy);  this->c[3][2] = 0;
        this->c[0][3] = 0;              this->c[1][3] = 0;
        this->c[2][3] = 0;              this->c[3][3] = 1;
    }

    Matrix4::Matrix4(real x11, real x12, real x13, real x14,
        real x21, real x22, real x23, real x24,
        real x31, real x32, real x33, real x34,
        real x41, real x42, real x43, real x44) {
            this->c[0][0] = x11; this->c[1][0] = x12; this->c[2][0] = x13; this->c[3][0] = x14;
            this->c[0][1] = x21; this->c[1][1] = x22; this->c[2][1] = x23; this->c[3][1] = x24;
            this->c[0][2] = x31; this->c[1][2] = x32; this->c[2][2] = x33; this->c[3][2] = x34;
            this->c[0][3] = x41; this->c[1][3] = x42; this->c[2][3] = x43; this->c[3][3] = x44;
    }

    Matrix4 Matrix4::operator+( const Matrix4 &m ) const {
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

    Matrix4 &Matrix4::operator+=( const Matrix4 &m ) {
        return *this = *this + m;
    }

    Matrix4 Matrix4::operator*( const Matrix4 &m ) const  {
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

    Matrix4& Matrix4::operator*=( const Matrix4& m) {
        *this = *this * m;
        return *this;
    }

    Matrix4 Matrix4::operator*( const real f ) const {
        Matrix4 m( *this );

        m.x[0]  *= f; m.x[1]  *= f; m.x[2]  *= f; m.x[3]  *= f;
        m.x[4]  *= f; m.x[5]  *= f; m.x[6]  *= f; m.x[7]  *= f;
        m.x[8]  *= f; m.x[9]  *= f; m.x[10] *= f; m.x[11] *= f;
        m.x[12] *= f; m.x[13] *= f; m.x[14] *= f; m.x[15] *= f;

        return m;
    }

    Vector3 Matrix4::operator*( const Vector3 &v ) const {
        return Vector3( v.x() * c[0][0] + v.y() * c[1][0] + v.z() * c[2][0] + c[3][0],
            v.x() * c[0][1] + v.y() * c[1][1] + v.z() * c[2][1] + c[3][1],
            v.x() * c[0][2] + v.y() * c[1][2] + v.z() * c[2][2] + c[3][2] );
    }

    Vector4 Matrix4::operator*( const Vector4 &v ) const {
        return Vector4( v.x() * c[0][0] + v.y() * c[1][0] + v.z() * c[2][0] + v.w() * c[3][0],
            v.x() * c[0][1] + v.y() * c[1][1] + v.z() * c[2][1] + v.w() * c[3][1],
            v.x() * c[0][2] + v.y() * c[1][2] + v.z() * c[2][2] + v.w() * c[3][2],
            v.x() * c[0][3] + v.y() * c[1][3] + v.z() * c[2][3] + v.w() * c[3][3] );
    }

    Vector3 Matrix4::mult33Vec( const Vector3 &v ) const {
        return Vector3( v.x() * c[0][0] + v.y() * c[1][0] + v.z() * c[2][0],
            v.x() * c[0][1] + v.y() * c[1][1] + v.z() * c[2][1],
            v.x() * c[0][2] + v.y() * c[1][2] + v.z() * c[2][2] );
    }

    Matrix4& Matrix4::translate( const real x, const real y, const real z ) {
        *this = *this * TransMat( x, y, z );
        return *this;
    }

    Matrix4& Matrix4::scale( const real x, const real y, const real z ) {
        *this = *this * ScaleMat( x, y, z );
        return *this;
    }

    Matrix4& Matrix4::rotate( const real x, const real y, const real z ) {
        *this = *this * RotMat( x, y, z );
        return *this;
    }

    Matrix4 Matrix4::transposed() const {
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

    real Matrix4::determinant() const {
        return 
            c[0][3]*c[1][2]*c[2][1]*c[3][0] - c[0][2]*c[1][3]*c[2][1]*c[3][0] - c[0][3]*c[1][1]*c[2][2]*c[3][0] + c[0][1]*c[1][3]*c[2][2]*c[3][0] +
            c[0][2]*c[1][1]*c[2][3]*c[3][0] - c[0][1]*c[1][2]*c[2][3]*c[3][0] - c[0][3]*c[1][2]*c[2][0]*c[3][1] + c[0][2]*c[1][3]*c[2][0]*c[3][1] +
            c[0][3]*c[1][0]*c[2][2]*c[3][1] - c[0][0]*c[1][3]*c[2][2]*c[3][1] - c[0][2]*c[1][0]*c[2][3]*c[3][1] + c[0][0]*c[1][2]*c[2][3]*c[3][1] +
            c[0][3]*c[1][1]*c[2][0]*c[3][2] - c[0][1]*c[1][3]*c[2][0]*c[3][2] - c[0][3]*c[1][0]*c[2][1]*c[3][2] + c[0][0]*c[1][3]*c[2][1]*c[3][2] +
            c[0][1]*c[1][0]*c[2][3]*c[3][2] - c[0][0]*c[1][1]*c[2][3]*c[3][2] - c[0][2]*c[1][1]*c[2][0]*c[3][3] + c[0][1]*c[1][2]*c[2][0]*c[3][3] +
            c[0][2]*c[1][0]*c[2][1]*c[3][3] - c[0][0]*c[1][2]*c[2][1]*c[3][3] - c[0][1]*c[1][0]*c[2][2]*c[3][3] + c[0][0]*c[1][1]*c[2][2]*c[3][3];
    }

    Matrix4 Matrix4::inverted() const {
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

    void Matrix4::decompose( Vector3 &trans, Vector3 &rot, Vector3 &scale ) const {
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

    Vector4 Matrix4::getCol( unsigned int col ) const {
        return Vector4( x[col * 4 + 0], x[col * 4 + 1], x[col * 4 + 2], x[col * 4 + 3] );
    }

    Vector4 Matrix4::getRow( unsigned int row ) const {
        return Vector4( x[row + 0], x[row + 4], x[row + 8], x[row + 12] );
    }

    Vector3 Matrix4::getTrans() const {
        return Vector3( c[3][0], c[3][1], c[3][2] );
    }

    Vector3 Matrix4::getScale() const {
        Vector3 scale;
        // Scale is length of columns
        scale.x() = sqrtf( c[0][0] * c[0][0] + c[0][1] * c[0][1] + c[0][2] * c[0][2] );
        scale.y() = sqrtf( c[1][0] * c[1][0] + c[1][1] * c[1][1] + c[1][2] * c[1][2] );
        scale.z() = sqrtf( c[2][0] * c[2][0] + c[2][1] * c[2][1] + c[2][2] * c[2][2] );
        return scale;
    }

    AABB3::AABB3(): 
        lower_left(Vector3()), 
        upper_right(Vector3()), 
        boundingRadius(0.f) { 
    }

    AABB3::AABB3(const Vector3& u, const Vector3& l): 
        lower_left(u), 
        upper_right(l), 
        boundingRadius((upper_right - lower_left).length() / 2.0f) { 
    }

    AABB3::AABB3(const AABB3& rhs): 
        lower_left(rhs.lower_left), 
        upper_right(rhs.upper_right), 
        boundingRadius(rhs.boundingRadius) {
    }

    AABB3& AABB3::operator=(const AABB3& rhs) {
        if(this != &rhs) {
            this->lower_left = rhs.lower_left;
            this->upper_right = rhs.upper_right;
            this->boundingRadius = rhs.boundingRadius;
        }
        return *this;
    }

    void AABB3::set(const Vector3& min, const Vector3& max) {
        this->lower_left = min;
        this->upper_right = max;
        this->boundingRadius = (this->upper_right - this->lower_left).length() / 2.0f;
    }
    void AABB3::set(const Vector3& min, const Vector3& max, real boundingRadius) {
        this->lower_left = min;
        this->upper_right = max;
        this->boundingRadius = boundingRadius;
    }

    real AABB3::getBoudingRadius() const {
        return this->boundingRadius;
    }
    Vector3 AABB3::getCenter() const {
        return (this->upper_right - this->lower_left) / 2.0f;
    }

    void AABB3::transform(const Matrix4& mat) {
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

    void AABB3::extend(const AABB3& rhs) {
        if(this->lower_left.x() > rhs.lower_left.x())
            this->lower_left.x() = rhs.lower_left.x();
        if(this->lower_left.y() > rhs.lower_left.y())
            this->lower_left.y() = rhs.lower_left.y();
        if(this->lower_left.z() > rhs.lower_left.z()) 
            this->lower_left.z() = rhs.lower_left.z();
        if(this->upper_right.x() < rhs.upper_right.x())
            this->upper_right.x() = rhs.upper_right.x();
        if(this->upper_right.y() < rhs.upper_right.y())
            this->upper_right.y() = rhs.upper_right.y();
        if(this->upper_right.z() < rhs.upper_right.z()) 
            this->upper_right.z() = rhs.upper_right.z();
    }

    const Vector3& AABB3::getMin() const {
        return this->lower_left;
    }
    const Vector3& AABB3::getMax() const {
        return this->upper_right;
    }

    Vector3& AABB3::getMin() {
        return this->lower_left;
    }
    Vector3& AABB3::getMax() {
        return this->upper_right; 
    }

    real AABB3::getWidth() const {
        return abs(this->upper_right.x() - this->lower_left.x());
    }
    real AABB3::getHeight() const {
        return abs(this->upper_right.y() - this->lower_left.y());
    }
    real AABB3::getDepth() const {
        return abs(this->upper_right.z() - this->lower_left.z());
    }

    void AABB3::scale(float scale) {
        Vector3 center = this->getCenter();
        Vector3 dist = (this->upper_right - center) / 2.0 * scale;

        set(center - dist, center + dist);
    }

    Plane::Plane() { }

    Plane::Plane(const Plane& rhs):
        mPlane(rhs.mPlane) { }

    Plane::Plane(const Vector4& vec):
        mPlane(vec) { }

    Plane::Plane(real x, real y, real z, real w) {
        mPlane.set(x, y, z, w);
    }

    Plane& Plane::operator=(const Plane& rhs) {
        if(this != &rhs) {
            this->mPlane = rhs.mPlane;
        }
        return *this;
    }

    Plane& Plane::operator=(const Vector4& vec) {
        mPlane = vec;
        return *this;
    }

    bool Plane::operator==(const Plane& rhs) const {
        return this->mPlane.equals(rhs.mPlane);
    }

    void Plane::set(const Vector4& vec) {
        this->mPlane = vec;
    }

    Plane Plane::operator-() const {
        return Plane(-this->mPlane);
    }

    Vector4 Plane::get() const {
        return Vector4(this->mPlane.x(), this->mPlane.y(), this->mPlane.z(), this->mPlane.w());
    }

    real Plane::dot(const Vector3& rhs) const {
        return this->a() * rhs.x() + this->b() * rhs.y() + this->c() * rhs.z() + this->d();
    }

    real Plane::getDistance(const Vector3 point) {
        real num = this->mPlane.dot(Vector4(point));
        real dom = mist::math::fast_inv_sqrt(this->mPlane.x() * this->mPlane.x() + 
            this->mPlane.y() * this->mPlane.y() + 
            this->mPlane.z() * this->mPlane.z());
        return num/dom;
    }

    Sphere::Sphere():
        mRadius(0.f),
        mCenter(Vector3()) { }

    Sphere::Sphere(const Vector3& pos, real radius):
        mRadius(radius),
        mCenter(pos) { }

    Sphere::Sphere(const Vector4& vec) {
        this->mCenter.set(vec.x(), vec.y(), vec.z());
        this->mRadius = vec.w();
    }

    const real& Sphere::radius() const {
        return this->mRadius;
    }

    real& Sphere::radius() {
        return this->mRadius;
    }

    const Vector3& Sphere::center() const {
        return this->mCenter;
    }

    Vector3& Sphere::center() {
        return this->mCenter;
    }

    bool Sphere::isEmpty() const {
        return math::float_equal(this->mRadius, 0.f);
    }

    Sphere& Sphere::operator+=(const Vector3& trans) {
        this->mCenter += trans;
        return *this;
    }

    Sphere& Sphere::operator-=(const Vector3& trans) {
        this->mCenter -= trans;
        return *this;
    }

    Sphere& Sphere::operator*=(real s) {
        this->mRadius *= s;
        return *this;
    }

    Sphere& Sphere::operator/=(real s) {
        mist_assert(s != 0.0f);

        this->mRadius /= s;
        return *this;
    }

    Sphere& Sphere::operator=(const Sphere& rhs) {
        if(this != &rhs) {
            this->mRadius = rhs.mRadius;
            this->mCenter = rhs.mCenter;
        }
        return *this;
    }

    Vector4 Sphere::asVector4() const {
        return Vector4(mCenter.x(), mCenter.y(), mCenter.z(), mRadius);
    }

    bool Sphere::operator==(const Sphere& rhs) const {
        return this->mCenter.equals(rhs.mCenter) && 
            this->mRadius == rhs.mRadius;
    }

    bool Sphere::vecInBound(const Vector3& point) {
        real distantce = (point - this->mCenter).sqrLength();

        return distantce <= mRadius * mRadius;
    }

    bool Sphere::contains(const Vector3& point) {
        return vecInBound(point);
    }
}