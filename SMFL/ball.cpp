#include "ball.h"
#include "constants.h"
#include "physics.h"
#include <SFML/Graphics/CircleShape.hpp>
#include <cmath>

Ball::Ball()
    : prevPos(Constants::WindowWidth / 2.0f, 100.0f)
    , acceleration(0.0f, Constants::Gravity)
    , shape(sf::Vector2f(Constants::BallRadius * 2.0f, Constants::BallRadius * 2.0f))
{
    if (texture.loadFromFile("assets/images/soccer.png")) {
        shape.setTexture(&texture);
        hasTexture = true;
    }
    shape.setOrigin({ Constants::BallRadius, Constants::BallRadius });
    pos = { Constants::WindowWidth / 2.0f, 100.0f };
    velocity = { 0.0f, 0.0f };
}

void Ball::draw(sf::RenderWindow& window)
{
    if (hasTexture) {
        shape.setPosition(pos);
        window.draw(shape);
    }
    else {
        sf::CircleShape circle(Constants::BallRadius);
        circle.setPosition(pos - sf::Vector2f(Constants::BallRadius, Constants::BallRadius));
        circle.setFillColor(sf::Color::Red);
        window.draw(circle);
    }
}

void Ball::update()
{
    prevPos = pos;
    velocity += acceleration;
    if (!onGround) velocity *= Constants::BallAirDrag;
    else velocity.x *= Constants::BallGroundFriction;
    const float speed = std::hypot(velocity.x, velocity.y);
    if (speed > Constants::BallMaxSpeed) velocity *= (Constants::BallMaxSpeed / speed);
    pos += velocity;
    Physics::checkBallBoundary(this);
}

sf::Vector2f Ball::getPos()
{
    return pos;
}