#pragma once

#include "scene.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <functional>
#include <string>

struct Button
{
    sf::RectangleShape shape;
    sf::Text text;
    std::function<void()> onClick;
    bool hovered = false;

    Button(const sf::Font& font, const std::string& label,
        const sf::Vector2f& size, const sf::Vector2f& position,
        std::function<void()> callback);
    void handleMouseMove(const sf::Vector2i& mousePos);
    void handleMouseClick(const sf::Vector2i& mousePos);
    void setColor(const sf::Color& normal, const sf::Color& hover,
        const sf::Color& press);
    void draw(sf::RenderWindow& window);

private:
    sf::Color colorNormal, colorHover, colorPress;
    bool pressed = false;
};

class MenuScene : public Scene
{
public:
    MenuScene(const sf::Font& font);
    void handleEvent(const sf::Event& event) override;
    void draw(sf::RenderWindow& window) override;

    std::function<void()> onSinglePlayer;
    std::function<void()> onTwoPlayer;
    std::function<void()> onQuit;

private:
    Button singlePlayerButton;
    Button twoPlayerButton;
    Button exitButton;
    sf::Text titleText;
};