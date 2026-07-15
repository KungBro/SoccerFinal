#pragma once

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

class Actor
{
public:
    virtual ~Actor() = default;
    virtual void draw(sf::RenderWindow& window) = 0;
    virtual void update() = 0;

    sf::Vector2f pos;// 位置
    sf::Vector2f velocity;// 速度
    bool onGround = false;// 是否在地面
};
