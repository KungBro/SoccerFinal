#include "end_scene.h"
#include "constants.h"
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Mouse.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <cstdio>

EndScene::EndScene(const sf::Font& font)
    : font(font)
    , playAgainButton(font, "Play Again", { 220, 56 }, { 800, 468 },
        [this]() { if (onPlayAgain) onPlayAgain(); })
    , backToMenuButton(font, "Menu", { 220, 56 }, { 800, 558 },
        [this]() { if (onBackToMenu) onBackToMenu(); })
    , resultText(font)
{
    bool ok = bgTexture.loadFromFile("assets/images/background.png");
    (void)ok;

    playAgainButton.setColor({ 0xD3,0x54,0x00 }, { 0xE6,0x7E,0x22 }, { 0xA0,0x40,0x00 });
    backToMenuButton.setColor({ 0x8A,0x91,0x96 }, { 0x9B,0xA1,0xA6 }, { 0x6C,0x72,0x76 });
    resultText.setCharacterSize(32);
    resultText.setFillColor(sf::Color::White);
}

void EndScene::setResult(int s1, int s2, int p1, int p2)
{
    player1Type = p1; player2Type = p2;
    const char* abbrs[] = { "CHE","ATM","ARS","MCI","PSG","FCB" };
    sf::Color tcs[] = {
        {0x03,0x46,0x94},{0xCB,0x35,0x24},{0xEF,0x01,0x07},
        {0x6C,0xAB,0xDD},{0x00,0x41,0x70},{0xDC,0x05,0x2D}
    };

    std::string win; sf::Color wc = sf::Color::White;
    if (s1 > s2 && p1 >= 0 && p1 < 6) {
        win = std::string(abbrs[p1]) + " WIN!"; wc = tcs[p1];
    }
    else if (s2 > s1 && p2 >= 0 && p2 < 6) {
        win = std::string(abbrs[p2]) + " WIN!"; wc = tcs[p2];
    }
    else win = "Draw!";

    char buf[128];
    std::snprintf(buf, sizeof(buf), "%s\n%d : %d", win.c_str(), s1, s2);
    resultText.setString(buf); resultText.setFillColor(wc);
    auto tb = resultText.getLocalBounds();
    resultText.setOrigin({ tb.position.x + tb.size.x * 0.5f,
                          tb.position.y + tb.size.y * 0.5f });
    resultText.setPosition({ 800, 315 });
}

void EndScene::handleEvent(const sf::Event& event)
{
    if (const auto* mm = event.getIf<sf::Event::MouseMoved>()) {
        playAgainButton.handleMouseMove(mm->position);
        backToMenuButton.handleMouseMove(mm->position);
    }
    if (const auto* mb = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (mb->button == sf::Mouse::Button::Left) {
            playAgainButton.handleMouseClick(mb->position);
            backToMenuButton.handleMouseClick(mb->position);
        }
    }
}

void EndScene::draw(sf::RenderWindow& window)
{
    if (bgTexture.getSize().x > 0) {
        sf::Sprite bg(bgTexture);
        auto sz = bg.getLocalBounds().size;
        bg.setScale({ Constants::WindowWidth / sz.x,
                     Constants::WindowHeight / sz.y });
        window.draw(bg);
    }
    else {
        sf::RectangleShape bg({ Constants::WindowWidth, Constants::WindowHeight });
        bg.setFillColor({ 135, 206, 235 }); window.draw(bg);
    }
    window.draw(resultText);
    playAgainButton.draw(window);
    backToMenuButton.draw(window);
}