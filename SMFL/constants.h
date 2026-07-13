#pragma once

namespace Constants
{
    constexpr int WindowWidth = 1600;
    constexpr int WindowHeight = 900;
    constexpr int ArrowWidth = 233;
    constexpr int ArrowHeight = 101;
    constexpr int GroundLevel = 700;
    constexpr int PlayerRadius = 50;
    constexpr int PlayerHeight = 100;
    constexpr int PlayerBodyHeight = 50;
    constexpr int PlayerBodyWidth = 40;
    constexpr int Playeroffset = 20;
    constexpr int BallRadius = 25;
    constexpr int GoalWidth = 70;
    constexpr int GoalHeight = 240;
    constexpr int GoalBarThickness = 15;

    constexpr float Gravity = 0.70f;
    constexpr float BallAirDrag = 0.999f;
    constexpr float BallGroundFriction = 0.985f;
    constexpr float BallRestitutionGround = 0.70f;
    constexpr float BallRestitutionWall = 0.75f;
    constexpr float BallSleepVy = 0.9f;
    constexpr float BallMaxSpeed = 26.0f;

    constexpr float PlayerGroundAccel = 1.2f;
    constexpr float PlayerAirAccel = 0.45f;
    constexpr float PlayerGroundDrag = 0.80f;
    constexpr float PlayerAirDrag = 0.98f;
    constexpr float PlayerMaxSpeedGround = 9.0f;
    constexpr float PlayerMaxSpeedAir = 6.5f;
    constexpr float JumpSpeed = 22.0f;

    constexpr float KickMinStrength = 20.0f;
    constexpr float KickMaxStrength = 70.0f;
    constexpr float KickRangeForward = 150.0f;
    constexpr float KickRangeBackward = 25.0f;
    constexpr float KickRangeVertical = 100.0f;
    constexpr float KickMinAngleDeg = -5.0f;
    constexpr float KickMaxAngleDeg = 60.0f;
    constexpr float KickLiftBias = 0.35f;
}