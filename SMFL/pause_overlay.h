#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Window/Event.hpp>
#include <functional>
#include "menu_scene.h"  

class PauseOverlay
{
public:
    PauseOverlay(const sf::Font& font,
                 std::function<void()> onResume,
                 std::function<void()> onQuit);
    void handleEvent(const sf::Event& event);
    void draw(sf::RenderWindow& window);

private:
    sf::RectangleShape overlay;
    sf::Text pausedText;
    Button resumeButton;
    Button quitToMenuButton;
};