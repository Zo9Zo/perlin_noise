#pragma once

#include <SFML/Graphics.hpp>

namespace config {
    inline int grid_size = 5;
    inline sf::Vector2u window_size = {300, 300}; // Has to be square
    inline sf::Vector2u cell_size = {window_size.x / grid_size, window_size.y / grid_size};
}