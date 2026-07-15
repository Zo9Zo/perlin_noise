#include "events.hpp"

#include <SFML/Graphics.hpp>
#include <optional>

EventHandler::Events EventHandler::HandleEvents(sf::RenderWindow& window) {
    Events events;

    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            events.close_window = true;
        }
        else if (const auto* key_pressed_event = event->getIf<sf::Event::KeyPressed>()) {
            if (key_pressed_event->code == sf::Keyboard::Key::F) {
                events.reset_perlin = true;
            }
            else if (key_pressed_event->code == sf::Keyboard::Key::Escape) {
                events.close_window = true;
            }
        }
    }
    return events;
}
