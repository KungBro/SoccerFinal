#include "ai_player.h"
#include "player.h"
#include "ball.h"
#include "constants.h"
#include <cmath>
#include <cstdlib>

void AIPlayer::update(Player& target, const Ball& ball)
{
    target.move(0);

    float dx = ball.pos.x - target.pos.x;
    float dy = ball.pos.y - target.pos.y;
    if (std::abs(dx) > 10.0f) {
        target.playerface = (dx > 0) ? 1 : -1;
    }

    if (std::abs(dx) > 40.0f) {
        target.move(dx > 0 ? 1 : -1);
    } else if (std::abs(dx) > 15.0f) {
        target.move(dx > 0 ? 1 : -1);
    }

    decisionTimer -= 1.0f;
    if (decisionTimer <= 0.0f) {
        decisionTimer = 8.0f + (std::rand() % 6);

        float dist = std::hypot(dx, dy);

        bool ballHigh = ball.pos.y < target.pos.y - 20.0f;
        bool closeEnough = dist < 300.0f;
        if (ballHigh && closeEnough && target.onGround) {
            target.jump();
        }

        float kickDist = Constants::KickRangeForward * 0.9f;
        if (dist < kickDist) {
            target.iskick = true;
        }
    }
}

void AIPlayer::reset()
{
    decisionTimer = 0.0f;
}