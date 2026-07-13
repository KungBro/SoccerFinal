#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Window/Event.hpp>
#include <functional>
#include "menu_scene.h"  // for Button

// Semi-transparent pause overlay with Resume / Quit buttons.
// Owns its own UI elements and handles mouse events internally.
class PauseOverlay
{
public:
    PauseOverlay(const sf::Font& font,
                 std::function<void()> onResume,
                 std::function<void()> onQuit);

    // Process mouse events for the overlay buttons.
    void handleEvent(const sf::Event& event);

    // Render the overlay on top of the current frame.
    void draw(sf::RenderWindow& window);

private:
    sf::RectangleShape overlay;
    sf::Text pausedText;
    Button resumeButton;
    Button quitToMenuButton;
};