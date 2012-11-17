//
//  RandomUtil.cpp
//  Mist
//
//  Created by Robert Bu on 12/7/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#include "mist/RandomUtil.h"
#include "mist/Color.h"
#include "mist/MathUtil.h"

#include <stdlib.h>

namespace mist {
    
    static int32 g_current_seed = 0;
        
    void Random::SetRandomSeed(uint32 seed) {
        g_current_seed = seed;
        random_generator.random_seed_func(seed);
    }
    
    uint32 Random::GetRandomSeed() {
        return g_current_seed;
    }
    
    float Random::NormalizedRandomFloat() {
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
    
    float Random::RandomFloat(float min, float max) {
        return random_generator.normalized_random_func() * (max - min) + min;
    }
    
    int32 Random::RandomInt(int32 min, int32 max) {
        return random_generator.normalized_random_func() * (max - min + 1) + min;
    }
    
    float Randomizer<float>::Randomize(float min, float max) {
        return Random::RandomFloat(min, max);
    }
    
    Vector2 Randomizer<Vector2>::Randomize(const Vector2& min, const Vector2& max) {
        return Vector2(Random::RandomFloat(min.x, max.x),
                       Random::RandomFloat(min.y, max.y));
    }
    
    Vector3 Randomizer<Vector3>::Randomize(const Vector3& min, const Vector3& max) {
        return Vector3(Random::RandomFloat(min.x, max.x),
                       Random::RandomFloat(min.y, max.y),
                       Random::RandomFloat(min.z, max.z));
    }
    
    Color Randomizer<Color>::Randomize(const Color& min, const Color& max) {
        return Color(Random::RandomFloat(min.r, max.r),
                     Random::RandomFloat(min.g, max.g),
                     Random::RandomFloat(min.b, max.b),
                     Random::RandomFloat(min.a, max.a));
    }
        
} // namespace mist
