#include "select_scene.h"
#include "constants.h"
#include "AudioManager.h"
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Graphics/Sprite.hpp>

SelectScene::SelectScene(const sf::Font& font)
    : font(font)
{ 
    loadAssets(); 
}

void SelectScene::loadAssets()
{
    (void)bgTexture.loadFromFile("assets/images/select_scene.png");         //统一加载选人界面资源
    const char* heads[] = { "player_1","player_2","player_3",
                           "player_4","player_6","player_5" };
    const char* bodys[] = { "CHE","ATM","ARS","MCI","PSG","FCB" };
    headTextures.resize(6); bodyTextures.resize(6);
    for (int i = 0; i < 6; ++i) {
        (void)headTextures[i].loadFromFile(
            std::string("assets/images/") + heads[i] + ".png");
        (void)bodyTextures[i].loadFromFile(
            std::string("assets/images/") + bodys[i] + ".png");
    }
}

void SelectScene::handleEvent(const sf::Event& event)
{
    if (const auto* key = event.getIf<sf::Event::KeyPressed>()) {           //按键控制选择
        switch (key->code) {
        case sf::Keyboard::Key::A:
            player1Index = (player1Index + 5) % 6; 
            AudioManager::GetInstance()->play("select");
            break;
        case sf::Keyboard::Key::D:
            player1Index = (player1Index + 1) % 6; 
            AudioManager::GetInstance()->play("select");
            break;
        case sf::Keyboard::Key::Left:
            player2Index = (player2Index + 5) % 6; 
            AudioManager::GetInstance()->play("select");
            break;
        case sf::Keyboard::Key::Right:
            player2Index = (player2Index + 1) % 6;
            AudioManager::GetInstance()->play("select");
            break;
        case sf::Keyboard::Key::S:
        case sf::Keyboard::Key::Down:
            if (onPlayersSelected)
                AudioManager::GetInstance()->play("click");
                onPlayersSelected(player1Index, player2Index);
            break;
        case sf::Keyboard::Key::Escape:
            if (onBackToMenu) onBackToMenu(); break;
        default: break;
        }
    }
}

void SelectScene::update(float) {}

void SelectScene::draw(sf::RenderWindow& window)                         //绘制选人界面
{
    sf::Sprite bgSprite(bgTexture);
    auto bgSize = bgSprite.getLocalBounds().size;
    bgSprite.setScale({ Constants::WindowWidth / bgSize.x,
                       Constants::WindowHeight / bgSize.y });
    window.draw(bgSprite);

    sf::Text titleText(font);
    titleText.setString("Bouncing World Cup");
    titleText.setCharacterSize(42);
    titleText.setFillColor(sf::Color::White);
    auto tb = titleText.getLocalBounds();
    titleText.setOrigin({ tb.position.x + tb.size.x * 0.5f,
                         tb.position.y + tb.size.y * 0.5f });
    titleText.setPosition({ 800, 60 });
    window.draw(titleText);

    const int count = 6;
    const float spacing = 24;
    const float slotWidth = (Constants::WindowWidth - spacing * 2) / count;
    const float groundY = Constants::GroundLevel;
    const float headCenterY = groundY - Constants::PlayerHeight;
    const char* names[] = { "Enzo","Alvarez","Rice","Haaland","Dembele","Kane" };

    const char* headPaths[] = { "player_1","player_2","player_3",
                                "player_4","player_6","player_5" };
    const char* bodyPaths[] = { "CHE","ATM","ARS","MCI","PSG","FCB" };

    for (int i = 0; i < count; ++i) {               //六名球员的模型展示
        const float cx = spacing + slotWidth * (i + 0.5f);
        Player preview;
        preview.pos = { cx, headCenterY };
        preview.playerface = 1;
        preview.loadHeadshot(
            std::string("assets/images/") + headPaths[i] + ".png");
        preview.loadBodyTexture(
            std::string("assets/images/") + bodyPaths[i] + ".png");
        preview.draw(window);

        sf::Text nameText(font);
        nameText.setString(names[i]);
        nameText.setCharacterSize(18);
        nameText.setFillColor(sf::Color::White);
        auto nb = nameText.getLocalBounds();
        nameText.setOrigin({ nb.position.x + nb.size.x * 0.5f,
                            nb.position.y + nb.size.y * 0.5f });
        nameText.setPosition({ cx, groundY + 28 });
        window.draw(nameText);
    }

    const float arrowSz = 18;
    const float headY = headCenterY - Constants::PlayerRadius;

    auto drawArrow = [&](int idx, sf::Color c, bool down) {
        float cx = spacing + slotWidth * (idx + 0.5f);
        sf::ConvexShape arrow(3);
        if (down) {
            arrow.setPoint(0, { cx, groundY + 45 });
            arrow.setPoint(1, { cx - arrowSz * 0.6f, groundY + 45 + arrowSz });
            arrow.setPoint(2, { cx + arrowSz * 0.6f, groundY + 45 + arrowSz });
        }
        else {
            arrow.setPoint(0, { cx, headY - 55 + arrowSz });
            arrow.setPoint(1, { cx - arrowSz * 0.6f, headY - 55 });
            arrow.setPoint(2, { cx + arrowSz * 0.6f, headY - 55 });
        }
        arrow.setFillColor(c);
        window.draw(arrow);
        };
    drawArrow(player1Index, { 255, 215, 0 }, false);
    drawArrow(player2Index, { 0, 200, 255 }, true);
}