#include "player.h"
#include "physics.h"
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <algorithm>
#include <cmath>

// 球员属性设置
static const PlayerAttributes kPlayerAttributes[6] = {
    // CHE         ATM         ARS         MCI         PSG         FCB
    {1.0f,1.0f,1.2f}, {1.2f,1.0f,1.4f}, {1.0f,1.2f,1.4f},
    {1.4f,1.0f,1.4f}, {1.5f,0.9f,1.0f}, {0.9f,1.2f,1.5f},
};

const PlayerAttributes& getPlayerAttributes(int type)
{
    return kPlayerAttributes[type];
}

Player::Player()
    : acceleration(Constants::Gravity)
{
    onGround = true;
}

void Player::applyEffect(Effect::Type type, float duration)
{
    for (auto& e : effects) {
        if (e.type == type) {
            e.remaining = std::max(e.remaining, duration);
            return;
        }
    }
    effects.push_back({ type, duration });
}

bool Player::hasEffect(Effect::Type type) const
{
    for (auto& e : effects) {
        if (e.type == type && e.remaining > 0.0f)
            return true;
    }
    return false;
}

void Player::updateEffects(float dt)
{
    for (auto& e : effects)
        e.remaining -= dt;

    effects.erase(
        std::remove_if(effects.begin(), effects.end(),
            [](const Effect& e) { return e.remaining <= 0.0f; }),
        effects.end());

    effectSpeedMult  = 1.0f;
    effectJumpMult   = 1.0f;
    effectKickMult   = 1.0f;
    effectFrozen     = false;

    for (auto& e : effects) {
        switch (e.type) {
        case Effect::SpeedUp:   effectSpeedMult *= 1.5f;  break;
        case Effect::SlowDown:  effectSpeedMult *= 0.5f;  break;
        case Effect::JumpBoost: effectJumpMult  *= 1.5f;  break;
        case Effect::BigKick:   effectKickMult  *= 1.5f;  break;
        case Effect::Freeze:    effectFrozen     = true;  break;
        }
    }
}

bool Player::loadHeadshot(const std::string& path)
{
    hasHead = headTex.loadFromFile(path);
    return hasHead;
}

bool Player::loadBodyTexture(const std::string& path)
{
    hasBody = bodyTex.loadFromFile(path);
    return hasBody;
}

void Player::draw(sf::RenderWindow& window)
{
    if (hasBody) {
        sf::Sprite bodySprite(bodyTex);
        auto sz = bodyTex.getSize();
        bodySprite.setScale({
            (float)Constants::PlayerBodyWidth / (float)sz.x,
            (float)Constants::PlayerBodyHeight / (float)sz.y
            });
        bodySprite.setOrigin({ (float)sz.x * 0.5f, 0.0f });
        bodySprite.setPosition(
            pos + sf::Vector2f(0.0f, (float)Constants::PlayerRadius));
        window.draw(bodySprite);
    }
    else {
        sf::RectangleShape fallback(sf::Vector2f(
            (float)Constants::PlayerBodyWidth,
            (float)Constants::PlayerBodyHeight));
        fallback.setOrigin({ Constants::PlayerBodyWidth * 0.5f, 0.0f });
        fallback.setPosition(
            pos + sf::Vector2f(0.0f, (float)Constants::PlayerRadius));
        fallback.setFillColor(sf::Color::Blue);
        window.draw(fallback);
    }

    const float headSize = Constants::PlayerRadius * 2.0f * 1.25f;
    const float headOffsetY = Constants::PlayerRadius * 0.25f;

    if (hasHead) {
        sf::Sprite headSprite(headTex);
        auto sz = headTex.getSize();
        headSprite.setScale({ headSize / (float)sz.x, headSize / (float)sz.y });
        headSprite.setOrigin({ (float)sz.x * 0.5f, (float)sz.y * 0.5f });
        headSprite.setPosition(
            pos + sf::Vector2f(0.0f, headOffsetY));
        headSprite.setScale({
            (playerface == 1 ? -1.0f : 1.0f) * headSize / (float)sz.x,
            headSize / (float)sz.y
            });
        window.draw(headSprite);
    }
    else {
        sf::CircleShape fallback(Constants::PlayerRadius * 1.25f);
        fallback.setOrigin({ Constants::PlayerRadius * 1.25f,
                            Constants::PlayerRadius * 1.25f });
        fallback.setPosition(pos + sf::Vector2f(0.0f, headOffsetY));
        fallback.setFillColor(sf::Color(200, 200, 200));
        window.draw(fallback);
    }
}

void Player::move(int orientation)
{
    moveInput = std::clamp(orientation, -1, 1);
}

void Player::jump()
{
    if (!onGround) return;
    onGround = false;
    velocity.y = -Constants::JumpSpeed * attribs.jumpMult * effectJumpMult;
}

void Player::update()
{
    updateEffects(1.0f / 60.0f);

    const int effectiveInput = effectFrozen ? 0 : moveInput;

    const float ax = (onGround ? Constants::PlayerGroundAccel : Constants::PlayerAirAccel)
                   * attribs.speedMult;
    velocity.x += effectiveInput * ax;
    const float vmax = (onGround ? Constants::PlayerMaxSpeedGround : Constants::PlayerMaxSpeedAir)
                       * attribs.speedMult * effectSpeedMult;
    velocity.x = std::clamp(velocity.x, -vmax, vmax);
    if (effectiveInput == 0) {
        velocity.x *= (onGround ? Constants::PlayerGroundDrag : Constants::PlayerAirDrag);
        if (std::abs(velocity.x) < 0.08f) velocity.x = 0.0f;
    }
    velocity.y += acceleration * 2.0f;
    pos += velocity;
    Physics::checkPlayerBoundary(this);
}