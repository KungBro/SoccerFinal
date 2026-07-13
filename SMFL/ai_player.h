#pragma once

class Player;
class Ball;

// AI controller that drives a single player in single-player mode.
// Reads the ball state and issues move / jump / kick commands.
class AIPlayer
{
public:
    AIPlayer() = default;

    // Call once per frame BEFORE the target player's update().
    // Reads ball state and applies decisions to `target`.
    void update(Player& target, const Ball& ball);

    // Reset decision state (call on match restart).
    void reset();

private:
    float decisionTimer = 0.0f;
};