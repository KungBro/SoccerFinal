#pragma once

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/CircleShape.hpp>

class Player;

class Buff
{
public:
    enum Type { SpeedUp, SlowDown, JumpBoost, Freeze, BigKick };

    Buff(Type type, sf::Vector2f pos);
    ~Buff() = default;

    void update(float dt);
    void draw(sf::RenderWindow& window);
    void applyTo(Player& target);
    bool affectsOpponent() const;

    sf::Vector2f pos;
    Type type;
    bool active = true;

    static void preload();                 // 游戏启动时预加载纹理
    static float radius();
    static const char* typeName(Type t);

private:
    float lifetime = 15.0f;
    sf::CircleShape shape;
};
