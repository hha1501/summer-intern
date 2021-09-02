#include "TileSpriteLoader.h"

#include "../GameWorldConfig.h"

#include <assert.h>

using namespace GameWorldConfig;

TileSpriteLoader::TileSpriteLoader(bool isAnimated)
{
    auto resourceManager = ResourceManagers::GetInstance();

    m_spriteModel = resourceManager->GetModel("Sprite2D-cartesian.nfg");
    m_spriteShader = isAnimated ? resourceManager->GetShader("AnimationTextureShader") : resourceManager->GetShader("TextureShader");
}

std::unique_ptr<Sprite2D> TileSpriteLoader::LoadSprite(TileType tileType, uint8_t tileDetails, Vector2Int position)
{
    auto resourceManager = ResourceManagers::GetInstance();

    const char* texturePath = nullptr;
    switch (tileType)
    {
    case TileType::Wall:
        if (tileDetails == 1)
        {
            texturePath = "tiles\\Floor_Tiled_Cracked.tga";
        }
        else
        {
            texturePath = "tiles\\Dungeon-2D-PixelArt-Tileset_5.tga";
        }
        break;
    case TileType::Exit:
        texturePath = "tiles\\Top_Middle_Dungeon_Door_Closed.tga";
        break;
    default:
        assert(false && "Unknown tile type");
        break;
    }

    std::shared_ptr<Texture> tileTexture = resourceManager->GetTexture(texturePath, true);

    std::unique_ptr<Sprite2D> tileSprite = std::make_unique<Sprite2D>(m_spriteModel, m_spriteShader, tileTexture);
    tileSprite->SetSize(c_tileSize, c_tileSize);
    tileSprite->Set2DPosition(Vector2(position.x + c_tileAlign, position.y + c_tileAlign));

    return tileSprite;
}
