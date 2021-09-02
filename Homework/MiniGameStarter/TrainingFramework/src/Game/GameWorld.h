#pragma once

#include "Entity/Entity.h"
#include "Entity/PlayerEntity.h"
#include "Entity/KeyEntity.h"

#include "GridSlot.h"

#include "Camera.h"
#include "Sprite2D.h"

#include <memory>
#include <vector>

class GameWorld
{
public:
    enum class GravityDirection
    {
        Down = 0,
        Left = 1,
        Up = 2,
        Right = 3
    };

    enum class InputDirection
    {
        Left = 0,
        Right = 1
    };

public:
    GameWorld();

    void Init();

    void SetGravity(GravityDirection gravityDirection);
    void MovePlayer(InputDirection direction);

    void Update(float deltaTime);
    void Draw();

private:
    void LoadMap(int id);
    void AddEntity(EntityType entityType, Vector2Int position);
    void CreateTiles(const std::vector<uint8_t>& tiles);

    bool TryMoveEntityUnderGravity(Entity* entity, Vector2Int newPosition);
    bool TryMovePlayerEntityUnderGravity(Entity* entity, Vector2Int newPosition, GridSlot& currentGridSlot, GridSlot& targetGridSlot);
    bool TryMoveBoxEntityUnderGravity(Entity* entity, Vector2Int newPosition, GridSlot& currentGridSlot, GridSlot& targetGridSlot);
    bool TryMoveRockEntityUnderGravity(Entity* entity, Vector2Int newPosition, GridSlot& currentGridSlot, GridSlot& targetGridSlot);
    bool TryMoveKeyEntityUnderGravity(Entity* entity, Vector2Int newPosition, GridSlot& currentGridSlot, GridSlot& targetGridSlot);

    bool TryMovePlayerUnderInput(InputDirection inputDirection);
    bool TryMoveBoxEntityUnderInput(Entity* entity, Vector2Int direction);

    void MarkWorldAsChanged();
    void WorldStepOnce();
    bool ApplyGravity();

    void MarkEntityAsRemoved(Entity* entity);
    void ProcessEntityRemovePendingList();

    size_t GridPos2Index(Vector2Int position) const;
    static constexpr Vector2Int GravityDirection2Vector(GravityDirection gravityDirection);

private:
    std::shared_ptr<Camera> m_worldCamera;
    
    std::vector<std::unique_ptr<Sprite2D>> m_tiles;
    std::vector<std::unique_ptr<Entity>> m_entities;

    PlayerEntity* m_player;
    KeyEntity* m_keyEntity;
    bool m_playerHasKey;
    bool m_gameOver;
    bool m_playerExited;
    bool m_needToUpdate;

    GravityDirection m_currentGravityDirection;

    float m_updateTimer;

    uint8_t m_mapWidth;
    uint8_t m_mapHeight;
    std::vector<GridSlot> m_mapLookup;

    std::vector<Entity*> m_entityUpdateQueue;
    std::vector<Entity*> m_entityRemovePendingList;
};