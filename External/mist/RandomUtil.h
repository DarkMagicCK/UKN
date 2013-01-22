//
//  RandomUtil.h
//  Mist
//
//  Created by Robert Bu on 12/7/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef MISTRandomUtil_h
#define MISTRandomUtil_h

#include "mist/Platform.h"
#include "mist/MathUtil.h"

namespace mist {
    
    class MIST_API Random {
    public:
        static void SetRandomSeed(uint32 seed);
        static uint32 GetRandomSeed();
        
        // 0 - 1
        static float NormalizedRandomFloat();
        
        static float RandomFloat(float min, float max);
        static int32 RandomInt(int32 min, int32 max);
    };
    
    /**
     * Global RandomGenerator
     * You can change the functions in it to make the random generator use your funcs instead of internal generator funcs
     **/
     struct MIST_API RandomGenerator {
        RandomGenerator();
        
        typedef void (*set_random_seed_func_ptr)(uint32 seed);
        typedef float (*normalized_random_func_ptr)();
        
        set_random_seed_func_ptr random_seed_func;
        normalized_random_func_ptr normalized_random_func;  
    };
    static RandomGenerator random_generator;
    
    /**
     * Specialize this to support your types
     **/
    template<typename T>
    struct Randomizer;
 
    template<typename T>
    class RandomObject {
    public:
        RandomObject() { }
        
        RandomObject(const T& mint, const T& maxt):
        min(mint),
        max(maxt) { }
        
        ~RandomObject() { }
        
        const T& getMin() const { return this->min; }
        const T& getMax() const { return this->max; }
        
        T randomize() { 
            return Randomizer<T>::Randomize(min, max);
        }
        
        void set(const T& min, const T& max) {
            this->min = min;
            this->max = max;
        }
        
        T min;
        T max;
    };
    
    template<> struct Randomizer<float> {
        MIST_API static float Randomize(float min, float max);
    };
    
    typedef RandomObject<float> RandomRange;
    
    template<> struct Randomizer<Vector2> {
        MIST_API static Vector2 Randomize(const Vector2& min, const Vector2& max);
    };
    
    template<> struct Randomizer<Vector3> {
        MIST_API static Vector3 Randomize(const Vector3& min, const Vector3& max);
    };
    typedef RandomObject<Vector2>   RandomArea;
    typedef RandomObject<Vector3>   RandomVolume;
    
    class Color;
    template<> struct Randomizer<Color> {
        MIST_API static Color Randomize(const Color& min, const Color& max);
    };
    
    typedef RandomObject<Color>     RandomColor;

} // namespace mist

#endif
