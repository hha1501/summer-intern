#include "RockEntity.h"

RockEntity::RockEntity(std::unique_ptr<Sprite2D> sprite) : Entity(EntityType::Rock, std::move(sprite))
{
}
