#pragma once

#include <SFML/Graphics.hpp>

namespace config {
    inline constexpr int initial_grid_size = 5;
    inline constexpr sf::Vector2u window_size = {800, 800}; // Has to be square
    inline constexpr int layer_amount = 5;
}