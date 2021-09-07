#pragma once

#include "Sprite2D.h"
#include "../Common/Vector2Int.h"

#include <stdint.h>

enum class EntityType : uint8_t
{
    Player,
    Rock,
    Box,
    Key
};

class Entity
{
public:
    virtual ~Entity() = default;

    EntityType GetType() const { return m_entityType; }

    void Draw()
    { 
        m_sprite->Draw();
    }

    const Vector2Int& GetGridPosition() const
    {
        return m_gridPosition;
    }

    bool IsValid() const
    {
        return m_valid;
    }
    void Invalidate()
    {
        m_valid = false;
    }

    void SetPosition(Vector2Int gridPosition);
    void SetRotation(Vector3 rotation);
    void SetCamera(std::shared_ptr<Camera> camera);

    virtual void Update(float deltaTime)
    {
    }

protected:
    Entity(EntityType entityType) : m_entityType(entityType), m_valid(true)
    {
    }

    void LoadSprite(const char* texture);
    void LoadAtlasSprite(const char* texture, Vector2Int atlasSize);

protected:
    std::unique_ptr<Sprite2D> m_sprite;

    Vector2Int m_gridPosition;

private:
    EntityType m_entityType;
    bool m_valid;
};
