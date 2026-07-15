#include "perlin.hpp"
#include "config.hpp"
#include "random.hpp"

#include <spdlog/spdlog.h>
#include <array>
#include <cmath>
#include <cstdint>
#include <vector>

float Fade(float t) {
    return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
}

float Lerp(float a, float b, float t) {
    return a + (b - a) * t;
}

float BiLerp(const std::array<float, 4>& values, float t1, float t2) {
    t1 = Fade(t1);
    t2 = Fade(t2);
    float lerp1 = Lerp(values[0], values[1], t1);
    float lerp2 = Lerp(values[2], values[3], t1);
    return Lerp(lerp1, lerp2, t2);
}

std::vector<sf::Vector2f> GenerateVectors(int grid_size) {
    std::vector<sf::Vector2f> vectors;
    vectors.reserve((grid_size + 1) * (grid_size + 1));
    for (int i = 0; i < (grid_size + 1) * (grid_size + 1); ++i) {
        sf::Angle cur_angle = sf::radians(random_gen::GetFloat(0.0f, 2.0f * static_cast<float>(M_PI)));
        sf::Vector2f cur_vector = sf::Vector2f(1.0f, cur_angle);
        vectors.push_back(cur_vector);
    }
    return vectors;
}

std::vector<float> GenerateOctave(int grid_size) {
    std::vector<sf::Vector2f> vectors = GenerateVectors(grid_size);
    std::vector<float> pixel_values;
    pixel_values.reserve(config::window_size.x * config::window_size.y);
    for (int i = 0; i < config::window_size.y; ++i) {
        const float grid_y = (static_cast<float>(i) / config::window_size.y) * grid_size;
        const uint32_t cell_y = static_cast<uint32_t>(std::floor(grid_y));
        const float dy = grid_y - cell_y;

        for (int j = 0; j < config::window_size.x; ++j) {
            const float grid_x = (static_cast<float>(j) / config::window_size.x) * grid_size;
            const uint32_t cell_x = static_cast<uint32_t>(std::floor(grid_x));
            const float dx = grid_x - cell_x;

            std::array<sf::Vector2f, 4> vectors_to_pixel;
            vectors_to_pixel[0] = {dx, dy};
            vectors_to_pixel[1] = {dx - 1.0f, dy};
            vectors_to_pixel[2] = {dx, dy  - 1.0f};
            vectors_to_pixel[3] = {dx - 1.0f, dy  - 1.0f};

            std::array<sf::Vector2f, 4> corner_vectors;
            corner_vectors[0] = vectors[cell_y * (grid_size + 1) + cell_x];
            corner_vectors[1] = vectors[cell_y * (grid_size + 1) + (cell_x + 1)];
            corner_vectors[2] = vectors[(cell_y + 1) * (grid_size + 1) + cell_x];
            corner_vectors[3] = vectors[(cell_y + 1) * (grid_size + 1) + (cell_x + 1)];

            std::array<float, 4> dots;
            for (int k = 0; k < 4; ++k) {
                dots[k] = vectors_to_pixel[k].dot(corner_vectors[k]);
            }

            float pixel_value = 0.5f * (BiLerp(dots, dx, dy) * M_SQRT2 + 1.0f);
            pixel_values.push_back(pixel_value);
        }
    }
    return pixel_values;
}

std::vector<float> GeneratePerlinNoise() {
    std::vector<float> perlin_noise(config::window_size.x * config::window_size.y, 0.0f);
    for (int i = 0; i < config::layer_amount; ++i) {
        const int grid_size = config::initial_grid_size * (1 << i);
        if (grid_size > config::window_size.x || grid_size > config::window_size.y) {
            spdlog::warn("Perlin noise grid is bigger than window!");
            break;
        }
        std::vector<float> layer = GenerateOctave(grid_size);
        for (int j = 0; j < config::window_size.x * config::window_size.y; ++j) {
            perlin_noise[j] += layer[j] / (1 << i);
        }
    }
    for (float& pixel : perlin_noise) {
        pixel /= 2.0f - 1.0f / (1 << (config::layer_amount - 1));
    }
    return perlin_noise;
}