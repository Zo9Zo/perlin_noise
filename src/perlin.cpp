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

float BiLerp(const std::array<float, 4>& values, const std::array<float, 3>& t) {
    float lerp1 = Lerp(values[0], values[1], Fade(t[0]));
    float lerp2 = Lerp(values[2], values[3], Fade(t[1]));
    return Lerp(lerp1, lerp2, Fade(t[2]));
}

std::vector<sf::Vector2f> GenerateVectors(int grid_size) {
    std::vector<sf::Vector2f> vectors;
    for (int i = 0; i < (grid_size + 1) * (grid_size + 1); ++i) {
        sf::Angle cur_angle = sf::radians(random_gen::GetFloat(0.0f, 2 * M_PI));
        sf::Vector2 cur_vector = sf::Vector2f(1.0f, cur_angle);
        vectors.push_back(cur_vector);
    }
    return vectors;
}

std::vector<float> GenerateOctave(int grid_size) {
    std::vector<sf::Vector2f> vectors = GenerateVectors(grid_size);
    const sf::Vector2u cell_size = {config::window_size.x / grid_size, config::window_size.y / grid_size};
    std::vector<float> pixel_values;
    for (int i = 0; i < config::window_size.y; ++i) {
        const uint32_t cell_y = i / cell_size.y;
        const float dy = static_cast<float>(i % cell_size.y) / cell_size.y;

        for (int j = 0; j < config::window_size.x; ++j) {
            const uint32_t cell_x = j / cell_size.x;
            const float dx = static_cast<float>(j % cell_size.x) / cell_size.x;

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

            float pixel_value = 0.5f * (BiLerp(dots, {dx, dx, dy}) * M_SQRT1_2 + 1.0f);
            pixel_values.push_back(pixel_value);
        }
    }
    return pixel_values;
}

std::vector<float> GeneratePerlinNoise() {
    std::vector<float> perlin_noise(config::window_size.x * config::window_size.y, 0.0f);
    for (int i = 0; i < config::layer_amount; ++i) {
        if (config::initial_grid_size * (1 << i) > config::window_size.x || config::initial_grid_size * (1 << i) > config::window_size.y) {
            spdlog::warn("Perlin noise grid is bigger than window!");
            break;
        }
        std::vector<float> layer = GenerateOctave(config::initial_grid_size * (1 << i));
        for (int j = 0; j < config::window_size.x * config::window_size.y; ++j) {
            perlin_noise[j] += layer[j] / (1 << i);
        }
    }
    for (float& pixel : perlin_noise) {
        pixel /= 2.0f - 1.0f / (1 << (config::layer_amount - 1));
    }
    return perlin_noise;
}