#pragma once

#include "TileType.h"

#include "../Common/Vector2Int.h"

#include "ResourceManagers.h"
#include "Sprite2D.h"

class TileSpriteLoader
{
public:
    TileSpriteLoader(bool isAnimated = false);

    std::unique_ptr<Sprite2D> LoadSprite(TileType tileType, uint8_t tileDetails, Vector2Int position);

private:
    std::shared_ptr<Model> m_spriteModel;
    std::shared_ptr<Shader> m_spriteShader;
};
