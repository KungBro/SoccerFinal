#pragma once

#include "scene.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Text.hpp>
#include <functional>
#include <string>
#include "menu_scene.h"   // for Button

class EndScene : public Scene
{
public:
    EndScene(const sf::Font& font);
    ~EndScene() = default;

    void setResult(int score1, int score2, int player1Type, int player2Type);
    void handleEvent(const sf::Event& event) override;
    void draw(sf::RenderWindow& window) override;

    std::function<void()> onPlayAgain;
    std::function<void()> onBackToMenu;

private:
    const sf::Font& font;
    sf::Texture bgTexture;
    Button playAgainButton;
    Button backToMenuButton;
    sf::Text resultText;
    int player1Type = -1, player2Type = -1;
};