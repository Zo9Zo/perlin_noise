#include "perlin.hpp"
#include "config.hpp"
#include "random.hpp"

#include <array>
#include <cmath>
#include <cstdint>
#include <vector>

float Lerp(float a, float b, float t) {
    return a + (b - a) * t;
}

float BiLerp(const std::array<float, 4>& values, const std::array<float, 3>& t) {
    float lerp1 = Lerp(values[0], values[1], t[0]);
    float lerp2 = Lerp(values[2], values[3], t[1]);
    return Lerp(lerp1, lerp2, t[2]);
}

std::vector<sf::Vector2f> GenerateVectors() {
    std::vector<sf::Vector2f> vectors;
    for (int i = 0; i < (config::grid_size + 1) * (config::grid_size + 1); ++i) {
        sf::Angle cur_angle = sf::radians(random_gen::GetFloat(0.0f, 2 * M_PI));
        sf::Vector2 cur_vector = sf::Vector2f(1.0f, cur_angle);
        vectors.push_back(cur_vector);
    }
    return vectors;
}

std::vector<float> GenerateOctave() {
    std::vector<sf::Vector2f> vectors = GenerateVectors();
    std::vector<float> pixel_values;
    for (int i = 0; i < config::window_size.y; ++i) {
        const uint32_t cell_y = i / config::window_size.y;
        const float dy = static_cast<float>(i % config::window_size.y) / config::cell_size.y;

        for (int j = 0; j < config::window_size.x; ++j) {
            const uint32_t cell_x = j / config::window_size.x;
            const float dx = static_cast<float>(j % config::window_size.x) / config::cell_size.x;

            std::array<sf::Vector2f, 4> vectors_to_pixel;
            vectors_to_pixel[0] = {dx, dy};
            vectors_to_pixel[1] = {1.0f - dx, dy};
            vectors_to_pixel[2] = {dx, 1.0f - dy};
            vectors_to_pixel[3] = {1.0f - dx, 1.0f - dy};

            std::array<sf::Vector2f, 4> corner_vectors;
            corner_vectors[0] = vectors[cell_y * (config::grid_size + 1) + cell_x];
            corner_vectors[1] = vectors[cell_y * (config::grid_size + 1) + (cell_x + 1)];
            corner_vectors[2] = vectors[(cell_y + 1) * (config::grid_size + 1) + cell_x];
            corner_vectors[3] = vectors[(cell_y + 1) * (config::grid_size + 1) + (cell_x + 1)];

            std::array<float, 4> dots;
            for (int k = 0; k < 4; ++k) {
                dots[k] = vectors_to_pixel[k].dot(corner_vectors[k]);
            }

            float pixel_value = 0.5f * (BiLerp(dots, {dx, dx, dy}) + 1.0f);
            pixel_values.push_back(pixel_value);
        }
    }
    return pixel_values;
}