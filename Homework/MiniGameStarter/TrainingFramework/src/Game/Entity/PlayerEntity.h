#pragma once

#include "Entity.h"

class PlayerEntity : public Entity
{
public:
    PlayerEntity();

    void SetFacingDirection(bool right);
    void SetFalling(bool falling);

    void Update(float deltaTime) override;

private:
    void UpdateAnimation();

private:
    float m_currentFrameTime;
    int m_currentIdleFrameNumber;
    bool m_isFacingRight;
    bool m_isFalling;
};
