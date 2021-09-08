#pragma once

#include "Entity.h"

class PlayerEntity : public Entity
{
public:
    enum class AnimationState
    {
        Idle,
        Falling,
        GravityStop
    };

public:
    PlayerEntity();

    void SetFacingDirection(bool right);
    void SetAnimationState(AnimationState animationState);

    void Update(float deltaTime) override;

private:
    void UpdateAnimation();

private:
    float m_currentFrameTime;
    int m_currentIdleFrameNumber;
    bool m_isFacingRight;
    
    AnimationState m_currentAnimationState;
};
