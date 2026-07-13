#pragma once

#include "actor.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

class Ball : public Actor
{
public:
    Ball();
    void draw(sf::RenderWindow& window) override;
    void update() override;
    sf::Vector2f getPos();

    sf::Vector2f prevPos;
    sf::Vector2f acceleration;

private:
    sf::Texture texture;
    sf::RectangleShape shape;
    bool hasTexture = false;
};