//
//  RandomUtil.cpp
//  Project Unknown
//
//  Created by Ruiwei Bu on 12/7/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#include "UKN/RandomUtil.h"
#include "UKN/Color.h"
#include "UKN/MathUtil.h"

#include <stdlib.h>

namespace ukn {
    
    static int32 g_current_seed = 0;
        
    void set_random_seed(uint32 seed) {
        g_current_seed = seed;
        random_generator.random_seed_func(seed);
    }
    
    uint32 get_random_seed() {
        return g_current_seed;
    }
    
    float normlized_random() {
        return random_generator.normalized_random_func();
    }
        
    static void set_random_seed_in(uint32 seed) {
        std::srand(seed);
    }
    
    static float normalized_random_in() {
        return std::rand() / (float)RAND_MAX;
    }
    
    RandomGenerator::RandomGenerator():
    random_seed_func(set_random_seed_in),
    normalized_random_func(normalized_random_in) {
        
    }
    
    float random_float(float min, float max) {
        return random_generator.normalized_random_func() * (max - min) + min;
    }
    
    int32 random_int(int32 min, int32 max) {
        return random_generator.normalized_random_func() * (max - min + 1) + min;
    }
    
    float Randomizer<float>::Randomize(float min, float max) {
        return random_float(min, max);
    }
    
    Vector2 Randomizer<Vector2>::Randomize(const Vector2& min, const Vector2& max) {
        return Vector2(random_float(min.x, max.x),
                       random_float(min.y, max.y));
    }
    
    Vector3 Randomizer<Vector3>::Randomize(const Vector3& min, const Vector3& max) {
        return Vector3(random_float(min.x, max.x),
                       random_float(min.y, max.y),
                       random_float(min.z, max.z));
    }
    
    Color Randomizer<Color>::Randomize(const Color& min, const Color& max) {
        return Color(random_float(min.r, max.r),
                     random_float(min.g, max.g),
                     random_float(min.b, max.b),
                     random_float(min.a, max.a));
    }
        
} // namespace ukn
