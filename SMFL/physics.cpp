#include "physics.h"
#include "ball.h"
#include "player.h"
#include "buff.h"
#include "constants.h"
#include "AudioManager.h"
#include <algorithm>
#include <cmath>
#include <limits>

namespace
{
    struct AABB { float left, top, right, bottom; };

    inline float clampf(float v, float lo, float hi) { return std::max(lo, std::min(v, hi)); }

    bool sweptPointVsAABB(const sf::Vector2f& p0, const sf::Vector2f& p1,
        const AABB& box, float& tHit, sf::Vector2f& normal)
    {
        constexpr float inf = std::numeric_limits<float>::infinity();
        const float dx = p1.x - p0.x, dy = p1.y - p0.y, eps = 1e-7f;
        float tEnter = -inf, tExit = inf;
        normal = { 0, 0 };

        if (std::abs(dx) < eps) { if (p0.x < box.left || p0.x > box.right) return false; }
        else {
            float tx1 = (box.left - p0.x) / dx, tx2 = (box.right - p0.x) / dx;
            float txNear = std::min(tx1, tx2), txFar = std::max(tx1, tx2);
            if (txNear > tEnter) { tEnter = txNear; normal = { dx > 0 ? -1.0f : 1.0f, 0.0f }; }
            tExit = std::min(tExit, txFar);
        }

        if (std::abs(dy) < eps) { if (p0.y < box.top || p0.y > box.bottom) return false; }
        else {
            float ty1 = (box.top - p0.y) / dy, ty2 = (box.bottom - p0.y) / dy;
            float tyNear = std::min(ty1, ty2), tyFar = std::max(ty1, ty2);
            if (tyNear > tEnter) { tEnter = tyNear; normal = { 0.0f, dy > 0 ? -1.0f : 1.0f }; }
            tExit = std::min(tExit, tyFar);
        }

        if (tEnter > tExit || tExit < 0.0f || tEnter > 1.0f || tEnter < 0.0f) return false;
        tHit = tEnter; return true;
    }

    bool sweptPointVsCircle(const sf::Vector2f& p0, const sf::Vector2f& p1,
        const sf::Vector2f& center, float radius,
        float& tHit, sf::Vector2f& normal)
    {
        const sf::Vector2f d = p1 - p0, m = p0 - center;
        const float a = dot(d, d), b = 2.0f * dot(m, d), c = dot(m, m) - radius * radius;
        if (c <= 0.0f || a <= 1e-12f) return false;
        const float disc = b * b - 4.0f * a * c;
        if (disc < 0.0f) return false;
        const float t = (-b - std::sqrt(disc)) / (2.0f * a);
        if (t < 0.0f || t > 1.0f) return false;
        normal = (p0 + d * t) - center;
        const float nLen = std::sqrt(dot(normal, normal));
        normal = (nLen <= 1e-9f) ? sf::Vector2f(1.0f, 0.0f) : normal / nLen;
        tHit = t; return true;
    }

    bool resolveBallAABBCollision(Ball* ball, const AABB& box, float restitution)
    {
        float cx = clampf(ball->pos.x, box.left, box.right);
        float cy = clampf(ball->pos.y, box.top, box.bottom);
        sf::Vector2f closest(cx, cy), diff = ball->pos - closest;
        float dist2 = dot(diff, diff);
        const float r = (float)Constants::BallRadius;
        if (dist2 >= r * r) return false;

        sf::Vector2f normal;
        float dist = std::sqrt(std::max(dist2, 1e-12f));
        if (dist > 1e-6f) { normal = diff / dist; ball->pos += normal * (r - dist); }
        else {
            float toLeft = std::abs(ball->pos.x - box.left), toRight = std::abs(box.right - ball->pos.x);
            float toTop = std::abs(ball->pos.y - box.top), toBottom = std::abs(box.bottom - ball->pos.y);
            float minD = std::min({ toLeft, toRight, toTop, toBottom });
            if (minD == toLeft) normal = { -1, 0 };
            else if (minD == toRight) normal = { 1, 0 };
            else if (minD == toTop) normal = { 0, -1 };
            else normal = { 0, 1 };
            ball->pos += normal * (r + 0.1f);
        }
        float vn = dot(ball->velocity, normal);
        if (vn < 0.0f) ball->velocity -= (1.0f + restitution) * vn * normal;
        return true;
    }
}

void Physics::checkGoalCrossbarCollision(Ball* ball)
{
    const float goalTop = (float)(Constants::GroundLevel - Constants::GoalHeight);
    const AABB leftBar{ 0.0f, goalTop - Constants::GoalBarThickness, (float)Constants::GoalWidth, goalTop };
    const AABB rightBar{ (float)(Constants::WindowWidth - Constants::GoalWidth),
                        goalTop - Constants::GoalBarThickness,
                        (float)Constants::WindowWidth, goalTop };
    resolveBallAABBCollision(ball, leftBar, 0.55f);
    resolveBallAABBCollision(ball, rightBar, 0.55f);
}

void Physics::checkBallBoundary(Ball* ball)
{
    if (ball->pos.y + Constants::BallRadius >= Constants::GroundLevel) {
        ball->pos.y = Constants::GroundLevel - Constants::BallRadius;
        if (std::abs(ball->velocity.y) < Constants::BallSleepVy) ball->velocity.y = 0.0f;
        else ball->velocity.y = -ball->velocity.y * Constants::BallRestitutionGround;
        ball->velocity.x *= Constants::BallGroundFriction;
        ball->onGround = true;
    }
    else ball->onGround = false;

    if (ball->pos.y - Constants::BallRadius <= 0) {
        ball->pos.y = Constants::BallRadius;
        ball->velocity.y = -ball->velocity.y * Constants::BallRestitutionWall;
    }
    if (ball->pos.x - Constants::BallRadius <= 0) {
        ball->pos.x = Constants::BallRadius;
        ball->velocity.x = -ball->velocity.x * Constants::BallRestitutionWall;
    }
    if (ball->pos.x + Constants::BallRadius >= Constants::WindowWidth) {
        ball->pos.x = Constants::WindowWidth - Constants::BallRadius;
        ball->velocity.x = -ball->velocity.x * Constants::BallRestitutionWall;
    }
}

void Physics::checkPlayerBoundary(Player* player)
{
    if (player->pos.y + Constants::PlayerHeight > Constants::GroundLevel) {
        player->pos.y = Constants::GroundLevel - Constants::PlayerHeight;
        player->onGround = true;
        player->velocity.y = 0;
    }
    if (player->pos.x - Constants::PlayerRadius < 0) {
        player->pos.x = Constants::PlayerRadius;
        player->velocity.x = 0;
    }
    if (player->pos.x + Constants::PlayerRadius > Constants::WindowWidth) {
        player->pos.x = Constants::WindowWidth - Constants::PlayerRadius;
        player->velocity.x = 0;
    }
}

void Physics::checkPlayerCollision(Player* player1, Player* player2)
{
    sf::Vector2f orient = player2->pos - player1->pos;
    float dist = std::sqrt(orient.x * orient.x + orient.y * orient.y);
    float combinedR = 2.0f * Constants::PlayerRadius;
    if (combinedR - dist <= 0.01f) return;
    player1->velocity.x = 0; player2->velocity.x = 0;
    player1->pos.x -= 0.8f * (combinedR - dist) * orient.x / dist;
    player2->pos.x += 0.8f * (combinedR - dist) * orient.x / dist;
}

bool Physics::checkCollideHead(Player* player, Ball* ball)
{
    const sf::Vector2f headCenter = player->pos;
    const float combinedR = (float)(Constants::PlayerRadius + Constants::BallRadius);
    bool hit = false;
    sf::Vector2f normal(0.0f, 0.0f);
    float tHit = 0.0f;

    if (sweptPointVsCircle(ball->prevPos, ball->pos, headCenter, combinedR, tHit, normal)) {
        ball->pos = ball->prevPos + (ball->pos - ball->prevPos) * tHit + normal * 0.05f;
        hit = true;
    }
    else {
        sf::Vector2f diff = ball->pos - headCenter;
        float dist2 = dot(diff, diff);
        if (dist2 < combinedR * combinedR) {
            float dist = std::sqrt(std::max(dist2, 1e-12f));
            normal = (dist > 1e-6f) ? diff / dist : sf::Vector2f(1.0f, 0.0f);
            ball->pos += normal * ((dist > 1e-6f) ? (combinedR - dist) : 0.1f);
            hit = true;
        }
    }
    if (!hit) return false;

    sf::Vector2f vRel = ball->velocity - player->velocity;
    float vn = dot(vRel, normal);
    if (vn < 0.0f) {
        constexpr float mPlayer = 60.0f, mBall = 0.5f, restitution = 0.5f;
        float j = -(1.0f + restitution) * vn / (1.0f / mPlayer + 1.0f / mBall);
        ball->velocity += (j / mBall) * normal;
        player->velocity -= (j / mPlayer) * normal;
    }
    ball->lastKicker = player;
    checkBallBoundary(ball);

    static sf::Clock hitCooldownClock;
    if (hitCooldownClock.getElapsedTime().asSeconds() >= 0.2f) {
        AudioManager::GetInstance()->play("hit");
        hitCooldownClock.restart();
    }

    return true;
}

bool Physics::isInKickZone(const Player& player, const Ball& ball)
{
    const sf::Vector2f kickCenter = player.pos
        + sf::Vector2f(0.0f, static_cast<float>(Constants::PlayerHeight));
    const sf::Vector2f toBall = ball.pos - kickCenter;
    const float forward = toBall.x * static_cast<float>(player.playerface);
    const float vertical = std::abs(toBall.y);

    return forward >= -Constants::KickRangeBackward
        && forward <= Constants::KickRangeForward
        && vertical <= Constants::KickRangeVertical;
}

void Physics::resolvePlayerStandingOnBall(Player& player, Ball& ball)
{
    // 只有球员在下落或已经站在球上时才处理
    if (player.velocity.y <= 0.0f || player.onGround)
        return;

    const float footY = player.pos.y + Constants::PlayerHeight;
    const float ballTopY = ball.pos.y - Constants::BallRadius;
    const float penetration = footY - ballTopY;
    if (penetration <= 0.0f || penetration >= Constants::BallRadius * 2)
        return;

    // 脚底水平区间
    const float footLeft = player.pos.x - (Constants::PlayerBodyWidth) / 2;
    const float footRight = player.pos.x + (Constants::PlayerBodyWidth) / 2;
    const float ballLeft = ball.pos.x - Constants::BallRadius;
    const float ballRight = ball.pos.x + Constants::BallRadius;

    // 没有水平重叠就不处理
    if (ballRight < footLeft || ballLeft > footRight)
        return;

    // 1. 位置修正：只解决穿透，不锁死
    player.pos.y = ballTopY - Constants::PlayerHeight;
    player.velocity.y = 0.0f;
    player.onGround = true; // 暂时认为着地，让跳跃等逻辑正常

    // 2. 水平分离：根据相对位置施加推开速度
    float dx = player.pos.x - ball.pos.x;
    float sign = (dx > 0.0f) ? 1.0f : -1.0f;

    // 分离力度，可以根据需要调整
    const float separationForce = 5.0f; // 推开速度
    player.velocity.x += sign * separationForce * 0.5f; // 球员往远离球方向滑
    ball.velocity.x -= sign * separationForce * 0.5f; // 球往反方向滚

    // 3. 微小上抛，模拟“踩到边缘被弹起”的感觉
    player.velocity.y = -12.0f; // 向上弹一下，避免卡住
    player.onGround = false;    // 弹起后不在地面

    // 4. 球也受到一点向下的力，模拟被踩
    ball.velocity.y = std::max(ball.velocity.y, -3.0f);

    // 防止球被压入地面
    checkBallBoundary(&ball);
}

bool Physics::checkCollideBody(Player* player, Ball* ball)
{
    if (player->iskick) {
        const sf::Vector2f kickCenter = player->pos
            + sf::Vector2f(0.0f, (float)Constants::PlayerHeight);
        const sf::Vector2f toBall = ball->pos - kickCenter;
        const float forward = toBall.x * player->playerface;
        const float vertical = std::abs(toBall.y);

        if (isInKickZone(*player, *ball)) {
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
                player->playerface * std::cos(angleRad), -std::sin(angleRad));
            const float t = 1.0f - std::clamp(std::hypot(fx, vy), 0.0f, 1.0f);
            float kickStrength =
                Constants::KickMinStrength
                + (Constants::KickMaxStrength - Constants::KickMinStrength) * t;
            ball->velocity += kickStrength * kickDirection;
            ball->velocity.x += player->velocity.x * 0.35f;
            ball->lastKicker = player;

            player->iskick = false;
            static sf::Clock hitCooldownClock;  // 静态时钟，只会初始化一次
            if (hitCooldownClock.getElapsedTime().asSeconds() >= 0.2f) {
                AudioManager::GetInstance()->play("kick");
                hitCooldownClock.restart();  // 重置计时器
            }
            return true;
        }
    }

    const float bodyLeft = player->pos.x - Constants::PlayerBodyWidth * 0.5f;
    const float bodyTop = player->pos.y + Constants::PlayerRadius;
    const float bodyRight = bodyLeft + Constants::PlayerBodyWidth;
    const float bodyBottom = bodyTop + Constants::PlayerBodyHeight;
    AABB expanded{ bodyLeft - Constants::BallRadius, bodyTop - Constants::BallRadius,
                  bodyRight + Constants::BallRadius, bodyBottom + Constants::BallRadius };

    bool hit = false;
    sf::Vector2f normal(0, 0);
    float tHit = 0.0f;

    if (sweptPointVsAABB(ball->prevPos, ball->pos, expanded, tHit, normal)) {
        ball->pos = ball->prevPos + (ball->pos - ball->prevPos) * tHit + normal * 0.05f;
        hit = true;
    }
    else {
        float cx = clampf(ball->pos.x, bodyLeft, bodyRight);
        float cy = clampf(ball->pos.y, bodyTop, bodyBottom);
        sf::Vector2f closest(cx, cy), diff = ball->pos - closest;
        float dist2 = diff.x * diff.x + diff.y * diff.y;
        float r = Constants::BallRadius;
        if (dist2 < r * r) {
            float dist = std::sqrt(std::max(dist2, 1e-12f));
            if (dist > 1e-6f) { normal = diff / dist; ball->pos += normal * (r - dist); }
            else {
                float toLeft = std::abs(ball->pos.x - bodyLeft);
                float toRight = std::abs(bodyRight - ball->pos.x);
                float toTop = std::abs(ball->pos.y - bodyTop);
                float toBottom = std::abs(bodyBottom - ball->pos.y);
                float minD = std::min({ toLeft, toRight, toTop, toBottom });
                if (minD == toLeft) normal = { -1, 0 };
                else if (minD == toRight) normal = { 1, 0 };
                else if (minD == toTop) normal = { 0, -1 };
                else normal = { 0, 1 };
                ball->pos += normal * (r + 0.1f);
            }
            hit = true;
        }
    }
    if (!hit) return false;

    const bool playerFalling = player->velocity.y > 0.0f;
    const bool ballBelowBody = normal.y > 0.5f;
    const bool ballNearGround =
        (Constants::GroundLevel - (ball->pos.y + Constants::BallRadius)) < 2.0f;

    if (playerFalling && ballBelowBody && ballNearGround) {
        player->pos.y = (ball->pos.y - Constants::BallRadius) - Constants::PlayerHeight;
        player->velocity.y = 0.0f; player->onGround = true;
        ball->velocity.y = 0.0f;
        ball->velocity.x = ball->velocity.x * 0.98f + player->velocity.x * 0.02f;
        checkBallBoundary(ball);
        return true;
    }

    float vn = dot(ball->velocity - player->velocity, normal);
    if (vn < 0.0f) {
        constexpr float restitution = 0.25f;
        ball->velocity -= (1.0f + restitution) * vn * normal;
        sf::Vector2f tangent(-normal.y, normal.x);
        float vt = dot(ball->velocity, tangent);
        ball->velocity -= 0.10f * vt * tangent;
    }
    checkBallBoundary(ball);

    //static sf::Clock hitCooldownClock;
    //if (hitCooldownClock.getElapsedTime().asSeconds() >= 0.2f) {
    //    AudioManager::GetInstance()->play("hit");
    //    hitCooldownClock.restart();
    //}

    return true;
}

bool Physics::checkBuffCollision(const Ball& ball, const Buff& buff)
{
    const sf::Vector2f diff = ball.pos - buff.pos;
    const float dist2 = dot(diff, diff);
    const float combinedR = Constants::BallRadius + Buff::radius();
    if (dist2 < combinedR * combinedR)
    {
        AudioManager::GetInstance()->play("select");
    }
    return dist2 < combinedR * combinedR;
}