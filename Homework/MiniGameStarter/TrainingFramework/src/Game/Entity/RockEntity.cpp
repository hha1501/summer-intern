#include "RockEntity.h"

RockEntity::RockEntity() : Entity(EntityType::Rock)
{
    LoadSprite("objects\\rock head.tga");
}
