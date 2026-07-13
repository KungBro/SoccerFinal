#pragma once

#include "scene.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/System/Clock.hpp>

class MenuScene;
class SelectScene;
class MatchScene;
class EndScene;

class Game
{
public:
    Game();
    ~Game();
    void run();

private:
    void processEvents();
    void update(float dt);
    void render();
    void goToMenu();
    void goToSelect(bool singlePlayer);
    void startMatch(int p1Type, int p2Type, bool singlePlayer);
    void goToEnd(int score1, int score2);
    void replayMatch();

    sf::RenderWindow window;
    sf::Font font;
    sf::Clock clock;

    Scene* currentScene = nullptr;

    MenuScene* menuScene = nullptr;
    SelectScene* selectScene = nullptr;
    MatchScene* matchScene = nullptr;
    EndScene* endScene = nullptr;

    int player1Type = 0;
    int player2Type = 0;
    bool gameIsSinglePlayer = false;
};
