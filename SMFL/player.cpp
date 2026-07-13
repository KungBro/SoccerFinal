#include "player.h"
#include "physics.h"
#include <SFML/Graphics/RectangleShape.hpp>   // <-- ��
#include <SFML/Graphics/CircleShape.hpp>      // <-- ��
#include <algorithm>
#include <cmath>

Player::Player()
    : acceleration(Constants::Gravity)
{
    onGround = true;
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
    // ---- ���壨ͷ�����·���----
    // ԭ�� Qt��bodyRect(pos.x-20, pos.y+50, 40, 50)
    if (hasBody) {
        sf::Sprite bodySprite(bodyTex);
        auto sz = bodyTex.getSize();
        // ���ţ����� 40���߶� 50
        bodySprite.setScale({
            (float)Constants::PlayerBodyWidth / (float)sz.x,
            (float)Constants::PlayerBodyHeight / (float)sz.y
            });
        // ԭ�㣺ˮƽ���У���������
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

    // ---- ͷ����Բ���� pos + ΢С��ƫ��----
    // ԭ�� Qt��Բ�� = pos + (0, PlayerRadius*0.25)���뾶 = PlayerRadius*1.25
    const float headSize = Constants::PlayerRadius * 2.0f * 1.25f;
    const float headOffsetY = Constants::PlayerRadius * 0.25f;

    if (hasHead) {
        sf::Sprite headSprite(headTex);
        auto sz = headTex.getSize();
        // ��������ͷ������
        headSprite.setScale({ headSize / (float)sz.x, headSize / (float)sz.y });
        // ԭ�㣺ͼƬ����
        headSprite.setOrigin({ (float)sz.x * 0.5f, (float)sz.y * 0.5f });
        headSprite.setPosition(
            pos + sf::Vector2f(0.0f, headOffsetY));
        // ����ת
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
    velocity.y = -Constants::JumpSpeed;
}

void Player::update()
{
    const float ax = onGround ? Constants::PlayerGroundAccel : Constants::PlayerAirAccel;
    velocity.x += moveInput * ax;
    const float vmax = onGround ? Constants::PlayerMaxSpeedGround : Constants::PlayerMaxSpeedAir;
    velocity.x = std::clamp(velocity.x, -vmax, vmax);
    if (moveInput == 0) {
        velocity.x *= (onGround ? Constants::PlayerGroundDrag : Constants::PlayerAirDrag);
        if (std::abs(velocity.x) < 0.08f) velocity.x = 0.0f;
    }
    velocity.y += acceleration * 2.0f;
    pos += velocity;
    Physics::checkPlayerBoundary(this);
}