#pragma once

#include "scene.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Audio.hpp>
#include <functional>
#include <memory>
#include <vector>
#include "ball.h"
#include "player.h"
#include "ai_player.h"
#include "buff.h"

class PauseOverlay;

class MatchScene : public Scene
{
public:
    MatchScene(const sf::Font& font);
    ~MatchScene();

    void setPlayers(int p1Type, int p2Type);
    void setSinglePlayer(bool sp) { singlePlayer = sp; }
    void restartMatch();
    int getPlayer1Type() const { return player1Type; }
    int getPlayer2Type() const { return player2Type; }
    void setArrow(const Player& player, sf::RectangleShape& arrow);

    void handleEvent(const sf::Event& event) override;
    void update(float dt) override;
    void draw(sf::RenderWindow& window) override;

    std::function<void(int score1, int score2)> onGameFinished;
    std::function<void()> onBackToMenu;

private:
    void resetPositions();
    void collision();
    void checkGoalAndScore();
    void togglePause();

    const sf::Font& font;
    Player player1;
    Player player2;
    Ball ball;

    sf::Texture bgTexture;
    sf::Texture arTexture;
    std::vector<sf::Texture> headTextures;
    std::vector<sf::Texture> bodyTextures;
    sf::RectangleShape bgRect;
    sf::RectangleShape arrow_1;
    sf::RectangleShape arrow_2;
    sf::Vector2f arPos;

    int player1Type = 0, player2Type = 0;
    int score1 = 0, score2 = 0;
    float remainingMs = 90000.0f;
    bool matchFinished = false;

    // Single player / pause
    bool singlePlayer = false;
    bool paused = false;
    AIPlayer aiPlayer;
    PauseOverlay* pauseOverlay = nullptr;

    // 按键状态（只记录物理状态，不参与游戏逻辑）
    bool p1LeftPressed = false;
    bool p1RightPressed = false;
    bool p1JumpPressed = false;
    bool p1KickPressed = false;

    bool p2LeftPressed = false;
    bool p2RightPressed = false;
    bool p2JumpPressed = false;
    bool p2KickPressed = false;

    // 射门防连发：记录上一帧的按键状态
    bool p1KickWasPressed = false;
    bool p2KickWasPressed = false;

    // Buff 系统
    std::vector<Buff> buffs;
    float buffSpawnTimer = 0.0f;
    void spawnBuff();
    void updateBuffs(float dt);

    // 状态栏（预创建对象，每帧只更新文本）
    static constexpr int kMaxStatusLines = 5;
    void initStatusPanel();
    std::unique_ptr<sf::Text> statusP1Label;
    std::unique_ptr<sf::Text> statusP2Label;
    std::vector<std::unique_ptr<sf::Text>> statusP1Texts;
    std::vector<std::unique_ptr<sf::Text>> statusP2Texts;
    std::vector<sf::RectangleShape> statusP1Dots;
    std::vector<sf::RectangleShape> statusP2Dots;
    const sf::Color statusColors[5] = {
        { 50,  220, 50  },
        { 220, 50,  50  },
        { 50,  150, 255 },
        { 150, 200, 255 },
        { 255, 180, 30  },
    };
};