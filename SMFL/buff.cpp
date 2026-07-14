#include "buff.h"
#include "player.h"
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Texture.hpp>

Buff::Buff(Type t, sf::Vector2f p)
    : pos(p), type(t)
    , shape(radius())
{
    // 函数内静态变量：首次调用时加载贴图，之后复用
    static sf::Texture FreezeTex;
    static sf::Texture JumpBoostTex;
    static sf::Texture BigKickTex;
    static sf::Texture SlowDownTex;
    static sf::Texture speedUpTex;
    static bool texLoaded = false;
    if (!texLoaded) {
        speedUpTex.loadFromFile("assets/images/bolt.png");
        FreezeTex.loadFromFile("assets/images/ice.png");
        SlowDownTex.loadFromFile("assets/images/slow.png");
        BigKickTex.loadFromFile("assets/images/big.png");
        JumpBoostTex.loadFromFile("assets/images/jump.png");
        texLoaded = true;
    }

    shape.setOrigin({ radius(), radius() });
    shape.setOutlineThickness(5);
    

    switch (type) {
    case SpeedUp:
        shape.setOutlineColor(sf::Color::Yellow);
        shape.setTexture(&speedUpTex);
        break;
    case SlowDown:  
        shape.setOutlineColor(sf::Color::Red);
        shape.setTexture(&SlowDownTex);
        break;
    case JumpBoost: 
        shape.setOutlineColor(sf::Color::Blue);
        shape.setTexture(&JumpBoostTex);
        break;
    case Freeze:    
        shape.setOutlineColor(sf::Color::Blue);
        shape.setTexture(&FreezeTex);
        break;
    case BigKick:   
        shape.setOutlineColor(sf::Color::Yellow);
        shape.setTexture(&BigKickTex);
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
