#include "Entity.h"

#include "../GameWorldConfig.h"

#include "ResourceManagers.h"

#include "Sprite2D.h"
#include "AtlasSprite2D.h"

using namespace GameWorldConfig;

void Entity::SetPosition(Vector2Int gridPosition)
{
    m_gridPosition = gridPosition;
    m_sprite->Set2DPosition(Vector2(gridPosition.x + c_tileAlign, gridPosition.y + c_tileAlign));
}

void Entity::SetRotation(Vector3 rotation)
{
    m_sprite->SetRotation(rotation);
    m_sprite->CalculateWorldMatrix();
}

void Entity::SetCamera(std::shared_ptr<Camera> camera)
{
    m_sprite->SetCamera(camera);
}

void Entity::LoadSprite(const char* texture)
{
    auto resourceManager = ResourceManagers::GetInstance();

    std::shared_ptr<Model> spriteModel = resourceManager->GetModel("Sprite2D-cartesian.nfg");
    std::shared_ptr<Texture> tileTexture = resourceManager->GetTexture(texture, true);

    std::shared_ptr<Shader> spriteShader = resourceManager->GetShader("TextureShader");
    m_sprite = std::make_unique<Sprite2D>(spriteModel, spriteShader, tileTexture);

    m_sprite->SetSize(c_tileSize, c_tileSize);
}

void Entity::LoadAtlasSprite(const char* texture, Vector2Int atlasSize)
{
    auto resourceManager = ResourceManagers::GetInstance();

    std::shared_ptr<Model> spriteModel = resourceManager->GetModel("Sprite2D-cartesian.nfg");
    std::shared_ptr<Texture> tileTexture = resourceManager->GetTexture(texture, true);

    std::shared_ptr<Shader> spriteShader = resourceManager->GetShader("AtlasTextureShader");
    m_sprite = std::make_unique<AtlasSprite2D>(spriteModel, spriteShader, tileTexture, atlasSize);

    m_sprite->SetSize(c_tileSize, c_tileSize);
}
