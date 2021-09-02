#include "GameWorld.h"

#include "GameWorldConfig.h"
#include "Map/GameMap.h"

#include "Entity/BoxEntity.h"
#include "Entity/RockEntity.h"

#include "Tile/TileSpriteLoader.h"

#include "ResourceManagers.h"

#include <array>
#include <algorithm>
#include <assert.h>

using namespace GameWorldConfig;

GameWorld::GameWorld() : m_worldCamera{}, m_tiles{}, m_entities{}, m_player{}, m_keyEntity{},
m_playerHasKey(false), m_gameOver(false), m_playerExited(false), m_needToUpdate(false),
m_currentGravityDirection{}, m_updateTimer{},
m_mapWidth{}, m_mapHeight{}, m_mapLookup{}, m_entityUpdateQueue{}, m_entityRemovePendingList{}
{
}

void GameWorld::Init()
{
    m_worldCamera = std::make_shared<Camera>(0, 0.0f, c_mapWidth, c_mapHeight, 0.0f, -1.0f, 1.0f, 0.0f);

    LoadMap(1);
}

void GameWorld::SetGravity(GravityDirection gravityDirection)
{
    if (m_needToUpdate)
    {
        return;
    }

    if (gravityDirection == GravityDirection::Down)
    {
        return;
    }

    size_t gravityIndex = (size_t)m_currentGravityDirection + (size_t)gravityDirection;
    if (gravityIndex >= 4)
    {
        gravityIndex -= 4;
    }

    m_currentGravityDirection = (GravityDirection)gravityIndex;

    constexpr std::array<float, 4> directionToZAngle
    {
        0.0f,
        PI * 1.5f,
        PI * 1.0f,
        PI * 0.5f
    };

    float zAngle = directionToZAngle[(size_t)m_currentGravityDirection];

    // Rotate camera
    m_worldCamera->BeginUpdate()
        .SetRotationZ(-zAngle)
        .ApplyChanges();

    // Rotate entities
    for (auto& entity : m_entities)
    {
        entity->SetRotation(Vector3(0, 0, zAngle));
    }

    MarkWorldAsChanged();
}

void GameWorld::MovePlayer(InputDirection direction)
{
    if (m_needToUpdate)
    {
        return;
    }

    if (TryMovePlayerUnderInput(direction))
    {
        MarkWorldAsChanged();
    }
}

void GameWorld::Update(float deltaTime)
{
    constexpr float c_frameTime = 0.2f;

    m_updateTimer += deltaTime;
    if (m_updateTimer >= c_frameTime)
    {
        if (m_needToUpdate)
        {
            WorldStepOnce();
        }

        m_updateTimer -= c_frameTime;
    }
}

void GameWorld::Draw()
{
    for (auto& tile : m_tiles)
    {
        tile->Draw();
    }

    for (auto& entity : m_entities)
    {
        if (entity->IsValid())
        {
            entity->Draw();
        }
    }
}

void GameWorld::LoadMap(int id)
{
    GameMap gameMap = GameMap::LoadFromFile(id);

    m_mapWidth = gameMap.Width();
    m_mapHeight = gameMap.Height();

    m_mapLookup = std::vector<GridSlot>(m_mapWidth * m_mapHeight);

    // Create tiles
    CreateTiles(gameMap.Tiles());

    // Create entities
    const std::vector<GameMap::EntityEntry>& entityEntries = gameMap.Entities();

    for (const auto& entityEntry : entityEntries)
    {
        AddEntity((EntityType)entityEntry.entityType, Vector2Int(entityEntry.x, entityEntry.y));
    }
}

void GameWorld::AddEntity(EntityType entityType, Vector2Int position)
{
    Entity* newEntity = nullptr;

    switch (entityType)
    {
    case EntityType::Player: {
        std::unique_ptr<PlayerEntity> playerEntity = std::make_unique<PlayerEntity>();
        newEntity = playerEntity.get();
        m_player = playerEntity.get();

        playerEntity->SetPosition(position);
        playerEntity->SetCamera(m_worldCamera);

        m_entities.push_back(std::move(playerEntity));
        break;
    }
    case EntityType::Rock: {
        std::unique_ptr<RockEntity> rockEntity = std::make_unique<RockEntity>();
        newEntity = rockEntity.get();

        rockEntity->SetPosition(position);
        rockEntity->SetCamera(m_worldCamera);

        m_entities.push_back(std::move(rockEntity));
        break;
    }
    case EntityType::Box: {
        std::unique_ptr<BoxEntity> boxEntity = std::make_unique<BoxEntity>();
        newEntity = boxEntity.get();

        boxEntity->SetPosition(position);
        boxEntity->SetCamera(m_worldCamera);

        m_entities.push_back(std::move(boxEntity));
        break;
    }
    case EntityType::Key: {
        std::unique_ptr<KeyEntity> keyEntity = std::make_unique<KeyEntity>();
        newEntity = keyEntity.get();
        m_keyEntity = keyEntity.get();

        keyEntity->SetPosition(position);
        keyEntity->SetCamera(m_worldCamera);

        m_entities.push_back(std::move(keyEntity));
        break;
    }
    default:
        break;
    }

    GridSlot& gridSlot = m_mapLookup[GridPos2Index(position)];
    GridSlot::ContentType slotContentType = gridSlot.Type();

    // TODO: Convert this into bit mask checking
    if (slotContentType == GridSlot::ContentType::Empty)
    {
        gridSlot = GridSlot(newEntity);
    }
    else if (slotContentType == GridSlot::ContentType::Entity)
    {
        EntityType slotEntityType = gridSlot.AsEntity()->GetType();

        if (entityType == EntityType::Key && (slotEntityType == EntityType::Box || slotEntityType == EntityType::Rock))
        {
            // Case 1: Key added to grid slot
            gridSlot = GridSlot(gridSlot.AsEntity(), true);
        }
        else if (slotEntityType == EntityType::Key && (entityType == EntityType::Box || entityType == EntityType::Rock))
        {
            // Case 2: Entity added to grid slot that already contains a key
            gridSlot = GridSlot(newEntity, true);
        }
        else
        {
            assert(false && "Invalid GridSlot fusion");
        }
    }
    else
    {
        assert(false && "Invalid GridSlot fusion");
    }
}

void GameWorld::CreateTiles(const std::vector<uint8_t>& tiles)
{
    TileSpriteLoader tileSpriteLoader;

    for (size_t y = 0; y < m_mapHeight; y++)
    {
        for (size_t x = 0; x < m_mapWidth; x++)
        {
            uint8_t tileData = tiles[GridPos2Index(Vector2Int(x, y))];
            TileType tileType = (TileType)(tileData & GameMap::c_tileTypeMask);
            uint8_t tileDetails = tileData >> GameMap::c_tileDetailsMaskShift;

            if (tileType != TileType::Empty)
            {
                std::unique_ptr<Sprite2D> tileSprite = tileSpriteLoader.LoadSprite(tileType, tileDetails, Vector2Int(x, y));
                tileSprite->SetCamera(m_worldCamera);

                GridSlot& gridSlot = m_mapLookup[GridPos2Index(Vector2Int(x, y))];
                if (gridSlot.Type() == GridSlot::ContentType::Empty)
                {
                    gridSlot = GridSlot(tileType);
                }
                else
                {
                    assert(false && "Invalid GridSlot fusion");
                }

                m_tiles.push_back(std::move(tileSprite));
            }
        }
    }
}


bool GameWorld::TryMoveEntityUnderGravity(Entity* entity, Vector2Int newPosition)
{
    // Check bounds
    if (newPosition.x < 0 || newPosition.x >= m_mapWidth || newPosition.y < 0 || newPosition.y >= m_mapHeight)
    {
        return false;
    }

    GridSlot& currentGridSlot = m_mapLookup[GridPos2Index(entity->GetGridPosition())];
    GridSlot& targetGridSlot = m_mapLookup[GridPos2Index(newPosition)];

    if (targetGridSlot.Type() == GridSlot::ContentType::Tile)
    {
        return false;
    }

    // Special case: moving to an empty slot
    if (targetGridSlot.Type() == GridSlot::ContentType::Empty)
    {
        // Update grid slots, decompose if needed
        if (currentGridSlot.Type() == GridSlot::ContentType::EntityAndKey)
        {
            if (entity->GetType() == EntityType::Key)
            {
                // Remove key from fused grid slot
                currentGridSlot = GridSlot(currentGridSlot.AsEntity());
            }
            else
            {
                // Remove entity from fused grid slot
                currentGridSlot = GridSlot(m_keyEntity);
            }
        }
        else
        {
            currentGridSlot = GridSlot();
        }

        targetGridSlot = GridSlot(entity);

        // Update position
        entity->SetPosition(newPosition);

        return true;
    }

    // Moving to a slot that contains entities
    switch (entity->GetType())
    {
    case EntityType::Player:
        return TryMovePlayerEntityUnderGravity(entity, newPosition, currentGridSlot, targetGridSlot);
        break;
    case EntityType::Box:
        return TryMoveBoxEntityUnderGravity(entity, newPosition, currentGridSlot, targetGridSlot);
        break;
    case EntityType::Rock:
        return TryMoveRockEntityUnderGravity(entity, newPosition, currentGridSlot, targetGridSlot);
        break;
    case EntityType::Key:
        return TryMoveKeyEntityUnderGravity(entity, newPosition, currentGridSlot, targetGridSlot);
        break;
    default:
        break;
    }

    return false;
}

bool GameWorld::TryMovePlayerEntityUnderGravity(Entity* entity, Vector2Int newPosition, GridSlot& currentGridSlot, GridSlot& targetGridSlot)
{
    if (targetGridSlot.AsEntity()->GetType() == EntityType::Key)
    {
        // Move to a key
        // Remove key
        MarkEntityAsRemoved(m_keyEntity);
        m_keyEntity = nullptr;

        m_playerHasKey = true;

        // Update grid slots
        currentGridSlot = GridSlot();
        targetGridSlot = GridSlot(entity);

        // Update position
        entity->SetPosition(newPosition);

        return true;
    }

    // Blocked
    return false;
}

bool GameWorld::TryMoveBoxEntityUnderGravity(Entity* entity, Vector2Int newPosition, GridSlot& currentGridSlot, GridSlot& targetGridSlot)
{
    if (targetGridSlot.AsEntity()->GetType() == EntityType::Key)
    {
        // Move to a key
        // Fuse with key
        currentGridSlot = GridSlot();
        targetGridSlot = GridSlot(entity, true);

        // Update position
        entity->SetPosition(newPosition);

        return true;
    }

    // Blocked
    return false;
}

bool GameWorld::TryMoveRockEntityUnderGravity(Entity* entity, Vector2Int newPosition, GridSlot& currentGridSlot, GridSlot& targetGridSlot)
{
    EntityType slotEntityType = targetGridSlot.AsEntity()->GetType();
    if (slotEntityType == EntityType::Key)
    {
        // Move to a key
        // Fuse with key
        currentGridSlot = GridSlot();
        targetGridSlot = GridSlot(entity, true);

        // Update position
        entity->SetPosition(newPosition);

        return true;
    }
    else if (slotEntityType == EntityType::Box)
    {
        // Move to a box
        // Destroy box
        MarkEntityAsRemoved(targetGridSlot.AsEntity());

        // Update grid slots, decompose and fuse if needed
        if (currentGridSlot.Type() == GridSlot::ContentType::EntityAndKey)
        {
            // Remove entity from fused grid slot
            currentGridSlot = GridSlot(m_keyEntity);
            targetGridSlot = GridSlot(entity);
        }
        else
        {
            currentGridSlot = GridSlot();
            if (targetGridSlot.Type() == GridSlot::ContentType::EntityAndKey)
            {
                targetGridSlot = GridSlot(entity, true);
            }
            else
            {
                targetGridSlot = GridSlot(entity);
            }
        }

        // Update position
        entity->SetPosition(newPosition);

        return true;
    }
    else if (slotEntityType == EntityType::Player)
    {
        // Move to player
        // Destroy player
        MarkEntityAsRemoved(m_player);
        m_player = nullptr;

        m_gameOver = true;

        // Update grid slots, decompose if needed
        if (currentGridSlot.Type() == GridSlot::ContentType::EntityAndKey)
        {
            // Remove entity from fused grid slot
            currentGridSlot = GridSlot(m_keyEntity);
            targetGridSlot = GridSlot(entity);
        }
        else
        {
            currentGridSlot = GridSlot();
            targetGridSlot = GridSlot(entity);
        }

        // Update position
        entity->SetPosition(newPosition);

        return true;
    }

    // Blocked
    return false;
}

bool GameWorld::TryMoveKeyEntityUnderGravity(Entity* entity, Vector2Int newPosition, GridSlot& currentGridSlot, GridSlot& targetGridSlot)
{
    // Update current grid slots
    if (currentGridSlot.Type() == GridSlot::ContentType::EntityAndKey)
    {
        // Remove key from fused slot
        currentGridSlot = GridSlot(currentGridSlot.AsEntity());
    }
    else
    {
        currentGridSlot = GridSlot();
    }


    if (targetGridSlot.AsEntity()->GetType() == EntityType::Player)
    {
        // Move to player
        // Remove key
        MarkEntityAsRemoved(m_keyEntity);
        m_keyEntity = nullptr;

        m_playerHasKey = true;

        // Target slot stays unchanged

        // Update position
        entity->SetPosition(newPosition);

        return true;
    }
    else
    {
        // Fuse with target slot
        targetGridSlot = GridSlot(targetGridSlot.AsEntity(), true);
    }

    // Update position
    entity->SetPosition(newPosition);

    return true;
}

bool GameWorld::TryMovePlayerUnderInput(InputDirection inputDirection)
{
    constexpr std::array<Vector2Int, 4> gravityIndexToLeftVector
    {
        Vector2Int::Left(),
        Vector2Int::Up(),
        Vector2Int::Right(),
        Vector2Int::Down()
    };

    Vector2Int leftVector = gravityIndexToLeftVector[(size_t)m_currentGravityDirection];
    Vector2Int direction = inputDirection == InputDirection::Left ? leftVector : -leftVector;

    Entity* entity = m_player;
    Vector2Int newPosition = entity->GetGridPosition() + direction;

    // Check bounds
    if (newPosition.x < 0 || newPosition.x >= m_mapWidth || newPosition.y < 0 || newPosition.y >= m_mapHeight)
    {
        return false;
    }

    GridSlot& currentGridSlot = m_mapLookup[GridPos2Index(entity->GetGridPosition())];
    GridSlot& targetGridSlot = m_mapLookup[GridPos2Index(newPosition)];

    if (targetGridSlot.Type() == GridSlot::ContentType::Tile)
    {
        if (targetGridSlot.AsTile() == TileType::Exit && m_playerHasKey && m_currentGravityDirection == GravityDirection::Down)
        {
            currentGridSlot = GridSlot();
            m_playerExited = true;

            return true;
        }

        return false;
    }

    while (true)
    {
        // Special case: moving to an empty slot
        if (targetGridSlot.Type() == GridSlot::ContentType::Empty)
        {
            currentGridSlot = GridSlot();
            targetGridSlot = GridSlot(entity);

            // Update position
            entity->SetPosition(newPosition);

            return true;
        }

        EntityType targetSlotEntityType = targetGridSlot.AsEntity()->GetType();

        // Move to a rock
        if (targetSlotEntityType == EntityType::Rock)
        {
            return false;
        }

        if (targetSlotEntityType == EntityType::Key)
        {
            // Move to a key
            // Remove key
            MarkEntityAsRemoved(m_keyEntity);
            m_keyEntity = nullptr;

            m_playerHasKey = true;

            // Update grid slots
            currentGridSlot = GridSlot();
            targetGridSlot = GridSlot(entity);

            // Update position
            entity->SetPosition(newPosition);

            return true;
        }

        // Moving to a slot that contains a box
        if (targetSlotEntityType == EntityType::Box)
        {
            // Try to move the blocking box out of the way
            if (!TryMoveBoxEntityUnderInput(targetGridSlot.AsEntity(), direction))
            {
                return false;
            }
        }

        // Try to move again
    }

    // Blocked
    return false;
}

bool GameWorld::TryMoveBoxEntityUnderInput(Entity* entity, Vector2Int direction)
{
    Vector2Int newPosition = entity->GetGridPosition() + direction;

    // Check bounds
    if (newPosition.x < 0 || newPosition.x >= m_mapWidth || newPosition.y < 0 || newPosition.y >= m_mapHeight)
    {
        return false;
    }

    GridSlot& currentGridSlot = m_mapLookup[GridPos2Index(entity->GetGridPosition())];
    GridSlot& targetGridSlot = m_mapLookup[GridPos2Index(newPosition)];

    if (targetGridSlot.Type() == GridSlot::ContentType::Tile)
    {
        return false;
    }

    while (true)
    {
        // Special case: moving to an empty slot
        if (targetGridSlot.Type() == GridSlot::ContentType::Empty)
        {
            // Update grid slots, decompose if needed
            if (currentGridSlot.Type() == GridSlot::ContentType::EntityAndKey)
            {
                // Remove entity from fused grid slot
                currentGridSlot = GridSlot(m_keyEntity);
            }
            else
            {
                currentGridSlot = GridSlot();
            }

            targetGridSlot = GridSlot(entity);

            // Update position
            entity->SetPosition(newPosition);

            return true;
        }

        EntityType targetSlotEntityType = targetGridSlot.AsEntity()->GetType();

        // Move to a rock
        if (targetSlotEntityType == EntityType::Rock)
        {
            return false;
        }

        // Move to a key
        if (targetSlotEntityType == EntityType::Key)
        {
            // Fuse with key
            currentGridSlot = GridSlot();
            targetGridSlot = GridSlot(entity, true);

            // Update position
            entity->SetPosition(newPosition);

            return true;
        }

        // Moving to a slot that contains another box
        if (targetSlotEntityType == EntityType::Box)
        {
            // Try to move the blocking box out of the way
            if (!TryMoveBoxEntityUnderInput(targetGridSlot.AsEntity(), direction))
            {
                return false;
            }
        }

        // Try to move this box again
    }

    // Blocked
    return false;
}

void GameWorld::MarkWorldAsChanged()
{
    m_needToUpdate = true;
    m_updateTimer = 0.0f;

    // Queue up entities to be updated
    // List of entities will be sorted according to vertical component of gravity vector
    m_entityUpdateQueue.clear();
    for (const auto& entity : m_entities)
    {
        m_entityUpdateQueue.push_back(entity.get());
    }

    switch (m_currentGravityDirection)
    {
    case GravityDirection::Down:
        std::sort(m_entityUpdateQueue.begin(), m_entityUpdateQueue.end(), [](const Entity* left, const Entity* right)
        {
            return left->GetGridPosition().y < right->GetGridPosition().y;
        });
        break;
    case GravityDirection::Left:
        std::sort(m_entityUpdateQueue.begin(), m_entityUpdateQueue.end(), [](const Entity* left, const Entity* right)
        {
            return left->GetGridPosition().x < right->GetGridPosition().x;
        });
        break;
    case GravityDirection::Up:
        std::sort(m_entityUpdateQueue.begin(), m_entityUpdateQueue.end(), [](const Entity* left, const Entity* right)
        {
            return left->GetGridPosition().y > right->GetGridPosition().y;
        });
        break;
    case GravityDirection::Right:
        std::sort(m_entityUpdateQueue.begin(), m_entityUpdateQueue.end(), [](const Entity* left, const Entity* right)
        {
            return left->GetGridPosition().x > right->GetGridPosition().x;
        });
        break;
    }
}

void GameWorld::WorldStepOnce()
{
    m_needToUpdate = ApplyGravity();

    if (!m_needToUpdate)
    {
        ProcessEntityRemovePendingList();
    }
}

bool GameWorld::ApplyGravity()
{
    Vector2Int gravityVector = GravityDirection2Vector(m_currentGravityDirection);

    bool needToUpdate = false;
    for (auto entity : m_entityUpdateQueue)
    {
        if (!entity->IsValid())
        {
            continue;
        }

        Vector2Int newPosition = entity->GetGridPosition() + gravityVector;
        if (TryMoveEntityUnderGravity(entity, newPosition))
        {
            needToUpdate = true;
        }
    }

    return needToUpdate;
}


void GameWorld::MarkEntityAsRemoved(Entity* entity)
{
    entity->Invalidate();

    m_entityRemovePendingList.push_back(entity);
}

void GameWorld::ProcessEntityRemovePendingList()
{
    if (m_entityRemovePendingList.empty())
    {
        return;
    }

    for (auto entityTobeRemoved : m_entityRemovePendingList)
    {
        auto iterator = std::find_if(m_entities.begin(), m_entities.end(), [entityTobeRemoved](const std::unique_ptr<Entity>& entity)
        {
            return entity.get() == entityTobeRemoved;
        });

        m_entities.erase(iterator);
    }

    m_entityRemovePendingList.clear();
}

size_t GameWorld::GridPos2Index(Vector2Int position) const
{
    return position.x + (size_t)m_mapWidth * position.y;
}

constexpr Vector2Int GameWorld::GravityDirection2Vector(GravityDirection gravityDirection)
{
    constexpr std::array<Vector2Int, 4> directionToVector
    {
        Vector2Int::Down(),
        Vector2Int::Left(),
        Vector2Int::Up(),
        Vector2Int::Right()
    };

    return directionToVector[(size_t)gravityDirection];
}
