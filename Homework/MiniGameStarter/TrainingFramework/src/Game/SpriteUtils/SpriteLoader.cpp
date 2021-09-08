#include "SpriteLoader.h"

#include "../GameWorldConfig.h"
#include "SpriteResourceConfig.h"

#include "Sprite2D.h"
#include "AtlasSprite2D.h"

#include <assert.h>

using namespace GameWorldConfig;

SpriteLoader::SpriteLoader()
{
    auto resourceManager = ResourceManagers::GetInstance();

    m_spriteModel = resourceManager->GetModel("Sprite2D-cartesian.nfg");
    m_spriteShader = resourceManager->GetShader("TextureShader");
    m_atlasSpriteShader = resourceManager->GetShader("AtlasTextureShader");
}

std::unique_ptr<Sprite2D> SpriteLoader::LoadTileSprite(TileType tileType, uint8_t tileDetails, Vector2Int position) const
{
    auto resourceManager = ResourceManagers::GetInstance();

    const char* texturePath = nullptr;
    switch (tileType)
    {
    case TileType::Wall:
        if (tileDetails == 1)
        {
            texturePath = SpriteResourceConfig::TileWall1;
        }
        else
        {
            texturePath = SpriteResourceConfig::TileWall0;
        }
        break;
    case TileType::Exit:
        texturePath = SpriteResourceConfig::TileDoor;
        break;
    default:
        assert(false && "Unknown tile type");
        break;
    }

    std::shared_ptr<Texture> tileTexture = resourceManager->GetTexture(texturePath, true);
    std::unique_ptr<Sprite2D> tileSprite;

    if (tileType == TileType::Exit)
    {
        tileSprite = std::make_unique<AtlasSprite2D>(m_spriteModel, m_atlasSpriteShader, tileTexture, Vector2Int(2, 1));
    }
    else
    {
        tileSprite = std::make_unique<Sprite2D>(m_spriteModel, m_spriteShader, tileTexture);
    }

    tileSprite->SetSize(c_tileSize, c_tileSize);
    tileSprite->Set2DPosition(Vector2(position.x + c_tileAlign, position.y + c_tileAlign));

    return tileSprite;
}

std::unique_ptr<Sprite2D> SpriteLoader::LoadEntitySprite(EntityType entityType, Vector2Int position) const
{
    auto resourceManager = ResourceManagers::GetInstance();

    std::unique_ptr<Sprite2D> entitySprite;

    switch (entityType)
    {
    case EntityType::Player:
        entitySprite = std::make_unique<AtlasSprite2D>(
            m_spriteModel,
            m_atlasSpriteShader,
            resourceManager->GetTexture(SpriteResourceConfig::PlayerEntity, true),
            Vector2Int(6, 2));
        break;
    case EntityType::Rock:
        entitySprite = std::make_unique<Sprite2D>(
            m_spriteModel,
            m_spriteShader,
            resourceManager->GetTexture(SpriteResourceConfig::RockEntity, true));
        break;
    case EntityType::Box:
        entitySprite = std::make_unique<Sprite2D>(
            m_spriteModel,
            m_spriteShader,
            resourceManager->GetTexture(SpriteResourceConfig::BoxEntity, true));
        break;
    case EntityType::Key:
        entitySprite = std::make_unique<Sprite2D>(
            m_spriteModel,
            m_spriteShader,
            resourceManager->GetTexture(SpriteResourceConfig::KeyEntity, true));
        break;
    default:
        assert(false && "Unknown entity type");
        break;
    }

    entitySprite->SetSize(c_tileSize, c_tileSize);
    entitySprite->Set2DPosition(Vector2(position.x + c_tileAlign, position.y + c_tileAlign));

    return entitySprite;
}
