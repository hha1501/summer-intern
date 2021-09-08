#pragma once

#include "Entity/Entity.h"
#include "Entity/PlayerEntity.h"
#include "Entity/KeyEntity.h"

#include "GridSlot.h"

#include "Camera.h"
#include "Sprite2D.h"
#include "AtlasSprite2D.h"

#include <memory>
#include <vector>
#include <unordered_map>

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

    enum class WorldState
    {
        Invalid,
        Playing,
        GravitySelecting,
        Updating,
        Ended
    };

public:
    GameWorld();

    void Init(int level);

    void ToggleGravitySelection();
    void SetGravity(GravityDirection gravityDirection);
    void MovePlayer(InputDirection direction);

    void Update(float deltaTime);
    void Draw();

    bool IsInGravtitySelection() const;

    bool IsGameOver() const;
    bool IsVictory() const;

private:
    void LoadMap(int id);
    void AddEntity(EntityType entityType, Vector2Int position);
    void CreateTiles(const std::vector<uint8_t>& tiles);

    bool TryMoveEntityUnderGravity(Entity* entity, Vector2Int newPosition);
    bool TryMovePlayerEntityUnderGravity(Entity* entity, Vector2Int newPosition, GridSlot& currentGridSlot, GridSlot& targetGridSlot);
    bool TryMoveBoxEntityUnderGravity(Entity* entity, Vector2Int newPosition, GridSlot& currentGridSlot, GridSlot& targetGridSlot);
    bool TryMoveRockEntityUnderGravity(Entity* entity, Vector2Int newPosition, GridSlot& currentGridSlot, GridSlot& targetGridSlot);

    bool TryMovePlayerUnderInput(InputDirection inputDirection);
    bool TryMoveBoxEntityUnderInput(Entity* entity, Vector2Int direction);

    void OnPlayerPickupKey();

    void MarkWorldAsChanged(bool byPlayerMovement);
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
    std::vector<std::unique_ptr<Entity>> m_keys;

    AtlasSprite2D* m_doorTile;

    PlayerEntity* m_player;
    uint8_t m_keysToCollectCount;
    bool m_gameOver;
    bool m_playerExited;

    GravityDirection m_currentGravityDirection;

    WorldState m_worldState;
    float m_updateTimer;

    uint8_t m_mapWidth;
    uint8_t m_mapHeight;
    std::vector<GridSlot> m_mapLookup;
    std::unordered_map<Vector2Int, Entity*, Vector2IntHasher> m_keyLookup;

    std::vector<Entity*> m_entityUpdateQueue;
    bool m_entitiesListChanged;
    bool m_keysListChanged;
};