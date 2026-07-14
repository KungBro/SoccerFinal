#include "buff.h"
#include "player.h"
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Texture.hpp>

// File-scope static textures: shared by all Buff instances, loaded once in preload()
static sf::Texture s_speedUpTex;
static sf::Texture s_slowDownTex;
static sf::Texture s_jumpBoostTex;
static sf::Texture s_freezeTex;
static sf::Texture s_bigKickTex;

void Buff::preload()
{
    s_speedUpTex.loadFromFile("assets/images/bolt.png");
    s_slowDownTex.loadFromFile("assets/images/slow.png");
    s_jumpBoostTex.loadFromFile("assets/images/jump.png");
    s_freezeTex.loadFromFile("assets/images/ice.png");
    s_bigKickTex.loadFromFile("assets/images/big.png");
}

Buff::Buff(Type t, sf::Vector2f p)
    : pos(p), type(t)
    , shape(radius())
{
    shape.setOrigin({ radius(), radius() });
    shape.setOutlineThickness(5);

    switch (type) {
    case SpeedUp:
        shape.setOutlineColor(sf::Color::Yellow);
        shape.setTexture(&s_speedUpTex);
        break;
    case SlowDown:
        shape.setOutlineColor(sf::Color::Red);
        shape.setTexture(&s_slowDownTex);
        break;
    case JumpBoost:
        shape.setOutlineColor(sf::Color::Blue);
        shape.setTexture(&s_jumpBoostTex);
        break;
    case Freeze:
        shape.setOutlineColor(sf::Color::Blue);
        shape.setTexture(&s_freezeTex);
        break;
    case BigKick:
        shape.setOutlineColor(sf::Color::Yellow);
        shape.setTexture(&s_bigKickTex);
        break;
    }
}

float Buff::radius()
{
    return 25.0f;
}

const char* Buff::typeName(Type t)
{
    switch (t) {
    case SpeedUp:   return "Speed Up!";
    case SlowDown:  return "Slow Down!";
    case JumpBoost: return "Jump Boost!";
    case Freeze:    return "Frozen!";
    case BigKick:   return "Power Kick!";
    default:        return "???";
    }
}

bool Buff::affectsOpponent() const
{
    return type == SlowDown || type == Freeze;
}

void Buff::update(float dt)
{
    lifetime -= dt;
    if (lifetime <= 0.0f)
        active = false;
}

void Buff::draw(sf::RenderWindow& window)
{
    shape.setPosition(pos);
    window.draw(shape);
}

void Buff::applyTo(Player& target)
{
    Player::Effect::Type etype;
    switch (type) {
    case SpeedUp:   etype = Player::Effect::SpeedUp;   break;
    case JumpBoost: etype = Player::Effect::JumpBoost; break;
    case BigKick:   etype = Player::Effect::BigKick;   break;
    case SlowDown:  etype = Player::Effect::SlowDown;  break;
    case Freeze:    etype = Player::Effect::Freeze;    break;
    default: return;
    }
    target.applyEffect(etype, 6.0f);
}
