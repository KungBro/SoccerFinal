#pragma once

#include "actor.h"
#include "constants.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>

class Player : public Actor
{
public:
    Player();
    void draw(sf::RenderWindow& window) override;
    bool loadHeadshot(const std::string& path);
    bool loadBodyTexture(const std::string& path);
    void move(int orientation);
    void jump();
    void update() override;

    bool iskick = false;
    int playerface = 1;
    float acceleration = Constants::Gravity;
    int moveInput = 0;

private:
    sf::Texture headTex;
    sf::Texture bodyTex;
    bool hasHead = false;
    bool hasBody = false;
    
};