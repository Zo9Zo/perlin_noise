#pragma once

#include <SFML/Graphics.hpp>
#include <array>
#include <vector>

float Lerp(float a, float b, float t);

float BiLerp(const std::array<float, 4>& values, const std::array<float, 3>& t);

float Fade(float t);

std::vector<sf::Vector2f> GenerateVectors(int grid_size);

std::vector<float> GenerateOctave(int grid_size);

std::vector<float> GeneratePerlinNoise();