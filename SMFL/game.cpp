#include "game.h"
#include "constants.h"
#include "menu_scene.h"
#include "select_scene.h"
#include "match_scene.h"
#include "end_scene.h"
#include <SFML/Window/Event.hpp>
#include <iostream>

Game::Game()
    : window(sf::VideoMode({ Constants::WindowWidth, Constants::WindowHeight }), "SoccerGame")
{
    window.setFramerateLimit(60);

    const char* fontPaths[] = {
        "C:/Windows/Fonts/msyh.ttc",
        "C:/Windows/Fonts/simhei.ttf",
        "C:/Windows/Fonts/simsun.ttc",
    };
    for (const auto* path : fontPaths) {
        if (font.openFromFile(path)) break;
    }

    menuScene = new MenuScene(font);
    selectScene = new SelectScene(font);
    matchScene = new MatchScene(font);
    currentScene = menuScene;

    menuScene->onSinglePlayer = [this]() { goToSelect(true); };
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
    while (window.isOpen()) {
        processEvents();
        float dt = clock.restart().asSeconds();
        currentScene->update(dt);
        render();
    }
}

void Game::processEvents()
{
    while (auto event = window.pollEvent()) {
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

void Game::startMatch(int p1Type, int p2Type, bool singlePlayer)
{
    player1Type = p1Type; player2Type = p2Type;
    gameIsSinglePlayer = singlePlayer;
    matchScene->setPlayers(p1Type, p2Type);
    matchScene->setSinglePlayer(singlePlayer);
    matchScene->restartMatch();
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