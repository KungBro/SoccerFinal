#pragma once

#include <SFML/System/Vector2.hpp>

class Ball;
class Player;

class Physics
{
public:
    static void checkBallBoundary(Ball* ball);
    static void checkPlayerBoundary(Player* player);
    static void checkPlayerCollision(Player* player1, Player* player2);
    static bool checkCollideHead(Player* player, Ball* ball);
    static bool checkCollideBody(Player* player, Ball* ball);
    static void checkGoalCrossbarCollision(Ball* ball);
    static bool isInKickZone(const Player& player, const Ball& ball);
    static void resolvePlayerStandingOnBall(Player& player, Ball& ball);
};

inline float dot(const sf::Vector2f& a, const sf::Vector2f& b)
{
    return a.x * b.x + a.y * b.y;
}