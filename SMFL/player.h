#pragma once

#include "actor.h"
#include "constants.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <vector>

class Player : public Actor
{
public:
    // ---- Buff 效果系统 ----
    struct Effect {
        enum Type { SpeedUp, SlowDown, JumpBoost, Freeze, BigKick };
        Type type;
        float remaining;  // 剩余秒数
    };

    Player();
    void draw(sf::RenderWindow& window) override;
    bool loadHeadshot(const std::string& path);
    bool loadBodyTexture(const std::string& path);
    void move(int orientation);
    void jump();
    void update() override;

    void applyEffect(Effect::Type type, float duration);
    bool hasEffect(Effect::Type type) const;
    const std::vector<Effect>& getEffects() const { return effects; }

    bool iskick = false;
    int playerface = 1;
    float acceleration = Constants::Gravity;
    int moveInput = 0;

    // Buff 效果倍率（updateEffects 每帧更新）
    float effectSpeedMult = 1.0f;
    float effectJumpMult = 1.0f;
    float effectKickMult = 1.0f;
    bool  effectFrozen = false;

private:
    void updateEffects(float dt);

    sf::Texture headTex;
    sf::Texture bodyTex;
    bool hasHead = false;
    bool hasBody = false;
    std::vector<Effect> effects;
};