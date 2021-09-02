#include "Entity.h"

#include "../GameWorldConfig.h"

#include "ResourceManagers.h"

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

void Entity::LoadSprite(const char* texture, bool isAnimated)
{
    auto resourceManager = ResourceManagers::GetInstance();

    std::shared_ptr<Model> spriteModel = resourceManager->GetModel("Sprite2D-cartesian.nfg");
    std::shared_ptr<Shader> spriteShader = isAnimated ? resourceManager->GetShader("AnimationTextureShader") : resourceManager->GetShader("TextureShader");
    std::shared_ptr<Texture> tileTexture = resourceManager->GetTexture(texture, true);

    m_sprite = std::make_unique<Sprite2D>(spriteModel, spriteShader, tileTexture);
    m_sprite->SetSize(c_tileSize, c_tileSize);
}
