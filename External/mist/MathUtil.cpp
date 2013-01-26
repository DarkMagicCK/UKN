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

        Vector3 float3_to_vector3(const float3& f3) {
            return Vector3(f3[0],
                f3[1],
                f3[2]);
        }

        Vector2 float2_to_vector2(const float2& f2) {
            return Vector2(f2[0],
                f2[1]);
        }

        Vector4 float4_to_vector4(const float4& f4) {
            return Vector4(f4[0],
                f4[1],
                f4[2],
                f4[3]);
        }

        Vector4 get_matrix_col(const Matrix4& mat, uint32 index) {
            return Vector4(mat.c[index][0],
                mat.c[index][1],
                mat.c[index][2],
                mat.c[index][3]);
        }

        Vector3 transform_quaternion(const Vector3& v, const Quaternion& q) {
            float a(q.w * q.w - q.get().length());
            float b(2 * v.dot(q.get()));
            float c(q.w + q.w);

            Vector3 cross_v(q.get().cross(v));
            return Vector3(a * v.x() + b * q.x + c * cross_v.x(),
                a * v.y() + b * q.y + c * cross_v.y(),
                a * v.z() + b * q.z + c * cross_v.z());
        }

        Quaternion inverse_quaternion(const Quaternion& q) {
            float inv(1.f / q.length());
            return Quaternion(-q.x * inv, -q.y * inv, -q.z * inv, q.w * inv);
        }
    }
}