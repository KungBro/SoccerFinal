#pragma once

class Player;
class Ball;


class AIPlayer
{
public:
    AIPlayer() = default;

    // 在玩家update()之前调用
    // 读取球的位置，控制target
    void update(Player& target, const Ball& ball);

    void reset(); // 重置AI

private:
    float decisionTimer = 0.0f; // 决策计时
};