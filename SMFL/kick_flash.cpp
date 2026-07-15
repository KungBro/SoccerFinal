#include "kick_flash.h"
#include <algorithm>

KickFlash::KickFlash(const sf::Vector2f& pos)
{
    shape.setPosition(pos);
    shape.setRadius(START_RADIUS);
    shape.setOrigin({ START_RADIUS, START_RADIUS });
    shape.setFillColor(sf::Color(255, 240, 180, START_ALPHA));
    shape.setOutlineThickness(0.0f);
}

void KickFlash::update(float dt)
{
    elapsed += dt;
    const float t = std::clamp(elapsed / DURATION, 0.0f, 1.0f);

    const float r = START_RADIUS + (END_RADIUS - START_RADIUS) * t;
    shape.setRadius(r);
    shape.setOrigin({ r, r });

    const auto alpha = static_cast<uint8_t>(START_ALPHA * (1.0f - t * t));
    const auto& c = shape.getFillColor();
    shape.setFillColor(sf::Color(c.r, c.g, c.b, alpha));

    if (elapsed >= DURATION)
        active = false;
}

void KickFlash::draw(sf::RenderWindow& window)
{
    window.draw(shape);
}
