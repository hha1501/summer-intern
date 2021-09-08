#include "KeyEntity.h"

KeyEntity::KeyEntity(std::unique_ptr<Sprite2D> sprite) : Entity(EntityType::Key, std::move(sprite))
{
}
