#pragma once

#include "config.hpp"

#include <random>

namespace random_gen {
    inline std::random_device rd;

    inline std::mt19937 generator(rd());

    inline float GetFloat(float min_value, float max_value) {
        std::uniform_real_distribution<float> distribution(min_value, max_value);
        return distribution(generator);
    }
}