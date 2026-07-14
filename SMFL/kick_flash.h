#pragma once

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstdint>

// Simple expanding-flash effect that appears at the kick point.
// Grows from a small bright circle to a larger transparent one,
// then auto-cleans up when the animation finishes.
class KickFlash
{
public:
    KickFlash(const sf::Vector2f& pos);

    void update(float dt);
    void draw(sf::RenderWindow& window);

    bool active = true;

private:
    sf::CircleShape shape;
    float elapsed = 0.0f;

    static constexpr float DURATION     = 0.35f;
    static constexpr float START_RADIUS = 8.0f;
    static constexpr float END_RADIUS   = 45.0f;

    // Warm gold, semi-transparent
    static constexpr uint8_t START_ALPHA = 220;
};
