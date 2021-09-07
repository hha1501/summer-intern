#include "PlayerEntity.h"

#include "AtlasSprite2D.h"

constexpr float c_idleFrameTime = 0.15f;
constexpr Vector2Int c_atlasSize = { 5, 2 };
constexpr int c_idleFrameCount = 4;
constexpr int c_fallingFrameNumber = 4;

constexpr int c_facingRightAtlasRow = 1;
constexpr int c_facingLeftAtlasRow = 0;

PlayerEntity::PlayerEntity() : Entity(EntityType::Player), m_currentFrameTime{}, m_currentIdleFrameNumber{}, m_isFacingRight(true), m_isFalling(false)
{
    LoadAtlasSprite("player\\v2\\player-idle.tga", c_atlasSize);
    UpdateAnimation();
}

void PlayerEntity::SetFacingDirection(bool right)
{
    m_isFacingRight = right;
    UpdateAnimation();
}

void PlayerEntity::SetFalling(bool falling)
{
    m_isFalling = falling;

    UpdateAnimation();
}

void PlayerEntity::Update(float deltaTime)
{
    m_currentFrameTime += deltaTime;

    if (m_currentFrameTime >= c_idleFrameTime)
    {
        m_currentIdleFrameNumber++;
        if (m_currentIdleFrameNumber >= c_idleFrameCount)
        {
            m_currentIdleFrameNumber = 0;
        }

        UpdateAnimation();

        m_currentFrameTime -= c_idleFrameTime;
    }
}

void PlayerEntity::UpdateAnimation()
{
    AtlasSprite2D* atlasSprite2D = static_cast<AtlasSprite2D*>(m_sprite.get());

    if (m_isFalling)
    {
        atlasSprite2D->SetAtlasCoord(Vector2Int(c_fallingFrameNumber, m_isFacingRight ? c_facingRightAtlasRow : c_facingLeftAtlasRow));
    }
    else
    {
        atlasSprite2D->SetAtlasCoord(Vector2Int(m_currentIdleFrameNumber, m_isFacingRight ? c_facingRightAtlasRow : c_facingLeftAtlasRow));
    }
}
