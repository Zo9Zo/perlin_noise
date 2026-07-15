#include "image.hpp"
#include "perlin.hpp"
#include "events.hpp"
#include "config.hpp"

#include <SFML/Graphics.hpp>
#include <vector>

int main()
{
	sf::RenderWindow window( sf::VideoMode(config::window_size), "SFML works!" );

	std::vector<float> normal_values = GenerateOctave();
	sf::Image perlin_image = NormalToGrayscale(normal_values);
	sf::Texture perlin_texture(perlin_image);
	sf::Sprite perlin_sprite(perlin_texture);

	while (window.isOpen())
	{
		EventHandler::Events events = EventHandler::HandleEvents(window);
		if (events.close_window) {
            window.close();
		}
		if (events.reset_perlin) {
			normal_values = GenerateOctave();
			perlin_image = NormalToGrayscale(normal_values);
			if (!perlin_texture.loadFromImage(perlin_image)) {
				window.close();
				throw std::runtime_error("Failed to load perlin texture from image!");
			}
		}
		window.clear();
		window.draw(perlin_sprite);
		window.display();
	}
}
