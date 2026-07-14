#include "match_scene.h"
#include "constants.h"
#include "physics.h"
#include "pause_overlay.h"
#include "AudioManager.h"
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <cstdio>
#include <cmath>

MatchScene::MatchScene(const sf::Font& font)
    : font(font)
    , bgRect({ Constants::WindowWidth, Constants::WindowHeight })
    , arrow_1({ Constants::ArrowWidth, Constants::ArrowHeight })
    , arrow_2({ Constants::ArrowWidth, Constants::ArrowHeight })
    //, buffer_("assets/Audio/kick.wav")
    //, kick_(buffer_)
{
    bool bgOk = bgTexture.loadFromFile("assets/images/background.png");
    if (bgOk) {
        bgRect.setTexture(&bgTexture);
        bgRect.setTextureRect(sf::IntRect(
            { 0, 0 },
            { (int)bgTexture.getSize().x, (int)bgTexture.getSize().y }));
    }

    bool arOk = arTexture.loadFromFile("assets/images/arrow.png");
    if (arOk)
    {
        arrow_1.setTexture(&arTexture);
        arrow_2.setTexture(&arTexture);
        arrow_1.setTextureRect(sf::IntRect(
            { 0, 0 },
            { (int)arTexture.getSize().x, (int)arTexture.getSize().y }));
        arrow_2.setTextureRect(sf::IntRect(
            { 0, 0 },
            { (int)arTexture.getSize().x, (int)arTexture.getSize().y }));
        arrow_1.setScale({ 0.5f, 0.5f });
        arrow_2.setScale({ 0.5f, 0.5f });
    }

    // Preload head/body textures
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

    // Create pause overlay
    pauseOverlay = new PauseOverlay(font,
        [this]() { togglePause(); },
        [this]() { if (onBackToMenu) onBackToMenu(); });

    resetPositions();
}

MatchScene::~MatchScene()
{
    delete pauseOverlay;
}

void MatchScene::togglePause()
{
    if (matchFinished) return;
    paused = !paused;
}

void MatchScene::setPlayers(int p1, int p2)
{
    player1Type = p1; player2Type = p2;

    const char* heads[] = { "player_1","player_2","player_3",
                           "player_4","player_6","player_5" };
    const char* bodys[] = { "CHE","ATM","ARS","MCI","PSG","FCB" };

    if (p1 >= 0 && p1 < 6) {
        player1.loadHeadshot(
            std::string("assets/images/") + heads[p1] + ".png");
        player1.loadBodyTexture(
            std::string("assets/images/") + bodys[p1] + ".png");
    }
    if (p2 >= 0 && p2 < 6) {
        player2.loadHeadshot(
            std::string("assets/images/") + heads[p2] + ".png");
        player2.loadBodyTexture(
            std::string("assets/images/") + bodys[p2] + ".png");
    }
}

void MatchScene::resetPositions()
{
    player1.pos = { 300, Constants::GroundLevel - Constants::PlayerHeight };
    player2.pos = { 1300, Constants::GroundLevel - Constants::PlayerHeight };
    player1.velocity = player2.velocity = { 0, 0 };
    player1.moveInput = player2.moveInput = 0;
    player1.iskick = player2.iskick = false;
    player1.onGround = player2.onGround = true;
    player1.playerface = 1; player2.playerface = -1;

    ball.pos = { Constants::WindowWidth / 2.0f, 100 };
    ball.prevPos = ball.pos;
    ball.velocity = { 0, 0 };
    ball.onGround = false;

    aiPlayer.reset();
}

void MatchScene::restartMatch()
{
    score1 = score2 = 0;
    remainingMs = 90000;
    matchFinished = false;
    paused = false;
    resetPositions();
}

void MatchScene::handleEvent(const sf::Event& event)
{
    // Pause toggle (works even when paused or finished)
    if (const auto* key = event.getIf<sf::Event::KeyPressed>()) {
        if (key->code == sf::Keyboard::Key::Escape) {
            togglePause();
            return;
        }
    }

    // Delegate to pause overlay when paused
    if (paused && !matchFinished) {
        pauseOverlay->handleEvent(event);
        return;
    }

    if (matchFinished) {
        if (const auto* mb = event.getIf<sf::Event::MouseButtonPressed>())
            if (mb->button == sf::Mouse::Button::Left) restartMatch();
        return;
    }

    // Player 1 输入
    if (const auto* key = event.getIf<sf::Event::KeyPressed>()) 
    {
        switch (key->code) 
        {

        case sf::Keyboard::Key::A : p1LeftPressed = true; break;
        case sf::Keyboard::Key::D : p1RightPressed = true; break;
        case sf::Keyboard::Key::W : p1JumpPressed = true; break;
        case sf::Keyboard::Key::S : 
            if (!p1KickPressed) { p1KickPressed = true; }
            break;
        default: break;
        }

        // Player 2 输入 (仅双人模式中)
        if (!singlePlayer) {
            switch (key->code) {
            case sf::Keyboard::Key::Left:  p2LeftPressed = true; break;
            case sf::Keyboard::Key::Right: p2RightPressed = true; break;
            case sf::Keyboard::Key::Up:    p2JumpPressed = true; break;
            case sf::Keyboard::Key::Down:  
                if (!p2KickPressed) { p2KickPressed = true; }
                break;
            default: break;
            }
        }
    }

    if (const auto* key = event.getIf<sf::Event::KeyReleased>()) {
        switch (key->code) {
        case sf::Keyboard::Key::A: p1LeftPressed = false; break;
        case sf::Keyboard::Key::D: p1RightPressed = false; break;
        case sf::Keyboard::Key::W: p1JumpPressed = false; break;
        case sf::Keyboard::Key::S: p1KickPressed = false; break;
        default: break;
        }

        if (!singlePlayer) {
            switch (key->code) {
            case sf::Keyboard::Key::Left:  p2LeftPressed = false; break;
            case sf::Keyboard::Key::Right: p2RightPressed = false; break;
            case sf::Keyboard::Key::Up:    p2JumpPressed = false; break;
            case sf::Keyboard::Key::Down:  p2KickPressed = false; break;
            default: break;
            }
        }
    }
}

void MatchScene::update(float)
{
    if (matchFinished || paused) return;

    // ===== 玩家1：移动（持续响应，无卡顿） =====
    int p1Move = 0;
    if (p1LeftPressed)  p1Move -= 1;
    if (p1RightPressed) p1Move += 1;
    player1.move(p1Move);

    // 玩家1：跳跃
    if (p1JumpPressed) {
        player1.jump();
        p1JumpPressed = false; // 跳一次就消耗
    }

    // 玩家1：射门（上升沿触发，只在范围内生效）
    if (p1KickPressed && !p1KickWasPressed) {
        if (Physics::isInKickZone(player1, ball)) {  // 你需要自己实现这个判断
            player1.iskick = true;
        }
        // 不在范围内就忽略，不留痕迹
    }
    p1KickWasPressed = p1KickPressed;

    // ===== 玩家2：同理 =====
    if (!singlePlayer) 
    {
        int p2Move = 0;
        if (p2LeftPressed)  p2Move -= 1;
        if (p2RightPressed) p2Move += 1;
        player2.move(p2Move);

        if (p2JumpPressed) {
            player2.jump();
            p2JumpPressed = false;
        }

        if (p2KickPressed && !p2KickWasPressed) {
            if (Physics::isInKickZone(player2, ball)) {
                player2.iskick = true;
            }
        }
        p2KickWasPressed = p2KickPressed;
    }


    // AI controls player 2 before physics so inputs take effect this frame
    if (singlePlayer) {
        aiPlayer.update(player2, ball);
    }

    Actor* actors[] = { &player1, &player2, &ball };
    for (auto* actor : actors)
        actor->update();

    setArrow(player1, arrow_1);
    setArrow(player2, arrow_2);

    Physics::resolvePlayerStandingOnBall(player1, ball);
    Physics::resolvePlayerStandingOnBall(player2, ball);
    collision();
    checkGoalAndScore();

    remainingMs -= 16;
    if (remainingMs <= 0) {
        remainingMs = 0;
        matchFinished = true;
        if (onGameFinished) onGameFinished(score1, score2);
    }
}

void MatchScene::setArrow(const Player& player, sf::RectangleShape& arrow)
{
    arPos = ball.getPos();
    float directionRad = 0.f;
    sf::Vector2f KickDirection;

    // 获取形状的本地边界（SFML 3.x 语法）
    sf::FloatRect bounds = arrow.getLocalBounds();

    // 将原点设为宽度和高度的一半（即中心点）
    arrow.setOrigin({ 0, bounds.size.y / 2.f });

    const sf::Vector2f kickCenter = player.pos
        + sf::Vector2f(0.0f, (float)Constants::PlayerHeight);
    const sf::Vector2f toBall = ball.pos - kickCenter;
    const float forward = toBall.x * player.playerface;
    const float vertical = std::abs(toBall.y);

    const float fx = std::clamp(forward / Constants::KickRangeForward, 0.0f, 1.0f);
    const float vy = std::clamp(vertical / Constants::KickRangeVertical, 0.0f, 1.0f);
    float loft = std::clamp(
        (1.0f - fx) * 0.75f + (1.0f - vy) * 0.25f + Constants::KickLiftBias, 0.0f, 1.0f);
    const float angleDeg =
        Constants::KickMinAngleDeg
        + (Constants::KickMaxAngleDeg - Constants::KickMinAngleDeg) * loft;
    constexpr float pi = 3.14159265358979323846f;
    const float angleRad = angleDeg * pi / 180.0f;
    sf::Vector2f kickDirection(
        player.playerface * std::cos(angleRad), -std::sin(angleRad));
    const float t = 1.0f - std::clamp(std::hypot(fx, vy), 0.0f, 1.0f);
    float kickStrength =
        Constants::KickMinStrength
        + (Constants::KickMaxStrength - Constants::KickMinStrength) * t;
    if (kickDirection.x != 0.f || kickDirection.y != 0.f) {
        directionRad = std::atan2(kickDirection.y, kickDirection.x);
    }
    sf::Angle rotationAngle = sf::radians(directionRad);

    arPos += kickDirection * 25.0f;
    arrow.setPosition(arPos);
    arrow.setRotation(rotationAngle);
    arrow.setScale({ kickStrength / 80, kickStrength / 80 });
}

void MatchScene::collision()
{
    Physics::checkPlayerCollision(&player1, &player2);
    for (auto* p : { &player1, &player2 }) {
        if (!Physics::checkCollideBody(p, &ball))
            Physics::checkCollideHead(p, &ball);
    }
    Physics::checkGoalCrossbarCollision(&ball);
}

void MatchScene::checkGoalAndScore()
{
    const float goalTop = Constants::GroundLevel - Constants::GoalHeight;
    if (ball.pos.y < goalTop || ball.pos.y > Constants::GroundLevel) return;

    if (ball.pos.x <= Constants::GoalWidth) {
        ++score2; resetPositions();
    }
    else if (ball.pos.x >= Constants::WindowWidth - Constants::GoalWidth) {
        ++score1; resetPositions();
    }
}

void MatchScene::draw(sf::RenderWindow& window)
{
    window.draw(bgRect);

    const float gl = Constants::GroundLevel;
    const float gw = Constants::GoalWidth, gh = Constants::GoalHeight;

    auto drawGoal = [&](float x) {
        sf::RectangleShape g({ gw, gh });
        g.setPosition({ x, gl - gh });
        g.setFillColor(sf::Color::Transparent);
        g.setOutlineColor({ 245, 245, 245 });
        g.setOutlineThickness(5);
        window.draw(g);
        };
    drawGoal(0); drawGoal(Constants::WindowWidth - gw);

    sf::Color nc(255, 255, 255, 120);
    auto net = [&](float ox, float oy) {
        for (int i = 0; i <= (int)gw; i += 18) {
            sf::Vertex v[2];
            v[0].position = { ox + i, oy };      v[0].color = nc;
            v[1].position = { ox + i, oy + gh }; v[1].color = nc;
            window.draw(v, 2, sf::PrimitiveType::Lines);
        }
        for (int j = 0; j <= (int)gh; j += 18) {
            sf::Vertex v[2];
            v[0].position = { ox, oy + j };      v[0].color = nc;
            v[1].position = { ox + gw, oy + j }; v[1].color = nc;
            window.draw(v, 2, sf::PrimitiveType::Lines);
        }
        };
    net(0, gl - gh);
    net(Constants::WindowWidth - gw, gl - gh);

    auto bar = [&](float x) {
        sf::RectangleShape b({ gw, (float)Constants::GoalBarThickness });
        b.setPosition({ x, gl - gh - (float)Constants::GoalBarThickness });
        b.setFillColor({ 230, 230, 230 });
        window.draw(b);
        };
    bar(0); bar(Constants::WindowWidth - gw);

    Actor* actors[] = { &player1, &player2, &ball };
    for (auto* actor : actors)
        actor->draw(window);

    if (Physics::isInKickZone(player1, ball))
    {
        window.draw(arrow_1);
    }
    if (Physics::isInKickZone(player2, ball))
    {
        window.draw(arrow_2);
    }

    const char* abbrs[] = { "CHE","ATM","ARS","MCI","PSG","FCB" };
    sf::Color tcs[] = {
        {0x03,0x46,0x94},{0xCB,0x35,0x24},{0xEF,0x01,0x07},
        {0x6C,0xAB,0xDD},{0x00,0x41,0x70},{0xDC,0x05,0x2D}
    };
    const float sy = 28, rh = 42, rw = 110;

    {
        sf::RectangleShape r({ rw, rh }); r.setPosition({ 24, sy });
        r.setFillColor((player1Type >= 0 && player1Type < 6)
            ? tcs[player1Type] : sf::Color(0x44, 0x44, 0x44));
        window.draw(r);
    }
    {
        sf::RectangleShape r({ rw, rh }); r.setPosition({ 142, sy });
        r.setFillColor((player2Type >= 0 && player2Type < 6)
            ? tcs[player2Type] : sf::Color(0x44, 0x44, 0x44));
        window.draw(r);
    }

    auto drawScore = [&](float x, const char* abbr, int sc) {
        sf::Text t(font); char buf[64];
        std::snprintf(buf, sizeof(buf), "%s %d", abbr, sc);
        t.setString(buf); t.setCharacterSize(22);
        t.setFillColor(sf::Color::White);
        auto tb = t.getLocalBounds();
        t.setOrigin({ tb.position.x + tb.size.x * 0.5f,
                     tb.position.y + tb.size.y * 0.5f });
        t.setPosition({ x, sy + rh * 0.5f }); window.draw(t);
        };
    const char* a1 = (player1Type >= 0 && player1Type < 6) ? abbrs[player1Type] : "???";
    const char* a2 = (player2Type >= 0 && player2Type < 6) ? abbrs[player2Type] : "???";
    drawScore(79, a1, score1);
    drawScore(197, a2, score2);

    // AI indicator
    if (singlePlayer) {
        sf::Text aiLabel(font);
        aiLabel.setString("(AI)");
        aiLabel.setCharacterSize(14);
        aiLabel.setFillColor({ 255, 200, 50 });
        auto alb = aiLabel.getLocalBounds();
        aiLabel.setOrigin({ alb.position.x + alb.size.x * 0.5f,
                           alb.position.y + alb.size.y * 0.5f });
        aiLabel.setPosition({ 197 + rw * 0.5f, sy + rh + 12 });
        window.draw(aiLabel);
    }

    int sec = (int)remainingMs / 1000;
    sf::Text timer(font); char tbuf[16];
    std::snprintf(tbuf, sizeof(tbuf), "%02d:%02d", sec / 60, sec % 60);
    timer.setString(tbuf); timer.setCharacterSize(50);
    timer.setFillColor(sf::Color::White);
    auto ttb = timer.getLocalBounds();
    timer.setOrigin({ ttb.position.x + ttb.size.x * 0.5f,
                     ttb.position.y + ttb.size.y * 0.5f });
    timer.setPosition({ 800, 40 }); window.draw(timer);

    if (matchFinished) {
        sf::Text go(font);
        go.setString("Game Over - Click to Restart");
        go.setCharacterSize(36); go.setFillColor(sf::Color::White);
        auto gb = go.getLocalBounds();
        go.setOrigin({ gb.position.x + gb.size.x * 0.5f,
                      gb.position.y + gb.size.y * 0.5f });
        go.setPosition({ 800, 450 }); window.draw(go);
    }

    // Pause overlay (drawn last, on top of everything)
    if (paused && !matchFinished) {
        pauseOverlay->draw(window);
    }
}