#include "game.h"
#include "constants.h"
#include "menu_scene.h"
#include "select_scene.h"
#include "match_scene.h"
#include "end_scene.h"
#include "AudioManager.h"
#include <SFML/Window/Event.hpp>
#include <iostream>

Game::Game()
    : window(sf::VideoMode({ Constants::WindowWidth, Constants::WindowHeight }), "SoccerGame")      //游戏窗口初始化
{
    window.setFramerateLimit(60);

    const char* fontPaths[] = {                     //启动时加载字体
        "C:/Windows/Fonts/msyh.ttc",
        "C:/Windows/Fonts/simhei.ttf",
        "C:/Windows/Fonts/simsun.ttc",
    };
    for (const auto* path : fontPaths) {
        if (font.openFromFile(path)) break;
    }

    menuScene = new MenuScene(font);                //生成游戏场景
    selectScene = new SelectScene(font);
    matchScene = new MatchScene(font);
    currentScene = menuScene;

    menuScene->onSinglePlayer = [this]() { goToSelect(true); };     //将功能函数填充到表示选择的函数容器里，实现选择->跳转的功能
    menuScene->onTwoPlayer = [this]() { goToSelect(false); };
    menuScene->onQuit = [this]() { window.close(); };
    selectScene->onPlayersSelected = [this](int p1, int p2) {
        startMatch(p1, p2, gameIsSinglePlayer);
    };
    selectScene->onBackToMenu = [this]() { goToMenu(); };
    matchScene->onGameFinished = [this](int s1, int s2) { goToEnd(s1, s2); };
    matchScene->onBackToMenu = [this]() { goToMenu(); };
}

Game::~Game() {
    delete menuScene; delete selectScene;
    delete matchScene; delete endScene;
}

void Game::run()
{
    AudioManager::GetInstance()->init();                //启动音频初始化
    while (window.isOpen()) {                           //基础游戏循环
        processEvents();                                    //L--基于事件处理的按键反馈
        float dt = clock.restart().asSeconds();             //|
        currentScene->update(dt);                           //L--状态更新
        render();                                           //L--渲染
    }
}

void Game::processEvents()
{
    while (auto event = window.pollEvent()) {                               //最外层只负责窗口关闭响应
        if (event->is<sf::Event::Closed>()) { window.close(); return; }
        currentScene->handleEvent(*event);
    }
}

void Game::render()
{
    window.clear(sf::Color::Black);
    currentScene->draw(window);
    window.display();
}

void Game::goToMenu() { currentScene = menuScene; }

void Game::goToSelect(bool singlePlayer)
{
    gameIsSinglePlayer = singlePlayer;
    currentScene = selectScene;
}

void Game::startMatch(int p1Type, int p2Type, bool singlePlayer)    //进入游戏界面
{
    player1Type = p1Type; player2Type = p2Type;
    gameIsSinglePlayer = singlePlayer;
    matchScene->setPlayers(p1Type, p2Type);                         //球员初始化
    matchScene->setSinglePlayer(singlePlayer);
    matchScene->restartMatch();                                     //复用重置比赛代码
    currentScene = matchScene;
}

void Game::goToEnd(int score1, int score2)
{
    if (!endScene) {
        endScene = new EndScene(font);
        endScene->onPlayAgain = [this]() { replayMatch(); };
        endScene->onBackToMenu = [this]() { goToMenu(); };
    }
    endScene->setResult(score1, score2,
        matchScene->getPlayer1Type(), matchScene->getPlayer2Type());
    currentScene = endScene;
}

void Game::replayMatch()
{
    matchScene->restartMatch();
    currentScene = matchScene;
}