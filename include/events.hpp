#pragma once

#include <SFML/Graphics.hpp>

namespace EventHandler {
	struct Events {
        bool close_window = false;
        bool reset_perlin = false;
    };

    Events HandleEvents(sf::RenderWindow& window);
}
