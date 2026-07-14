#include "menu_scene.h"
#include "constants.h"
#include "AudioManager.h"
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Mouse.hpp>

// ---- Button ----
Button::Button(const sf::Font& font, const std::string& label,
    const sf::Vector2f& size, const sf::Vector2f& position,
    std::function<void()> callback)
    : shape(size)
    , text(font)
    , onClick(std::move(callback))
{
    shape.setOrigin({ size.x * 0.5f, size.y * 0.5f });
    shape.setPosition(position);
    text.setString(label);
    text.setCharacterSize(24);
    text.setFillColor(sf::Color::White);
    auto tb = text.getLocalBounds();
    text.setOrigin({ tb.position.x + tb.size.x * 0.5f,
                    tb.position.y + tb.size.y * 0.5f });
    text.setPosition(position);
}

void Button::setColor(const sf::Color& n, const sf::Color& h, const sf::Color& p)
{
    colorNormal = n; colorHover = h; colorPress = p;
    shape.setFillColor(n);
}

void Button::handleMouseMove(const sf::Vector2i& mp)
{
    hovered = shape.getGlobalBounds().contains(
        { (float)mp.x, (float)mp.y });
    if (pressed && !hovered) pressed = false;
    if (!pressed) shape.setFillColor(hovered ? colorHover : colorNormal);
}

void Button::handleMouseClick(const sf::Vector2i& mp)
{
    if (shape.getGlobalBounds().contains({ (float)mp.x, (float)mp.y })) {
        pressed = true;
        AudioManager::GetInstance()->play("click");
        //sound_.play();
        if (AudioManager::GetInstance()->isMusicPaused)
        {
            AudioManager::GetInstance()->playMusic();
            AudioManager::GetInstance()->isMusicPaused = false;
        }
        shape.setFillColor(colorPress);
        if (onClick) onClick();
        pressed = false;
    }
}

void Button::draw(sf::RenderWindow& window) {
    window.draw(shape); window.draw(text);
}

// ---- MenuScene ----
MenuScene::MenuScene(const sf::Font& font)
    : singlePlayerButton(font, "Single Player", { 250, 80 }, { 800, 360 },
        [this]() { if (onSinglePlayer) onSinglePlayer(); })
    , twoPlayerButton(font, "Two Player", { 250, 80 }, { 800, 470 },
        [this]() { if (onTwoPlayer) onTwoPlayer(); })
    , exitButton(font, "Quit", { 250, 80 }, { 800, 580 },
        [this]() { if (onQuit) onQuit(); })
    , titleText(font)
{
    titleText.setString("SoccerGame");
    titleText.setCharacterSize(48);
    titleText.setFillColor(sf::Color::White);
    auto tb = titleText.getLocalBounds();
    titleText.setOrigin({ tb.position.x + tb.size.x * 0.5f,
                         tb.position.y + tb.size.y * 0.5f });
    titleText.setPosition({ 800, 180 });

    singlePlayerButton.setColor({ 0x27,0xAE,0x60 }, { 0x2E,0xCC,0x71 }, { 0x1E,0x84,0x45 });
    twoPlayerButton.setColor({ 0xD3,0x54,0x00 }, { 0xE6,0x7E,0x22 }, { 0xA0,0x40,0x00 });
    exitButton.setColor({ 0x8A,0x91,0x96 }, { 0x9B,0xA1,0xA6 }, { 0x6C,0x72,0x76 });
}

void MenuScene::handleEvent(const sf::Event& event)
{
    if (const auto* mm = event.getIf<sf::Event::MouseMoved>()) {
        singlePlayerButton.handleMouseMove(mm->position);
        twoPlayerButton.handleMouseMove(mm->position);
        exitButton.handleMouseMove(mm->position);
    }
    if (const auto* mb = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (mb->button == sf::Mouse::Button::Left) {
            singlePlayerButton.handleMouseClick(mb->position);
            twoPlayerButton.handleMouseClick(mb->position);
            exitButton.handleMouseClick(mb->position);
        }
    }
}

void MenuScene::draw(sf::RenderWindow& window)
{
    sf::RectangleShape bg({ Constants::WindowWidth, Constants::WindowHeight });
    bg.setFillColor({ 135, 206, 235 });
    window.draw(bg);
    window.draw(titleText);
    singlePlayerButton.draw(window);
    twoPlayerButton.draw(window);
    exitButton.draw(window);
}