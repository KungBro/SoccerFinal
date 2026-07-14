#pragma once

#include "scene.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/ConvexShape.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <functional>
#include "player.h"

class SelectScene : public Scene
{
public:
    SelectScene(const sf::Font& font);
    ~SelectScene() = default;

    void handleEvent(const sf::Event& event) override;
    void update(float dt) override;
    void draw(sf::RenderWindow& window) override;

    std::function<void(int p1Type, int p2Type)> onPlayersSelected;
    std::function<void()> onBackToMenu;

private:
    void loadAssets();

    const sf::Font& font;
    sf::Texture bgTexture;
    std::vector<sf::Texture> headTextures;
    std::vector<sf::Texture> bodyTextures;
    int player1Index = 0;
    int player2Index = 0;
};