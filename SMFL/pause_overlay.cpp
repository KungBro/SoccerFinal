#include "pause_overlay.h"
#include "constants.h"
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Mouse.hpp>

PauseOverlay::PauseOverlay(const sf::Font& font,
                           std::function<void()> onResume,
                           std::function<void()> onQuit)
    : pausedText(font)
    , resumeButton(font, "Resume", { 220, 56 }, { 800, 420 }, std::move(onResume))
    , quitToMenuButton(font, "Quit to Menu", { 220, 56 }, { 800, 510 }, std::move(onQuit))
{
    overlay.setSize({ Constants::WindowWidth, Constants::WindowHeight });
    overlay.setFillColor({ 0, 0, 0, 160 });

    pausedText.setString("PAUSED");
    pausedText.setCharacterSize(56);
    pausedText.setFillColor(sf::Color::White);
    auto ptb = pausedText.getLocalBounds();
    pausedText.setOrigin({ ptb.position.x + ptb.size.x * 0.5f,
                          ptb.position.y + ptb.size.y * 0.5f });
    pausedText.setPosition({ 800, 280 });

    resumeButton.setColor({ 0x27, 0xAE, 0x60 }, { 0x2E, 0xCC, 0x71 }, { 0x1E, 0x84, 0x45 });
    quitToMenuButton.setColor({ 0x8A, 0x91, 0x96 }, { 0x9B, 0xA1, 0xA6 }, { 0x6C, 0x72, 0x76 });
}

void PauseOverlay::handleEvent(const sf::Event& event)
{
    if (const auto* mm = event.getIf<sf::Event::MouseMoved>()) {
        resumeButton.handleMouseMove(mm->position);
        quitToMenuButton.handleMouseMove(mm->position);
    }
    if (const auto* mb = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (mb->button == sf::Mouse::Button::Left) {
            resumeButton.handleMouseClick(mb->position);
            quitToMenuButton.handleMouseClick(mb->position);
        }
    }
}

void PauseOverlay::draw(sf::RenderWindow& window)
{
    window.draw(overlay);
    window.draw(pausedText);
    resumeButton.draw(window);
    quitToMenuButton.draw(window);
}